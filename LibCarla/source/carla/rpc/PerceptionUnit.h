// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/MsgPack.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/BoundingBox.h"
#include "carla/rpc/ActorId.h"
#include "carla/rpc/ActorDescription.h"

namespace carla {
namespace rpc {

  class PerceptionUnit {
  public:

    PerceptionUnit() = default;

    PerceptionUnit(
        carla::rpc::ActorId id,
        geom::BoundingBox bbox,
        std::string type,
        geom::Location location,
        geom::Rotation rotation,
        geom::Vector3D velocity,
        geom::Vector3D acceleration)
      : id(id),
        bbox(bbox),
        type(type),
        location(location),
        rotation(rotation),
        velocity(velocity),
        acceleration(acceleration) {}

    carla::rpc::ActorId id;
    geom::BoundingBox bbox;
    std::string type;
    geom::Location location;
    geom::Rotation rotation;
    geom::Vector3D velocity;
    geom::Vector3D acceleration;

    MSGPACK_DEFINE_ARRAY(
        id,
        bbox,
        type,
        location,
        rotation,
        velocity,
        acceleration);
  };

} // namespace rpc
} // namespace carla
