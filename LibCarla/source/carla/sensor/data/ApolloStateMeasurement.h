// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/geom/GeoLocation.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/BoundingBox.h"
#include "carla/rpc/ActorId.h"
#include "carla/rpc/VehicleControl.h"
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
      id = client::detail::ActorVariant(Serializer::DeserializeRawData(data).id);
      type = Serializer::DeserializeRawData(data).type;
      bbox = Serializer::DeserializeRawData(data).bbox;
      location = Serializer::DeserializeRawData(data).location;
      rotation = Serializer::DeserializeRawData(data).rotation;
      velocity = Serializer::DeserializeRawData(data).velocity;
      speed = Serializer::DeserializeRawData(data).speed;
      acceleration = Serializer::DeserializeRawData(data).acceleration;
      angular_velocity = Serializer::DeserializeRawData(data).angular_velocity;
      geo_location = Serializer::DeserializeRawData(data).geo_location;
      qw = Serializer::DeserializeRawData(data).qw;
      qx = Serializer::DeserializeRawData(data).qx;
      qy = Serializer::DeserializeRawData(data).qy;
      qz = Serializer::DeserializeRawData(data).qz;
      control = Serializer::DeserializeRawData(data).control;
    }

  public:

    rpc::ActorId GetId() const{
      return id;
    }

    std::string GetType() const{
      return type;
    }

    geom::BoundingBox GetBBox() const{
      return bbox;
    }

    geom::Location GetLocation() const{
      return location;
    }

    geom::Rotation GetRotation() const{
      return rotation;
    }

    geom::Vector3D GetVelocity() const{
      return velocity;
    }

    float GetSpeed() const{
      return speed;
    }

    geom::Vector3D GetAcceleration() const{
      return acceleration;
    }

    geom::Vector3D GetAngularVelocity() const{
      return angular_velocity;
    }

    geom::GeoLocation GetGeoLocation() const {
      return geo_location;
    }

    double GetLongitude() const {
      return geo_location.longitude;
    }

    double GetLatitude() const {
      return geo_location.latitude;
    }

    double GetAltitude() const {
      return geo_location.altitude;
    }

    float GetQw() const {
      return qw;
    }

    float GetQx() const {
      return qx;
    }

    float GetQy() const {
      return qy;
    }

    float GetQz() const {
      return qz;
    }

    rpc::VehicleControl GetControl() const{
      return control;
    }

    float GetThrottle() const{
      return control.throttle;
    }

    float GetSteer() const{
      return control.steer;
    }

    float GetBrake() const{
      return control.brake;
    }

    bool GetHandBrake() const{
      return control.hand_brake;
    }

    bool GetReverse() const{
      return control.reverse;
    }

    bool GetManualGearShift() const{
      return control.manual_gear_shift;
    }

    int GetGear() const{
      return control.gear;
    }

  private:
    rpc::ActorId id;
    std::string type;
    geom::BoundingBox bbox;
    geom::Location location;
    geom::Rotation rotation;
    geom::Vector3D velocity;
    float speed;
    geom::Vector3D acceleration;
    geom::Vector3D angular_velocity;
    geom::GeoLocation geo_location;
    float qw;
    float qx;
    float qy;
    float qz;
    rpc::VehicleControl control;
  };

} // namespace data
} // namespace sensor
} // namespace carla