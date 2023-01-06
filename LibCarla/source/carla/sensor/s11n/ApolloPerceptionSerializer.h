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
        const PerceptionUnitListT &detected_actors) {
      const uint32_t size_in_bytes = sizeof(rpc::PerceptionUnit) * detected_actors.Num();
      Buffer buffer{size_in_bytes};
      unsigned char *it = buffer.data();
      for (auto *actor : detected_actors) {
        std::memcpy(it, &actor, sizeof(rpc::PerceptionUnit));
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