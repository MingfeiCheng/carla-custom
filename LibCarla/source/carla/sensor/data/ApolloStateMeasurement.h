// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/GeoLocation.h"
#include "carla/rpc/Actor.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloStateSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  /// A change of Apollo State Measurement.
  class ApolloStateMeasurement : public SensorData {

    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloStateSerializer;

    friend Serializer;

    explicit ApolloStateMeasurement(const RawData &&data)
      : Super(data){

      rpc::Actor actor = Serializer::DeserializeRawData(data).actor;
      geom::GeoLocation geo_location = Serializer::DeserializeRawData(data).geo_location;
      float qw = Serializer::DeserializeRawData(data).qw;
      float qx = Serializer::DeserializeRawData(data).qx;
      float qy = Serializer::DeserializeRawData(data).qy;
      float qz = Serializer::DeserializeRawData(data).qz;

      _actor = actor;
      _geo_location = geo_location;
      _qw = qw;
      _qx = qx;
      _qy = qy;
      _qz = qz;
    }

  public:

    rpc::Actor GetActor() const {
      return _actor;
    }

    geom::GeoLocation GetGeoLocation() const {
      return _geo_location;
    }

    double GetLongitude() const {
      return _geo_location.longitude;
    }

    double GetLatitude() const {
      return _geo_location.latitude;
    }

    double GetAltitude() const {
      return _geo_location.altitude;
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
    rpc::Actor _actor;
    geom::GeoLocation _geo_location;
    float _qw;
    float _qx;
    float _qy;
    float _qz;
  };

} // namespace data
} // namespace sensor
} // namespace carla