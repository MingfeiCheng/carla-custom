// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/rpc/Actor.h"
#include "carla/sensor/data/Array.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloPerceptionMeasurement : public Array<client::Actor> {
  public:

    explicit ApolloPerceptionMeasurement(RawData &&data) 
    : Array<rpc::Actor>(0u, std::move(data)) {}
  };

} // namespace data
} // namespace sensor
} // namespace carla
