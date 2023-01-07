#pragma once

#include "carla/sensor/data/Array.h"
#include "carla/sensor/data/ApolloObstacle.h"
#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloObstacleArray : public Array<ApolloObstacle> {
    
    using Super = Array<ApolloObstacle>;

  public:

    // using Serializer = s11n::ApolloPerceptionSerializer;

    // friend Serializer;

    explicit ApolloObstacleArray(RawData &&data)
      : Super(0u, std::move(data)) {
        // how to put in this
      }

  // public:

    // std::vector<SharedPtr<client::Actor>> GetActors() const {
    //   std::vector<SharedPtr<client::Actor>> actors = std::vector<SharedPtr<client::Actor>>();
    //   for (const auto &actor_rpc : *this) {
    //     const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(actor_rpc);
    //     actors.push_back(actor_variant.Get(GetEpisode()));
    //   }
    //   return actors;
    // }

    // SharedPtr<client::Actor> GetActor(size_t pos) const {
    //   const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(this->at(pos));
    //   return actor_variant.Get(GetEpisode());
    // }
  };

} // namespace data
} // namespace sensor
} // namespace carla
