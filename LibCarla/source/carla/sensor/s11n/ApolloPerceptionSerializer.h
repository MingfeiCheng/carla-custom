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

    static ApolloObstacleArray DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<ApolloObstacleArray>(message.begin(), message.size());
    }
    
    template <typename SensorT>
    static Buffer Serialize(
        const SensorT &,
        const ApolloObstacleArray &obstacles
        ) {
      return MsgPack::Pack(obstacles);
    }
  
    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

} // namespace s11n
} // namespace sensor
} // namespace carla