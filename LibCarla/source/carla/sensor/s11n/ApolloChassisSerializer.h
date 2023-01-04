// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "Carla/Vehicle/VehicleControl.h"
#include "carla/sensor/RawData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloChassisSerializer {
  public:

    static FVehicleControl DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<FVehicleControl>(message.begin(), message.size());
    }

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &,
      const FVehicleControl &control) {
      return MsgPack::Pack(control);
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };


} // namespace s11n
} // namespace sensor
} // namespace carla
