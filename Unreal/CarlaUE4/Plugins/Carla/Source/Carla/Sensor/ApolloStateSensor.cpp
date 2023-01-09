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
#include <carla/geom/Vector3D.h>
#include <carla/geom/Math.h>
#include <carla/geom/GeoLocation.h>
#include <carla/geom/Location.h>
#include <carla/geom/Rotation.h>
#include <carla/rpc/Actor.h>
#include <compiler/enable-ue4-macros.h>

AApolloStateSensor::AApolloStateSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
  RandomEngine = CreateDefaultSubobject<URandomEngine>(TEXT("RandomEngine"));
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
  
  constexpr float TO_METERS = 1e-2;
  const UCarlaEpisode* Episode = UCarlaStatics::GetCurrentEpisode(GetWorld());

  const FCarlaActor* ActorView = Episode->FindCarlaActor(GetOwner());
  const FActorInfo* ActorInfo = ActorView->GetActorInfo();

  const carla::rpc::ActorId ApolloActorId = ActorView->GetActorId();
  std::string ApolloActorType = "apollo";

  FBoundingBox ActorBBox = UBoundingBoxCalculator::GetActorBoundingBox(It);
  const carla::geom::Location ActorBBoxLocation = carla::geom::Location(ActorBBox.Origin.X * TO_METERS, ActorBBox.Origin.Y * TO_METERS, ActorBBox.Origin.Z * TO_METERS);
  const carla::geom::Vector3D ActorBBoxExtent = carla::geom::Vector3D(ActorBBox.Extent.X * TO_METERS, ActorBBox.Extent.Y * TO_METERS, ActorBBox.Extent.Z * TO_METERS);
  const carla::geom::Rotation ActorBBoxRotation = carla::geom::Rotation(ActorBBox.Rotation.Pitch, ActorBBox.Rotation.Yaw, ActorBBox.Rotation.Roll);
  const carla::geom::BoundingBox ApolloActorBBox = carla::geom::BoundingBox(ActorBBoxLocation, ActorBBoxExtent, ActorBBoxRotation);

  ActorTransform = ActorView->GetActorGlobalTransform();
  const FVector ActorLocation = ActorTransform.GetLocation();
  const FRotator ActorRotation = ActorTransform.GetRotation().Rotator();  
  // FVector ActorLocation = GetOwner()->GetActorLocation();
  // const FRotator ActorRotation = GetOwner()->GetActorRotation();
  // //add gps & quant
  // ALargeMapManager * LargeMap = UCarlaStatics::GetLargeMapManager(GetWorld());
  // if (LargeMap)
  // {
  //   ActorLocation = LargeMap->LocalToGlobalLocation(ActorLocation);
  // }
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

  const carla::geom::GeoLocation ApolloActorGeoLocation = carla::geom::GeoLocation{Latitude, Longitude, Altitude};

  // Location, Rotation, Latitude, Longitude, Altitude, Quant
  const carla::geom::Location ApolloActorLocation = carla::geom::Location(ActorLocation.X * TO_METERS, -ActorLocation.Y * TO_METERS, ActorLocation.Z * TO_METERS);
  const carla::geom::Rotation ApolloActorRotation = carla::geom::Rotation(-ActorRotation.Pitch, -(ActorRotation.Yaw + 90), ActorRotation.Roll);
  
  const FQuat ApolloActorRotationQuat = FRotator(ApolloActorRotation.pitch, ApolloActorRotation.yaw, ApolloActorRotation.roll).Quaternion();
  const float ApolloActorQw = ApolloActorRotationQuat.W;
  const float ApolloActorQx = ApolloActorRotationQuat.X;
  const float ApolloActorQy = ApolloActorRotationQuat.Y;
  const float ApolloActorQz = ApolloActorRotationQuat.Z;

  // Velocity
  FTransform ActorTransform;
  FVector Velocity(0.0f);
  // FVector Acceleration(0.0f);
  carla::geom::Vector3D ActorAngularVelocity(0.0f, 0.0f, 0.0f);

  check(ActorView);

  if(ActorView->IsDormant())
  {
    const FActorData* ActorData = ActorView->GetActorData();
    Velocity = TO_METERS * ActorData->Velocity;
    ActorAngularVelocity = carla::geom::Vector3D
                                {ActorData->AngularVelocity.X,
                                -ActorData->AngularVelocity.Y,
                                -ActorData->AngularVelocity.Z};
  }
  else
  {
    Velocity = TO_METERS * ActorView->GetActor()->GetVelocity();
    const auto RootComponent = Cast<UPrimitiveComponent>(Actor.GetRootComponent());
    const FVector AngularVelocity =
        RootComponent != nullptr ?
            RootComponent->GetPhysicsAngularVelocityInDegrees() :
            FVector{0.0f, 0.0f, 0.0f};
    ActorAngularVelocity = carla::geom::Vector3D(AngularVelocity.X, -AngularVelocity.Y, -AngularVelocity.Z);
  }

  FVector &PreviousVelocity = ActorView->GetActorInfo()->Velocity;
  const FVector Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;
  PreviousVelocity = Velocity;
  ActorTransform = ActorView->GetActorGlobalTransform();
  const FVector ActorLocation = ActorTransform.GetLocation();
  const FRotator ActorRotation = ActorTransform.GetRotation().Rotator();    

  const carla::geom::Vector3D ApolloActorAcceleration = carla::geom::Vector3D(Acceleration.X, -Acceleration.Y, Acceleration.Z);
  const carla::geom::Vector3D ApolloActorVelocity = carla::geom::Vector3D(Velocity.X, -Velocity.Y, Velocity.Z);
  const carla::geom::Vector3D ApolloActorAngularVelocity = ActorAngularVelocity;
  const float ApolloActorSpeed = ApolloActorVelocity.Length();

  // carla::rpc::Actor actor_obj = GetEpisode().SerializeActor(GetOwner());
  auto Vehicle = Cast<ACarlaWheeledVehicle>(ActorView->GetActor());
  const carla::rpc::VehicleControl ApolloActorControl = carla::rpc::VehicleControl{Vehicle->GetVehicleControl()};
  
  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloStateSensor Stream Send");
    auto Stream = GetDataStream(*this);
    // Stream.Send(*this, actor_obj, ApolloGeoLocation, qw, qx, qy, qz);
    Stream.Send(*this, ApolloActorId,
                       ApolloActorType,
                       ApolloActorBBox, 
                       ApolloActorLocation,
                       ApolloActorRotation,
                       ApolloActorVelocity,
                       ApolloActorSpeed,
                       ApolloActorAcceleration,
                       ApolloActorAngularVelocity,
                       ApolloActorGeoLocation,
                       ApolloActorQw,
                       ApolloActorQx,
                       ApolloActorQy,
                       ApolloActorQz,
                       ApolloActorControl);
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

  const UCarlaEpisode* tmp_episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
  CurrentGeoReference = tmp_episode->GetGeoReference();
}

