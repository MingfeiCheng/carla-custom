// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "carla/geom/GeoLocation.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/BoundingBox.h"
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
      rpc::ActorId id;
      std::string type;
      geom::BoundingBox bbox;
      geom::Location location;
      geom::Rotation rotation;
      geom::Vector3D velocity;
      float speed;
      geom::Vector3D acceleration;
      geom::Vector3D angular_velocity;
      geom::GeoLocation geo_location;
      float qw;
      float qx;
      float qy;
      float qz;
      rpc::VehicleControl control;
      MSGPACK_DEFINE_ARRAY(actor, geo_location, qw, qx, qy, qz)
    };

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &sensor,
      const rpc::ActorId id,
      const std::string type,
      const geom::BoundingBox bbox,
      const geom::Location location,
      const geom::Rotation rotation,
      const geom::Vector3D velocity,
      const float speed,
      const geom::Vector3D acceleration,
      const geom::Vector3D angular_velocity,
      const geom::GeoLocation geo_location,
      const float qw,
      const float qx,
      const float qy,
      const float qz,
      const rpc::VehicleControl control);

    static Data DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<Data>(message.begin(), message.size());
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename SensorT>
  inline Buffer ApolloStateSerializer::Serialize(
      const SensorT &,
      const rpc::ActorId id,
      const std::string type,
      const geom::BoundingBox bbox,
      const geom::Location location,
      const geom::Rotation rotation,
      const geom::Vector3D velocity,
      const float speed,
      const geom::Vector3D acceleration,
      const geom::Vector3D angular_velocity,
      const geom::GeoLocation geo_location,
      const float qw,
      const float qx,
      const float qy,
      const float qz,
      const rpc::VehicleControl control) {
    return MsgPack::Pack(Data{id, 
                              type, 
                              bbox, 
                              location, 
                              rotation,
                              velocity,
                              speed,
                              acceleration,
                              angular_velocity,
                              geo_location,
                              qw,
                              qx,
                              qy,
                              qz,
                              control});
  }

} // namespace s11n
} // namespace sensor
} // namespace carla