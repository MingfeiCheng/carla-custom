// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/GeoLocation.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloGnssSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  /// A change of Apollo GNSS Measurement.
  class ApolloGnssMeasurement : public SensorData {

    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloGnssSerializer;

    friend Serializer;

    explicit ApolloGnssMeasurement(const RawData &&data)
      : Super(data){

      geom::GeoLocation geo_location = Serializer::DeserializeRawData(data).geo_location;
      geom::Location location = Serializer::DeserializeRawData(data).location;
      geom::Rotation rotation = Serializer::DeserializeRawData(data).rotation;
      _geo_location = geo_location;
      _location = location;
      _rotation = rotation;
    }

  public:

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

    geom::Location GetLocation() const {
      return _location;
    }

    geom::Rotation GetRotation() const {
      return _rotation;
    }

  private:

    geom::GeoLocation _geo_location;
    geom::Location _location;
    geom::Rotation _rotation;

  };

} // namespace data
} // namespace sensor
} // namespace carla
