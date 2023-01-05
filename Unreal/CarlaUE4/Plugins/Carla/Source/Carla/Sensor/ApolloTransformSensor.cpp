// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"

#include "Carla/Sensor/ApolloTransformSensor.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaStatics.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/geom/Math.h"
#include <compiler/enable-ue4-macros.h>

AApolloTransformSensor::AApolloTransformSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AApolloTransformSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("apollo"), TEXT("transform"));
}

void AApolloTransformSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AApolloTransformSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloTransformSensor::PostPhysTick);

  FVector ActorLocation = GetActorLocation();
  FRotator ActorRotation = GetActorRotation();

  carla::geom::Location Location = carla::geom::Location(ActorLocation.x, -ActorLocation.y, ActorLocation.z);
  const FQuat RotationQuat = FRotator(-ActorRotation.Pitch, -ActorRotation.Yaw, ActorRotation.Roll).Quaternion();
  const float qw = RotationQuat.W;
  const float qx = RotationQuat.X;
  const float qy = RotationQuat.Y;
  const float qz = RotationQuat.Z;

  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloTransformSensor Stream Send");
    auto Stream = GetDataStream(*this);
    Stream.Send(
      *this, 
      Location,
      qw,
      qx,
      qy,
      qz);
  }
}

void AApolloTransformSensor::BeginPlay()
{
  Super::BeginPlay();
}
