// Copyright (c) 2020 Robotics and Perception Group (GPR)
// University of Zurich and ETH Zurich
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Debug.h"
#include "carla/rpc/ActorId.h"
#include "carla/client/Actor.h"
#include "carla/client/detail/ActorVariant.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/Transform.h"
#include "carla/geom/BoundingBox.h"

namespace carla {
namespace sensor {
namespace data {

#pragma pack(push, 1)
struct ApolloObstacle {
    /// Default constructor
    ApolloObstacle() = default;

    /// Copy Constructor
    ApolloObstacle(const ApolloObstacle &arg)
      : id(arg.id),
        type(arg.type),
        bbox(arg.bbox),
        rotation(arg.rotation),
        location(arg.location),
        velocity(arg.velocity),
        acceleration(arg.acceleration) {}

    /// Moving constructor
    ApolloObstacle(const ApolloObstacle &&arg)
      : id(std::move(arg.id)),
        type(std::move(arg.type)),
        bbox(std::move(arg.bbox)),
        rotation(std::move(arg.rotation)),
        location(std::move(arg.location)),
        velocity(std::move(arg.velocity)),
        acceleration(std::move(arg.acceleration)) {}

    /// Constructor
    ApolloObstacle(const client::detail::EpisodeProxy &episode, const rpc::Actor &actor) {

      id = actor.id;
      type = actor.description.id; //TODO:Check
      bbox = actor.bounding_box;

      const detail::ActorVariant actor_variant = detail::ActorVariant(actor);
      const SharedPtr<client::Actor> actor_client = actor_variant.Get(episode);

      const geom::Transform actor_transform = actor_client->GetTransform();
      const geom::Location actor_location = actor_transform.location;
      const geom::Rotation actor_rotation = actor_transform.rotation;
      const geom::Vector3D actor_velocity = actor_client->GetVelocity();
      const geom::Vector3D actor_acceleration = actor_client->GetAcceleration();

      rotation = actor_rotation;
      location = geom::Location(actor_location.x, -actor_location.y, actor_location.z);
      velocity = geom::Vector3D(actor_velocity.x, -actor_velocity.y, actor_velocity.z);
      acceleration = geom::Vector3D(actor_acceleration.x, -actor_acceleration.y, actor_acceleration.z);
    }

    /// Assignement operator
    ApolloObstacle &operator=(const ApolloObstacle &other) {
      id = other.id;
      type = other.type;
      bbox = other.bbox;
      rotation = other.rotation;
      location = other.location;
      velocity = other.velocity;
      acceleration = other.acceleration;
      return *this;
    }

    /// Move Assignement operator
    ApolloObstacle &operator=(const ApolloObstacle &&other) {
      id = std::move(other.id);
      type = std::move(other.type);
      bbox = std::move(other.bbox);
      rotation = std::move(other.rotation);
      location = std::move(other.location);
      velocity = std::move(other.velocity);
      acceleration = std::move(other.acceleration);
      return *this;
    }

    bool operator==(const ApolloObstacle &rhs) const {
      return (id == rhs.id) && (type == rhs.type);
    }

    bool operator!=(const ApolloObstacle &rhs) const {
      return !(*this == rhs);
    }

    rpc::ActorId id;
    std::string type;
    geom::BoundingBox bbox;
    geom::Rotation rotation;
    geom::Location location;
    geom::Vector3D velocity;
    geom::Vector3D acceleration;
  
  };
#pragma pack(pop)
} // namespace data
} // namespace sensor
} // namespace carla
