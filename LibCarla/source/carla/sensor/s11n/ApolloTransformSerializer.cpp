// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/sensor/s11n/ApolloTransformSerializer.h"

//TODO: change data format
#include "carla/sensor/data/ApolloTransformMeasurement.h"

namespace carla {
namespace sensor {
namespace s11n {

  SharedPtr<SensorData> ApolloTransformSerializer::Deserialize(RawData &&data) {
    return SharedPtr<SensorData>(new data::ApolloTransformMeasurement(std::move(data)));
  }

} // namespace s11n
} // namespace sensor
} // namespace carla
