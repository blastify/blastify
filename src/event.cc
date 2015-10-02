// Copyright 2015 The Blastify Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License. See the AUTHORS file for names of
// contributors.

#include "blastify/event.h"
#include <assert.h>
#include <string.h>
#include "blastify/buffer.h"
#include "blastify/slice.h"

namespace {

static const char kBlastifyMagic[] = {'f', 'd', 's', '0'};

static const int kBlastifyMagicSize = sizeof(kBlastifyMagic);

}  // namespace

namespace blastify {

Event::Event(bool init_with_now) : timestamp_(init_with_now ? Now() : 0) {}

Event& Event::set_timestamp(int64_t epoch_nanos) {
  timestamp_ = epoch_nanos;
  return *this;
}

int64_t Event::timestamp() const { return timestamp_; }

bool Event::HasField(const std::string& field_name) const {
  const data_type::const_iterator i = data_.find(field_name);
  return (i != data_.end());
}

const std::string& Event::GetField(const std::string& field_name) const {
  static const std::string empty("");
  data_type::const_iterator i = data_.find(field_name);
  return ((i != data_.end()) ? i->second : empty);
}

int64_t Event::Now() {
  struct timespec ts = {0};
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ts.tv_sec * 1000000000) + ts.tv_nsec;
}

bool Event::EventToBuffer(const Event& event, Buffer* buffer) {
  assert(buffer != NULL);
  if (buffer == NULL) {
    return false;
  }

  // Append the magic header to the message.
  buffer->Append(kBlastifyMagic, sizeof(kBlastifyMagic));

  // Append the time stamp.
  // TODO(tdial): Deal with endianness.
  buffer->Append((const char*)(&(event.timestamp_)), sizeof(event.timestamp_));

  // App

  return true;
}

bool Event::BufferToEvent(const Buffer& buffer, Event* event) {
  assert(event != NULL);
  if (event == NULL) {
    return false;
  }

  // Create a slice, which may be considered to be a sliding window into
  // the buffer that we can advance cheaply without any copying.
  Slice slice(buffer);

  // First, ensure that we have enough data to hold the magic value.
  if (slice.size() < kBlastifyMagicSize) {
    return false;
  }

  // Check that the magic number sequence matches. If not, bail out.
  if (0 != memcmp(kBlastifyMagic, slice.data(), kBlastifyMagicSize)) {
    return false;
  }

  // Advance slice past the magic header.
  slice.RemovePrefix(kBlastifyMagicSize);

  // Next, ensure we have enough for the time stamp.
  if (slice.size() < sizeof(int64_t)) {
    return false;
  }

  // Copy time stamp directly into the user's event
  memcpy(&(event->timestamp_), slice.data(), sizeof(int64_t));

  // Advance past the timestamp portion of the message.
  slice.RemovePrefix(sizeof(int64_t));

  return true;
}

}  // namespace blastify
