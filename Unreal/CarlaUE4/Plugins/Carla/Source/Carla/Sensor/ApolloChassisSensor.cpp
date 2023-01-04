// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/ApolloChassisSensor.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaStatics.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Sensor/WorldObserver.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/rpc/ActorId.h"
#include "carla/rpc/VehicleControl.h"
#include <compiler/enable-ue4-macros.h>

AApolloChassisSensor::AApolloChassisSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AApolloChassisSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("apollo"), TEXT("chassis"));
}

void AApolloChassisSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AApolloChassisSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloChassisSensor::PostPhysTick);

  // if (GetParent() == nullptr) {
  //   throw_exception(std::runtime_error(GetDisplayId() + ": not attached to actor"));
  //   return;
  // }

  // const auto carla_ego_vehicle = boost::dynamic_pointer_cast<carla::client::Vehicle>(GetParent());
  // if (carla_ego_vehicle == nullptr) {
  //   _logger->error("Apollo Chassis Sensor only support vehicles as ego.");
  // }

  // carla::rpc::VehicleControl CurrentVehicleControl = carla_ego_vehicle.GetControl()

  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloChassisSensor Stream Send");

    const auto &Episode = GetEpisode()
    ActorId actor_id = Episode.FindActor(GetOwner()).GetActorId();
    rpc::VehicleControl control = Episode.Lock()->GetActorSnapshot(actor_id).state.vehicle_data.control; //Send point?

    auto Stream = GetDataStream(*this);
    Stream.Send(
      *this, 
      control);
  }
}

void AApolloChassisSensor::BeginPlay()
{
  Super::BeginPlay();
}
