// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/Location.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloTransformSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  /// A change of Apollo Transform Measurement.
  class ApolloTransformMeasurement : public SensorData {

    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloTransformSerializer;

    friend Serializer;

    explicit ApolloTransformMeasurement(const RawData &&data)
      : Super(data){

      geom::Location location = Serializer::DeserializeRawData(data).location;
      FQuat rotation = Serializer::DeserializeRawData(data).rotation;
      _location = location;
      _rotation = rotation;
    }

  public:

    geom::Location GetLocation() const {
      return _location;
    }

    FQuat GetRotation() const {
      return _rotation;
    }

  private:

    geom::Location _location;
    FQuat _rotation;

  };

} // namespace data
} // namespace sensor
} // namespace carla
