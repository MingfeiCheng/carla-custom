// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Buffer.h"
#include "carla/Memory.h"
#include "Carla/Vehicle/VehicleControl.h"
#include "carla/sensor/RawData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ApolloChassisSerializer {
  public:

    struct Data {
      float throttle;
      float steer;
      float brake;
      bool hand_brake;
      bool reverse;
      bool manual_gear_shif;
      int gear;

      MSGPACK_DEFINE_ARRAY(throttle, steer, brake, hand_brake, reverse, manual_gear_shif, gear)
    };

    template <typename SensorT>
    static Buffer Serialize(
      const SensorT &sensor,
      const FVehicleControl &control);
    
    static Data DeserializeRawData(const RawData &message) {
      return MsgPack::UnPack<Data>(message.begin(), message.size());
    }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

  template <typename SensorT>
  inline Buffer ApolloChassisSerializer::Serialize(
      const SensorT &,
      const FVehicleControl &control) {
    return MsgPack::Pack(Data{control.Throttle, control.Steer, control.Brake, control.bHandBrake, control.bReverse, control.bManualGearShift, control.Gear});
  }


} // namespace s11n
} // namespace sensor
} // namespace carla
