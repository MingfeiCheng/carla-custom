// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Debug.h"
#include "carla/geom/GeoLocation.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/geom/Vector3D.h"

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
        geom::GeoLocation geo_location = Serializer::DeserializeRawData(data).geo_location;
        geom::Location location = Serializer::DeserializeRawData(data).location;
        geom::Rotation rotation = Serializer::DeserializeRawData(data).rotation;
        float qw = Serializer::DeserializeRawData(data).qw;
        float qx = Serializer::DeserializeRawData(data).qx;
        float qy = Serializer::DeserializeRawData(data).qy;
        float qz = Serializer::DeserializeRawData(data).qz;
        carla::geom::Vector3D acceleration =  Serializer::DeserializeRawData(data).acceleration;
        carla::geom::Vector3D angular_velocity = Serializer::DeserializeRawData(data).angular_velocity;
        carla::geom::Vector3D linear_velocity = Serializer::DeserializeRawData(data).linear_velocity;
        float speed = Serializer::DeserializeRawData(data).speed;

         _geo_location = geo_location;
        _location = location;
        _rotation = rotation;
        _qw = qw;
        _qx = qx;
        _qy = qy;
        _qz = qz;
        _acceleration = acceleration;
        _angular_velocity = angular_velocity;
        _linear_velocity = linear_velocity;
        _speed = speed;        
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

    geom::Vector3D GetAcceleration() const{
      return _acceleration;
    }

    geom::Vector3D GetAngularVelocity() const{
      return _angular_velocity;
    }

    geom::Vector3D GetLinearVelocity() const{
      return _linear_velocity;
    }

    float GetSpeed() const {
      return _speed;
    }

  private:
    geom::GeoLocation _geo_location;
    geom::Location _location;
    geom::Rotation _rotation;
    float _qw;
    float _qx;
    float _qy;
    float _qz;
    geom::Vector3D _acceleration;
    geom::Vector3D _angular_velocity;
    geom::Vector3D _linear_velocity;
    float _speed;
  };

} // namespace data
} // namespace sensor
} // namespace carla
