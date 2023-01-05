// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
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
      float qw = Serializer::DeserializeRawData(data).qw;
      float qx = Serializer::DeserializeRawData(data).qx;
      float qy = Serializer::DeserializeRawData(data).qy;
      float qz = Serializer::DeserializeRawData(data).qz;
      _location = location;
      _qw = qw;
      _qx = qx;
      _qy = qy;
      _qz = qz;
    }

  public:

    geom::Location GetLocation() const {
      return _location;
    }

    float GetQw() const {
      return _qw;
    }

    float GetQx() const {
      return _qx;
    }

    float GetQy() const {
      return _qy;
    }

    float GetQz() const {
      return _qz;
    }

  private:

    geom::Location _location;
    float _qw;
    float _qx;
    float _qy;
    float _qz;

  };

} // namespace data
} // namespace sensor
} // namespace carla
