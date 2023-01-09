// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/CustomStateSensor.h"
#include "Carla/Game/CarlaStatics.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/MapGen/LargeMapManager.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Sensor/WorldObserver.h"
#include "Carla/Actor/CarlaActor.h"
#include "Carla/Util/BoundingBox.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/geom/Vector3D.h>
#include <carla/geom/Math.h>
#include <carla/geom/GeoLocation.h>
#include <carla/geom/Location.h>
#include <carla/geom/Rotation.h>
#include <carla/rpc/Actor.h>
#include <compiler/enable-ue4-macros.h>

ACustomStateSensor::ACustomStateSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
  RandomEngine = CreateDefaultSubobject<URandomEngine>(TEXT("RandomEngine"));
}

FActorDefinition ACustomStateSensor::GetSensorDefinition()
{
  return UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(TEXT("custom"), TEXT("state"));
}

void ACustomStateSensor::Set(const FActorDescription &ActorDescription)
{
  Super::Set(ActorDescription);
  //Change
  UActorBlueprintFunctionLibrary::SetCustomState(ActorDescription, this);
}

void ACustomStateSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void ACustomStateSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  TRACE_CPUPROFILER_EVENT_SCOPE(ACustomStateSensor::PostPhysTick);
  
  // Velocity
  FTransform ActorTransform;
  FVector Velocity(0.0f);
  // FVector Acceleration(0.0f);
  carla::geom::Vector3D ActorAngularVelocity(0.0f, 0.0f, 0.0f);

  constexpr float TO_METERS = 1e-2;
  const UCarlaEpisode* Episode = UCarlaStatics::GetCurrentEpisode(GetWorld());

  const FCarlaActor* ActorView = Episode->FindCarlaActor(GetOwner());
  const FActorInfo* ActorInfo = ActorView->GetActorInfo();

  const carla::rpc::ActorId CustomActorId = ActorView->GetActorId();
  std::string CustomActorType = "ego";

  FBoundingBox ActorBBox = UBoundingBoxCalculator::GetActorBoundingBox(GetOwner());
  const carla::geom::Location ActorBBoxLocation = carla::geom::Location(ActorBBox.Origin.X * TO_METERS, ActorBBox.Origin.Y * TO_METERS, ActorBBox.Origin.Z * TO_METERS);
  const carla::geom::Vector3D ActorBBoxExtent = carla::geom::Vector3D(ActorBBox.Extent.X * TO_METERS, ActorBBox.Extent.Y * TO_METERS, ActorBBox.Extent.Z * TO_METERS);
  const carla::geom::Rotation ActorBBoxRotation = carla::geom::Rotation(ActorBBox.Rotation.Pitch, ActorBBox.Rotation.Yaw, ActorBBox.Rotation.Roll);
  const carla::geom::BoundingBox CustomActorBBox = carla::geom::BoundingBox(ActorBBoxLocation, ActorBBoxExtent, ActorBBoxRotation);

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

  const carla::geom::GeoLocation CustomActorGeoLocation = carla::geom::GeoLocation{Latitude, Longitude, Altitude};

  // Location, Rotation, Latitude, Longitude, Altitude, Quant
  const carla::geom::Location CustomActorLocation = carla::geom::Location(ActorLocation.X * TO_METERS, ActorLocation.Y * TO_METERS, ActorLocation.Z * TO_METERS);
  const carla::geom::Rotation CustomActorRotation = carla::geom::Rotation(ActorRotation.Pitch, ActorRotation.Yaw, ActorRotation.Roll);
  
  const FQuat CustomActorRotationQuat = FRotator(CustomActorRotation.pitch, CustomActorRotation.yaw, CustomActorRotation.roll).Quaternion();
  const float CustomActorQw = CustomActorRotationQuat.W;
  const float CustomActorQx = CustomActorRotationQuat.X;
  const float CustomActorQy = CustomActorRotationQuat.Y;
  const float CustomActorQz = CustomActorRotationQuat.Z;

  check(ActorView);

  if(ActorView->IsDormant())
  {
    const FActorData* ActorData = ActorView->GetActorData();
    Velocity = TO_METERS * ActorData->Velocity;
    ActorAngularVelocity = carla::geom::Vector3D
                                {ActorData->AngularVelocity.X,
                                 ActorData->AngularVelocity.Y,
                                 ActorData->AngularVelocity.Z};
  }
  else
  {
    Velocity = TO_METERS * ActorView->GetActor()->GetVelocity();
    const auto RootComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    const FVector AngularVelocity =
        RootComponent != nullptr ?
            RootComponent->GetPhysicsAngularVelocityInDegrees() :
            FVector{0.0f, 0.0f, 0.0f};
    ActorAngularVelocity = carla::geom::Vector3D(AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z);
  }

  FVector &PreviousVelocity = ActorView->GetActorInfo()->Velocity;
  const FVector Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;
  PreviousVelocity = Velocity;

  const carla::geom::Vector3D CustomActorAcceleration = carla::geom::Vector3D(Acceleration.X, Acceleration.Y, Acceleration.Z);
  const carla::geom::Vector3D CustomActorVelocity = carla::geom::Vector3D(Velocity.X, Velocity.Y, Velocity.Z);
  const carla::geom::Vector3D CustomActorAngularVelocity = ActorAngularVelocity;
  const float CustomActorSpeed = CustomActorVelocity.Length();

  // carla::rpc::Actor actor_obj = GetEpisode().SerializeActor(GetOwner());
  auto Vehicle = Cast<ACarlaWheeledVehicle>(ActorView->GetActor());
  const carla::rpc::VehicleControl CustomActorControl = carla::rpc::VehicleControl{Vehicle->GetVehicleControl()};
  
  {
    TRACE_CPUPROFILER_EVENT_SCOPE_STR("ACustomStateSensor Stream Send");
    auto Stream = GetDataStream(*this);
    // Stream.Send(*this, actor_obj, CustomGeoLocation, qw, qx, qy, qz);
    Stream.Send(*this, CustomActorId,
                       CustomActorType,
                       CustomActorBBox, 
                       CustomActorLocation,
                       CustomActorRotation,
                       CustomActorVelocity,
                       CustomActorSpeed,
                       CustomActorAcceleration,
                       CustomActorAngularVelocity,
                       CustomActorGeoLocation,
                       CustomActorQw,
                       CustomActorQx,
                       CustomActorQy,
                       CustomActorQz,
                       CustomActorControl);
  }
}

