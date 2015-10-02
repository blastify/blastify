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

#ifndef INCLUDE_BLASTIFY_EVENT_H_
#define INCLUDE_BLASTIFY_EVENT_H_

#include <inttypes.h>
#include <time.h>
#include <map>
#include <sstream>
#include <string>

namespace blastify {

class Buffer;

class Event {
 public:
  // Create the empty event. Defaults to use Now() time stamp.
  explicit Event(bool init_with_now = true);

  // Set the timestamp in nanoseconds since the epoch.
  Event& set_timestamp(int64_t epoch_nanos);

  // Get the timestamp in nanoseconds since the epoch.
  int64_t timestamp() const;

  // Sets a value for a named field.
  template <typename ValueType>
  Event& SetField(const std::string& field_name, const ValueType& value) {
    std::stringstream tmp;
    tmp << value;
    data_.insert(data_type::value_type(field_name, tmp.str()));
    return *this;
  }

  // Return 'true' if the event has the specified field, false otherwise.
  bool HasField(const std::string& field_name) const;

  // Return the value for a field, or the empty string.
  const std::string& GetField(const std::string& field_name) const;

  // Get the current timestamp in nanoseconds since the epoch.
  static int64_t Now();

  // Serialize an event to a buffer.
  // Returns 'true' on success, 'false' if there was an invalid argument.
  static bool EventToBuffer(const Event& event, Buffer* buffer);

  // Deserialize an event from a buffer.
  // Returns 'true' on success, 'false' if there was an invalid arguments.
  static bool BufferToEvent(const Buffer& buffer, Event* event);

 private:
  typedef std::map<std::string, std::string> data_type;
  int64_t timestamp_;
  data_type data_;
};

}  // namespace blastify

#endif  // INCLUDE_BLASTIFY_EVENT_H_
