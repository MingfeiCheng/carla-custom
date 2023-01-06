// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/ApolloStateSensor.h"
#include "Carla/Game/CarlaStatics.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/MapGen/LargeMapManager.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Sensor/WorldObserver.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/geom/Vector3D.h"
#include "carla/geom/Math.h"
#include "carla/geom/GeoLocation.h"
#include "carla/geom/Location.h"
#include "carla/geom/Rotation.h"
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

void AApolloStateSensor::Set(const FActorDescription &ActorDescription)
{
  Super::Set(ActorDescription);
  //Change
  UActorBlueprintFunctionLibrary::SetApolloState(ActorDescription, this);
}

void AApolloStateSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AApolloStateSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloStateSensor::PostPhysTick);
  
  FVector ActorLocation = GetOwner()->GetActorLocation();
  const FRotator ActorRotation = GetOwner()->GetActorRotation();

  //add gps & quant
  ALargeMapManager * LargeMap = UCarlaStatics::GetLargeMapManager(GetWorld());
  if (LargeMap)
  {
    ActorLocation = LargeMap->LocalToGlobalLocation(ActorLocation);
  }
  
  const carla::geom::Location Location = ActorLocation;
  const carla::geom::GeoLocation CurrentLocation = CurrentGeoReference.Transform(Location);

  // Compute the noise for the sensor
  const float LatError = RandomEngine->GetNormalDistribution(0.0f, LatitudeDeviation);
  const float LonError = RandomEngine->GetNormalDistribution(0.0f, LongitudeDeviation);
  const float AltError = RandomEngine->GetNormalDistribution(0.0f, AltitudeDeviation);

  // Apply the noise to the sensor
  double Latitude = CurrentLocation.latitude + LatitudeBias + LatError;
  double Longitude = CurrentLocation.longitude + LongitudeBias + LonError;
  double Altitude = CurrentLocation.altitude + AltitudeBias + AltError;

  // Location, Rotation, Latitude, Longitude, Altitude, Quant
  const carla::geom::Location ApolloLocation = carla::geom::Location(ActorLocation.X, -ActorLocation.Y, ActorLocation.Z);
  const carla::geom::Rotation ApolloRotation = carla::geom::Rotation(-ActorRotation.Pitch, -(ActorRotation.Yaw + 90), ActorRotation.Roll);
  const carla::geom::GeoLocation ApolloGeoLocation = carla::geom::GeoLocation{Latitude, Longitude, Altitude};
  const FQuat ApolloRotationQuat = FRotator(ApolloRotation.pitch, ApolloRotation.yaw, ApolloRotation.roll).Quaternion();
  const float qw = ApolloRotationQuat.W;
  const float qx = ApolloRotationQuat.X;
  const float qy = ApolloRotationQuat.Y;
  const float qz = ApolloRotationQuat.Z;

  carla::rpc::Actor actor_obj = GetEpisode().SerializeActor(GetOwner());
  
  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloStateSensor Stream Send");
    auto Stream = GetDataStream(*this);
    Stream.Send(*this, actor_obj, ApolloGeoLocation, qw, qx, qy, qz);
  }
}

void AApolloStateSensor::SetLatitudeDeviation(float Value)
{
  LatitudeDeviation = Value;
}

void AApolloStateSensor::SetLongitudeDeviation(float Value)
{
  LongitudeDeviation = Value;
}

void AApolloStateSensor::SetAltitudeDeviation(float Value)
{
  AltitudeDeviation = Value;
}

void AApolloStateSensor::SetLatitudeBias(float Value)
{
  LatitudeBias = Value;
}

void AApolloStateSensor::SetLongitudeBias(float Value)
{
  LongitudeBias = Value;
}

void AApolloStateSensor::SetAltitudeBias(float Value)
{
  AltitudeBias = Value;
}

float AApolloStateSensor::GetLatitudeDeviation() const
{
  return LatitudeDeviation;
}
float AApolloStateSensor::GetLongitudeDeviation() const
{
  return LongitudeDeviation;
}
float AApolloStateSensor::GetAltitudeDeviation() const
{
  return AltitudeDeviation;
}

float AApolloStateSensor::GetLatitudeBias() const
{
  return LatitudeBias;
}
float AApolloStateSensor::GetLongitudeBias() const
{
  return LongitudeBias;
}
float AApolloStateSensor::GetAltitudeBias() const
{
  return AltitudeBias;
}

void AApolloStateSensor::BeginPlay()
{
  Super::BeginPlay();

  const UCarlaEpisode* episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
  CurrentGeoReference = episode->GetGeoReference();
}

