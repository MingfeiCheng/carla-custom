// Copyright (c) 2019 Intel Labs.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Debug.h"
#include "carla/client/detail/ActorVariant.h"
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
      : Super(data), _actor_state(Serializer::DeserializeRawData(data)) {}

  public:

    SharedPtr<client::Actor> GetActorState() const {
      return _actor_state.Get(GetEpisode());
    }

    // rpc::ActorId GetActorId() const {
    //   return _actor_id;
    // }

  private:
    client::detail::ActorVariant _actor_state;
  };

} // namespace data
} // namespace sensor
} // namespace carla
