// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "carla/rpc/Actor.h"
#include "carla/rpc/PerceptionUnit.h"
#include "carla/sensor/RawData.h"
#include "carla/client/detail/ActorVariant.h"
#include "carla/client/Actor.h"
#include "carla/geom/BoundingBox.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/rpc/ActorId.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloPerceptionSerializer {
  public:

    template <typename SensorT, typename EpisodeT, typename PerceptionUnitListT>
    static Buffer Serialize(
        const SensorT &,
        const EpisodeT &episode,
        const ActorListT &detected_actors) {
      const uint32_t size_in_bytes = sizeof(rpc::PerceptionUnit) * detected_actors.Num();
      Buffer buffer{size_in_bytes};
      unsigned char *it = buffer.data();
      for (auto *actor : detected_actors) {
        const carla::rpc::Actor actor_info = episode.SerializeActor(actor);
        carla::rpc::ActorId actor_id = actor_info.id;
        carla::geom::BoundingBox actor_bbox = actor_info.bounding_box;
        std::string actor_type = actor_info.description.id; //TODO:Check
        const carla::client::detail::ActorVariant actor_variant = carla::client::detail::ActorVariant(actor_info);
        const carla::SharedPtr<carla::client::Actor> actor_ptr = actor_variant.Get(episode);

        const carla::geom::Location actor_location = actor_ptr.GetLocation();
        const carla::geom::Rotation actor_rotation = actor_ptr.GetRotation();
        const carla::geom::Vector3D actor_velocity = actor_ptr.GetVelocity();
        const carla::geom::Vector3D actor_acceleration = actor_ptr.GetAcceleration();

        const carla::geom::Location apollo_actor_location = carla::geom::Location(actor_location.x, -actor_location.y, actor_location.z);
        const carla::geom::Vector3D apollo_actor_velocity = carla::geom::Vector3D(actor_velocity.x, -actor_velocity.y, actor_velocity.z);
        const carla::geom::Vector3D apollo_actor_acceleration = carla::geom::Vector3D(actor_acceleration.x, -actor_acceleration.y, actor_acceleration.z);

        const carla::rpc::PerceptionUnit actor_perception_unit = carla::rpc::PerceptionUnit(actor_id, actor_bbox, actor_type, actor_location, actor_rotation, actor_velocity, actor_acceleration);

        std::memcpy(it, &actor_perception_unit, sizeof(rpc::PerceptionUnit));
        it += sizeof(rpc::PerceptionUnit);
      }
      return buffer;
      // use CarlaEpisode->SerializeActor(AActor* Actor)
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

} // namespace s11n
} // namespace sensor
} // namespace carla