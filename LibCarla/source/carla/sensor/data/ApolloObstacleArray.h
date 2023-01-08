// #pragma once

// #include "carla/sensor/data/Array.h"
// #include "carla/sensor/data/ApolloObstacle.h"
// #include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

// namespace carla {
// namespace sensor {
// namespace data {

//   class ApolloObstacleArray : public Array<ApolloObstacle> {
    
//     using Super = Array<ApolloObstacle>;

//   public:

//     // using Serializer = s11n::ApolloPerceptionSerializer;

//     // friend Serializer;

//     explicit ApolloObstacleArray(RawData &&data)
//       : Super(0u, std::move(data)) {
//         // how to put in this
//       }

//   // public:

//     // std::vector<SharedPtr<client::Actor>> GetActors() const {
//     //   std::vector<SharedPtr<client::Actor>> actors = std::vector<SharedPtr<client::Actor>>();
//     //   for (const auto &actor_rpc : *this) {
//     //     const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(actor_rpc);
//     //     actors.push_back(actor_variant.Get(GetEpisode()));
//     //   }
//     //   return actors;
//     // }

//     // SharedPtr<client::Actor> GetActor(size_t pos) const {
//     //   const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(this->at(pos));
//     //   return actor_variant.Get(GetEpisode());
//     // }
//   };

// } // namespace data
// } // namespace sensor
// } // namespace carla

#pragma once

// #include "carla/sensor/data/Array.h"

#include "carla/sensor/data/ApolloObstacle.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloObstacleArray : public SensorData {
    
    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloPerceptionSerializer;
    using ObstacleArray = std::vector<data::ApolloObstacle>;

    friend Serializer;

    explicit ApolloObstacleArray(const RawData &&data)
      : Super(data) {
        // how to put in this
        ObstacleArray obstacles = Serializer::DeserializeRawData(data);
        _obstacles = obstacles;
      }

  public:
    auto GetSize() const {
      return _obstacles.size();
    }

  private:
    ObstacleArray _obstacles;

  };

} // namespace data
} // namespace sensor
} // namespace carla
