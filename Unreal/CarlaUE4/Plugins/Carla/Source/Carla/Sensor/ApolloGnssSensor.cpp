// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "Carla.h"
#include "Carla/Sensor/ApolloGnssSensor.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaStatics.h"
#include "Carla/MapGen/LargeMapManager.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Sensor/WorldObserver.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/geom/Vector3D.h"
#include "carla/geom/Math.h"
#include <compiler/enable-ue4-macros.h>

AApolloGnssSensor::AApolloGnssSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
  RandomEngine = CreateDefaultSubobject<URandomEngine>(TEXT("RandomEngine"));
}

FActorDefinition AApolloGnssSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeApolloGnssDefinition();
}

void AApolloGnssSensor::Set(const FActorDescription &ActorDescription)
{
  Super::Set(ActorDescription);
  UActorBlueprintFunctionLibrary::SetApolloGnss(ActorDescription, this);
}

void AApolloGnssSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AApolloGnssSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloGnssSensor::PostPhysTick);

  FVector ActorLocation = GetActorLocation();
  FRotator ActorRotation = GetActorRotation();

  ALargeMapManager * LargeMap = UCarlaStatics::GetLargeMapManager(GetWorld());
  if (LargeMap)
  {
    ActorLocation = LargeMap->LocalToGlobalLocation(ActorLocation);
  }
  
  carla::geom::Location Location = ActorLocation; //carla::geom::Location(ActorLocation.X, -ActorLocation.Y, ActorLocation.Z);
  carla::geom::GeoLocation CurrentLocation = CurrentGeoReference.Transform(Location);

  // Compute the noise for the sensor
  const float LatError = RandomEngine->GetNormalDistribution(0.0f, LatitudeDeviation);
  const float LonError = RandomEngine->GetNormalDistribution(0.0f, LongitudeDeviation);
  const float AltError = RandomEngine->GetNormalDistribution(0.0f, AltitudeDeviation);

  // Apply the noise to the sensor
  double Latitude = CurrentLocation.latitude + LatitudeBias + LatError;
  double Longitude = CurrentLocation.longitude + LongitudeBias + LonError;
  double Altitude = CurrentLocation.altitude + AltitudeBias + AltError;

  carla::geom::Location ApolloLocation = carla::geom::Location(ActorLocation.X, -ActorLocation.Y, ActorLocation.Z);
  carla::geom::Rotation ApolloRotation = carla::geom::Rotation(-ActorRotation.Pitch, -(ActorRotation.Yaw + 90), ActorRotation.Roll);

  const FQuat ApolloRotationQuat = FRotator(ApolloRotation.Pitch, ApolloRotation.Yaw, ApolloRotation.Roll).Quaternion();
  const float qw = ApolloRotationQuat.W;
  const float qx = ApolloRotationQuat.X;
  const float qy = ApolloRotationQuat.Y;
  const float qz = ApolloRotationQuat.Z;

  c

  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloGnssSensor Stream Send");
    auto Stream = GetDataStream(*this);
    Stream.Send(
      *this, 
      carla::geom::GeoLocation{Latitude, Longitude, Altitude},
      ApolloLocation,
      ApolloRotation,
      qw,
      qx,
      qy,
      qz);
  }
}

void AApolloGnssSensor::SetLatitudeDeviation(float Value)
{
  LatitudeDeviation = Value;
}

void AApolloGnssSensor::SetLongitudeDeviation(float Value)
{
  LongitudeDeviation = Value;
}

void AApolloGnssSensor::SetAltitudeDeviation(float Value)
{
  AltitudeDeviation = Value;
}

void AApolloGnssSensor::SetLatitudeBias(float Value)
{
  LatitudeBias = Value;
}

void AApolloGnssSensor::SetLongitudeBias(float Value)
{
  LongitudeBias = Value;
}

void AApolloGnssSensor::SetAltitudeBias(float Value)
{
  AltitudeBias = Value;
}

float AApolloGnssSensor::GetLatitudeDeviation() const
{
  return LatitudeDeviation;
}
float AApolloGnssSensor::GetLongitudeDeviation() const
{
  return LongitudeDeviation;
}
float AApolloGnssSensor::GetAltitudeDeviation() const
{
  return AltitudeDeviation;
}

float AApolloGnssSensor::GetLatitudeBias() const
{
  return LatitudeBias;
}
float AApolloGnssSensor::GetLongitudeBias() const
{
  return LongitudeBias;
}
float AApolloGnssSensor::GetAltitudeBias() const
{
  return AltitudeBias;
}

void AApolloGnssSensor::BeginPlay()
{
  Super::BeginPlay();

  const UCarlaEpisode* episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
  CurrentGeoReference = episode->GetGeoReference();
}