void ACustomStateSensor::SetLatitudeDeviation(float Value)
{
  LatitudeDeviation = Value;
}

void ACustomStateSensor::SetLongitudeDeviation(float Value)
{
  LongitudeDeviation = Value;
}

void ACustomStateSensor::SetAltitudeDeviation(float Value)
{
  AltitudeDeviation = Value;
}

void ACustomStateSensor::SetLatitudeBias(float Value)
{
  LatitudeBias = Value;
}

void ACustomStateSensor::SetLongitudeBias(float Value)
{
  LongitudeBias = Value;
}

void ACustomStateSensor::SetAltitudeBias(float Value)
{
  AltitudeBias = Value;
}

float ACustomStateSensor::GetLatitudeDeviation() const
{
  return LatitudeDeviation;
}
float ACustomStateSensor::GetLongitudeDeviation() const
{
  return LongitudeDeviation;
}
float ACustomStateSensor::GetAltitudeDeviation() const
{
  return AltitudeDeviation;
}

float ACustomStateSensor::GetLatitudeBias() const
{
  return LatitudeBias;
}
float ACustomStateSensor::GetLongitudeBias() const
{
  return LongitudeBias;
}
float ACustomStateSensor::GetAltitudeBias() const
{
  return AltitudeBias;
}

void ACustomStateSensor::BeginPlay()
{
  Super::BeginPlay();

  const UCarlaEpisode* tmp_episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
  CurrentGeoReference = tmp_episode->GetGeoReference();
}

