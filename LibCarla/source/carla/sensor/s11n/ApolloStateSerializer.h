// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

// #pragma once

// #include "carla/Buffer.h"
// #include "carla/Memory.h"
// #include "carla/rpc/Actor.h"
// #include "carla/sensor/RawData.h"

// #include <cstdint>
// #include <cstring>

// namespace carla {
// namespace sensor {

//   class SensorData;

// namespace s11n {

//   class ApolloStateSerializer {
//   public:

//     static rpc::Actor DeserializeRawData(const RawData &message) {
//       return MsgPack::UnPack<rpc::Actor>(message.begin(), message.size());
//     }

//     template <typename SensorT>
//     static Buffer Serialize(
//         const SensorT &,
//         const rpc::Actor &actorState
//         ) {
//       return MsgPack::Pack(actorState);
//     }

//     static SharedPtr<SensorData> Deserialize(RawData &&data);
//   };

// } // namespace s11n
// } // namespace sensor
// } // namespace carla

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
#include "carla/rpc/VehicleControl.h"
#include "carla/sensor/RawData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloStateSerializer {
  public:

    struct Data {
      geom::GeoLocation geo_location;
      geom::Location location;
      geom::Rotation rotation;
      float qw;
      float qx;
      float qy;
      float qz;
      rpc::VehicleControl control;
      geom::Vector3D acceleration;
      geom::Vector3D angular_velocity;
      geom::Vector3D linear_velocity;
      float speed;
      MSGPACK_DEFINE_ARRAY(geo_location, location, rotation, qw, qx, qy, qz, control, acceleration, angular_velocity, linear_velocity, speed)
    };

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &sensor,
      const geom::GeoLocation &geo_location,
      const geom::Location &location,
      const geom::Rotation &rotation,
      const float qw,
      const float qx,
      const float qy,
      const float qz,
      rpc::VehicleControl &control,
      geom::Vector3D &acceleration,
      geom::Vector3D &angular_velocity,
      geom::Vector3D &linear_velocity,
      float speed);

    static Data DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<Data>(message.begin(), message.size());
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename SensorT>
  inline Buffer ApolloStateSerializer::Serialize(
      const SensorT &,
      const geom::GeoLocation &geo_location,
      const geom::Location &location,
      const geom::Rotation &rotation,
      const float qw,
      const float qx,
      const float qy,
      const float qz,
      rpc::VehicleControl &control,
      geom::Vector3D &acceleration,
      geom::Vector3D &angular_velocity,
      geom::Vector3D &linear_velocity,
      float speed) {
    return MsgPack::Pack(Data{geo_location, location, rotation, qw, qx, qy, qz, control, acceleration, angular_velocity, linear_velocity});
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
