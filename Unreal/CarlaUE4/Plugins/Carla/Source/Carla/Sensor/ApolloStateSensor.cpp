// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/ApolloStateSensor.h"

#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/rpc/Actor.h"
#include <compiler/enable-ue4-macros.h>

AApolloStateSensor::AApolloStateSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AApolloStateSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("apollo"), TEXT("state"));
}

void AApolloStateSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AApolloStateSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloStateSensor::PostPhysTick);
  
  rpc::Actor actorState = GetEpisode().SerializeActor(GetOwner());
  
  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloStateSensor Stream Send");
    auto Stream = GetDataStream(*this);
    Stream.Send(*this, actorState);
  }
}

void AApolloTransformSensor::BeginPlay()
{
  Super::BeginPlay();
}

