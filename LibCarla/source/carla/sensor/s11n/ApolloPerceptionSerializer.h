// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "carla/sensor/RawData.h"
#include "carla/sensor/data/ApolloObstacle.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {


  class ApolloPerceptionSerializer {
  public:
    using ApolloObstacleArray = std::vector<data::ApolloObstacle>;
    template <typename Sensor>
    static Buffer Serialize(const Sensor &sensor, const ApolloObstacleArray &obstacles, Buffer &&output);
    static SharedPtr<SensorData> Deserialize(RawData &&data);

    // template <typename SensorT>
    // static Buffer Serialize(
    //     const SensorT &,
    //     const ApolloObstacleArray &obstacles) {
    //   size_t obstacles_length = obstacles.size();
    //   const uint32_t size_in_bytes = sizeof(data::ApolloObstacle) * obstacles_length;
    //   Buffer buffer{size_in_bytes};
    //   unsigned char *it = buffer.data();
    //   for (auto obstacle : obstacles) {
    //     // const FCarlaActor carla_actor = episode.FindCarlaActor(actor);
    //     std::memcpy(it, &obstacle, sizeof(data::ApolloObstacle));
    //     it += sizeof(data::ApolloObstacle);
    //   }
    //   return buffer;
    //   // use CarlaEpisode->SerializeActor(AActor* Actor)
    // }

    // template <typename SensorT, typename EpisodeT, typename ActorListT>
    // static Buffer Serialize(
    //     const SensorT &,
    //     const EpisodeT &episode,
    //     const ActorListT &detected_actors) {
    //   const uint32_t size_in_bytes = sizeof(data::ApolloObstacle) * detected_actors.Num();
    //   Buffer buffer{size_in_bytes};
    //   unsigned char *it = buffer.data();
    //   for (auto *actor : detected_actors) {
    //     // const FCarlaActor carla_actor = episode.FindCarlaActor(actor);
    //     const rpc::Actor actor_rpc = episode.SerializeActor(actor);
    //     const auto ceps = GetCurrentEpisode();
    //     const data::ApolloObstacle obstacle = data::ApolloObstacle(ceps, actor_rpc);
    //     std::memcpy(it, &obstacle, sizeof(data::ApolloObstacle));
    //     it += sizeof(data::ApolloObstacle);
    //   }
    //   return buffer;
    //   // use CarlaEpisode->SerializeActor(AActor* Actor)
    // }

    // static data::ApolloObstacle DeserializeRawData(const RawData &message){
    //   return MsgPack::UnPack<data::ApolloObstacle>(message.begin(), message.size());
    // }
  };

  template <typename Sensor>
  inline Buffer ApolloPerceptionSerializer::Serialize(const Sensor &sensor, const ApolloObstacleArray &obstacles, Buffer &&output) {

    /// Reset the output buffer
    output.reset((obstacles.size() * sizeof(data::ApolloObstacle)));

    /// Pointer to data in buffer
    unsigned char *it = output.data();

    /// Copy the events into the output buffer
    for (auto e : obstacles) {
      std::memcpy(it, reinterpret_cast<const void *>(&e), sizeof(data::ApolloObstacle));
      it += sizeof(data::ApolloObstacle);
    }
    return std::move(output);
  }

} // namespace s11n
} // namespace sensor
} // namespace carla