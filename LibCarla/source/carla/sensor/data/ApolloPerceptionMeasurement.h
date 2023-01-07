// // Copyright (c) 2019 Intel Labs.
// //
// // This work is licensed under the terms of the MIT license.
// // For a copy, see <https://opensource.org/licenses/MIT>.

// #pragma once

// #include "carla/client/detail/ActorVariant.h"
// #include "carla/client/Actor.h"
// #include "carla/rpc/Actor.h"
// #include "carla/sensor/data/Array.h"
// #include "carla/sensor/SensorData.h"

// #include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

// namespace carla {
// namespace sensor {
// namespace data {

//   class ApolloPerceptionMeasurement : public Array<rpc::Actor> {

//   protected:
//     using Serializer = s11n::ApolloPerceptionSerializer;

//     friend Serializer;

//     explicit ApolloPerceptionMeasurement(RawData &&data):
//       Array<rpc::Actor>(0u, std::move(data)) {
      
//       // actors_rpc = Array<rpc::Actor>(0u, std::move(data));
//       // actors = std::vector<SharedPtr<client::Actor>>();

//       std::vector<SharedPtr<client::Actor>> _actors = std::vector<SharedPtr<client::Actor>>();
//       for (auto actor_data : cbegin()) {
//         const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(Serializer.DeserializeRawData(actor_data));
//         SharedPtr<client::Actor> actor_client = actor_variant.Get(GetEpisode());
//         _actors.push_back(actor_client);
//       }

//       // const uint32_t size_in_bytes = sizeof(SharedPtr<client::Actor>) * actors_rpc.size();
//       // Buffer buffer{size_in_bytes};
//       // unsigned char *it = buffer.data();
//       // for (pos = 0; pos < actors_rpc.size(); pos++) {
//       //   const actor_variant = client::detail::ActorVariant(Serializer.DeserializeRawData(actors_rpc.at(pos)));
//       //   SharedPtr<client::Actor> actor_client = actor_variant.Get(GetEpisode())
//       //   std::memcpy(it, &actor_client, sizeof(SharedPtr<client::Actor>));
//       //   it += sizeof(SharedPtr<client::Actor>);
//       // }
//       // return buffer;

//       // for (pos = 0; pos < actors_rpc.size(); pos++){
        
//       //   actors.
//       // }
//     }

//   public:

//     std::vector<SharedPtr<client::Actor>> GetActors() const {
//       return _actors;
//     }

//     SharedPtr<client::Actor> GetActor(size_t pos) const {
//       return _actors[pos];
//     }

//   };

//   private:
    
//     std::vector<SharedPtr<client::Actor>> _actors;

// } // namespace data
// } // namespace sensor
// } // namespace carla


// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/client/detail/ActorVariant.h"
#include "carla/client/Actor.h"
#include "carla/rpc/Actor.h"
#include "carla/sensor/data/Array.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloPerceptionMeasurement : public Array<rpc::Actor> {
    
    using Super = Array<rpc::Actor>;

  protected:

    using Serializer = s11n::ApolloPerceptionSerializer;

    friend Serializer;

    explicit ApolloPerceptionMeasurement(RawData &&data)
      : Super(0u, std::move(data)) {}

  public:

    std::vector<SharedPtr<client::Actor>> GetActors() const {
      std::vector<SharedPtr<client::Actor>> actors = std::vector<SharedPtr<client::Actor>>();
      for (const auto &actor_rpc : *this) {
        const client::detail::ActorVariant actor_variant = client::detail::ActorVariant(actor_rpc);
        actors.push_back(actor_variant.Get(GetEpisode()));
      }
      return actors;
    }

    // SharedPtr<client::Actor> GetActor(size_t pos) const {
    //   return _actors[pos];
    // }
  };

} // namespace data
} // namespace sensor
} // namespace carla
