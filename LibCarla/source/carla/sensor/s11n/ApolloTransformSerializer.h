// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "carla/sensor/RawData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloTransformSerializer {
  public:

    struct Data {
      geom::Location location;
      float qw;
      MSGPACK_DEFINE_ARRAY(location, qw)
    };

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &sensor,
      const geom::Location &location,
      const float qw);

    static Data DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<Data>(message.begin(), message.size());
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename SensorT>
  inline Buffer ApolloTransformSerializer::Serialize(
      const SensorT &,
      const geom::Location &location,
      const float qw) {
    return MsgPack::Pack(Data{location, qw});
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
