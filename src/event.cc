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

namespace blastify {

Event::Event(bool init_with_now) : timestamp_(init_with_now ? Now() : 0) {}

Event& Event::set_timestamp(int64_t epoch_nanos) {
  timestamp_ = epoch_nanos;
  return *this;
}

int64_t Event::timestamp() const { return timestamp_; }

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

}  // namespace blastify
