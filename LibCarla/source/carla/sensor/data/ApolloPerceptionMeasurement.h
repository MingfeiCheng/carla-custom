// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/client/Actor.h"
#include "carla/rpc/Actor.h"
#include "carla/sensor/data/Array.h"
#include "carla/sensor/SensorData.h"
#include "carla/rpc/PerceptionUnit.h"

#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloPerceptionMeasurement : public Array<rpc::Actor> {
  public:

    explicit ApolloPerceptionMeasurement(RawData &&data) 
    : Array<rpc::Actor>(0u, std::move(data)) {}
  };

  // class ApolloPerceptionMeasurement : public Array<rpc::PerceptionUnit> {
  // public:

  //   explicit ApolloPerceptionMeasurement(RawData &&data) 
  //   : Array<rpc::PerceptionUnit>(0u, std::move(data)) {}
  // };

  // class ApolloPerceptionMeasurement : public Array<client::Actor> {
    
  //   using Super = Array<client::Actor>;
  // protected:
  //   using Serializer = s11n::ApolloPerceptionSerializer;

  //   friend Serializer;

  //   explicit ApolloPerceptionMeasurement(RawData &&data) 
  //   : Super(0u, std::move(data)) {
  //   }

  // public:

  //   Array<client::Actor> GetActorState() const {
  //     Array<client::Actor> array;
  //     for (const auto &event : *this){
  //       array.push_back(event.Get(GetEpisode()));
  //     }
  //     return array;
  //   }
  // };

} // namespace data
} // namespace sensor
} // namespace carla
