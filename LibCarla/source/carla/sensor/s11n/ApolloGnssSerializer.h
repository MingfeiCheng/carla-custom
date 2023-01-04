// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "carla/geom/GeoLocation.h"
#include "carla/rpc/ActorId.h"
#include "carla/sensor/RawData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloGnssSerializer {
  public:

    struct Data {
      geom::GeoLocation geo_location;
      geom::Location location;
      geom::Rotation rotation;
      MSGPACK_DEFINE_ARRAY(geo_location, location, rotation)
    };

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &sensor,
      const geom::GeoLocation &geo_location,
      const geom::Location &location,
      const geom::Rotation &rotation);

    static Data DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<Data>(message.begin(), message.size());
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename SensorT>
  inline Buffer ApolloGnssSerializer::Serialize(
      const SensorT &,
      const geom::GeoLocation &geo_location,
      const geom::Location &location,
      const geom::Rotation &rotation) {
    return MsgPack::Pack(Data{geo_location, location, rotation});
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
