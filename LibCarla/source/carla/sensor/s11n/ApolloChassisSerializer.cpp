// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/sensor/s11n/ApolloChassisSerializer.h"

//TODO: change data format
#include "carla/sensor/data/ApolloChassisMeasurement.h"

namespace carla {
namespace sensor {
namespace s11n {

  SharedPtr<SensorData> ApolloChassisSerializer::Deserialize(RawData &&data) {
    return SharedPtr<SensorData>(new data::ApolloChassisMeasurement(std::move(data)));
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
