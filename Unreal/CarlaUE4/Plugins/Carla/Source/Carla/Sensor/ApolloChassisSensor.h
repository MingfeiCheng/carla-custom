// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/rpc/VehicleControl.h"
#include <compiler/enable-ue4-macros.h>

#include <array>

#include "ApolloChassisSensor.generated.h"

/// Chassis sensor representation
/// The actual position calculation is done one server side
UCLASS()
class CARLA_API AApolloChassisSensor : public ASensor
{
  GENERATED_BODY()

public:

  AApolloChassisSensor(const FObjectInitializer &ObjectInitializer);

  static FActorDefinition GetSensorDefinition();

  void SetOwner(AActor *Owner) override;

  // virtual void PrePhysTick(float DeltaSeconds) override;
  virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds) override;

protected:

  virtual void BeginPlay() override;

};
