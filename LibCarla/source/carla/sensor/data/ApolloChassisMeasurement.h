// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/rpc/VehicleControl.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloChassisSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  /// A change of Apollo Chassis Measurement.
  class ApolloChassisMeasurement : public SensorData {

    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloChassisSerializer;

    friend Serializer;

    explicit ApolloChassisMeasurement(const RawData &&data)
      : Super(data){

      rpc::VehicleControl contorl_data = Serializer::DeserializeRawData(data);
      _control = contorl_data;
    }

  public:

    rpc::VehicleControl GetControl() const {
      return _control;
    }

  private:

    rpc::VehicleControl _control;

  };

} // namespace data
} // namespace sensor
} // namespace carla
