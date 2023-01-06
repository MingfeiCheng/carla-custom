// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/ApolloPerceptionSensor.h"

#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"

#include <compiler/disable-ue4-macros.h>
// #include "carla/geom/Location.h"
// #include "carla/geom/Rotation.h"
// #include "carla/geom/Vector3D.h"
// #include "carla/geom/BoundingBox.h"
// #include "carla/rpc/ActorId.h"
// #include "carla/rpc/ActorDescription.h"
// #include "carla/rpc/PerceptionUnit.h"
// #include "carla/rpc/Actor.h"
#include <compiler/enable-ue4-macros.h>

AApolloPerceptionSensor::AApolloPerceptionSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxOverlap"));
  Box->SetupAttachment(RootComponent);
  Box->SetHiddenInGame(true); // Disable for debugging.
  Box->SetCollisionProfileName(FName("OverlapAll"));

  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AApolloPerceptionSensor::GetSensorDefinition()
{
  auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
      TEXT("apollo"),
      TEXT("perception"));

  FActorVariation Front;
  Front.Id = TEXT("distance_front");
  Front.Type = EActorAttributeType::Float;
  Front.RecommendedValues = { TEXT("50.0") };
  Front.bRestrictToRecommended = false;

  FActorVariation Back;
  Back.Id = TEXT("distance_back");
  Back.Type = EActorAttributeType::Float;
  Back.RecommendedValues = { TEXT("50.0") };
  Back.bRestrictToRecommended = false;

  FActorVariation Lateral;
  Lateral.Id = TEXT("distance_lateral");
  Lateral.Type = EActorAttributeType::Float;
  Lateral.RecommendedValues = { TEXT("50.0") };
  Lateral.bRestrictToRecommended = false;

  Definition.Variations.Append({ Front, Back, Lateral });

  return Definition;
}

void AApolloPerceptionSensor::Set(const FActorDescription &Description)
{
  Super::Set(Description);

  float Front = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "distance_front",
      Description.Variations,
      50.0f);
  float Back = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "distance_back",
      Description.Variations,
      50.0f);
  float Lateral = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "distance_lateral",
      Description.Variations,
      50.0f);

  constexpr float M_TO_CM = 100.0f; // Unit conversion.

  float LocationX = M_TO_CM * (Front - Back) / 2.0f;
  float ExtentX = M_TO_CM * (Front + Back) / 2.0f;
  float ExtentY = M_TO_CM * Lateral;

  Box->SetRelativeLocation(FVector{LocationX, 0.0f, 0.0f});
  Box->SetBoxExtent(FVector{ExtentX, ExtentY, 0.0f});
}

void AApolloPerceptionSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);

  auto BoundingBox = UBoundingBoxCalculator::GetActorBoundingBox(Owner);

  Box->SetBoxExtent(BoundingBox.Extent + Box->GetUnscaledBoxExtent());
}

void AApolloPerceptionSensor::PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds)
{
  // Super::PostPhysTick(DeltaSeconds);

  TSet<AActor *> DetectedActors;
  Box->GetOverlappingActors(DetectedActors, ACarlaWheeledVehicle::StaticClass());
  DetectedActors.Remove(GetOwner());

  if (DetectedActors.Num() > 0){
    // ALargeMapManager * LargeMap = UCarlaStatics::GetLargeMapManager(GetWorld());

    // TSet<carla::rpc::PerceptionUnit> EnvObjectIdsSet;
    // for(AActor *actor : DetectedActors)
    // {
    //   //Location & Rotation
    //   // const FVector actor_loction = actor.GetActorLocation();
    //   // const FRotator actor_rotation = actor.GetActorRotation();

    //   // if (LargeMap)
    //   // {
    //   //   actor_loction = LargeMap->LocalToGlobalLocation(actor_loction);
    //   // }

    //   // const carla::geom::Location apollo_actor_location = carla::geom::Location(actor_loction.X, -actor_loction.Y, actor_loction.Z);
    //   // const carla::geom::Rotation apollo_actor_rotation = carla::geom::Rotation(-actor_rotation.Pitch, -(actor_rotation.Yaw + 90), actor_rotation.Roll);
      
    //   // const auto RootComponent = Cast<UPrimitiveComponent>(actor.GetRootComponent());
    //   // //Velocity
    //   // FVector actor_angular_velocity;
    //   // FVector actor_linear_velocity;
    //   // if (RootComponent != nullptr) {
    //   //   //Need converter
    //   //     const FQuat actor_global_rotation = RootComponent->GetComponentTransform().GetRotation();
    //   //     const FVector actor_global_angular_velocity = RootComponent->GetPhysicsAngularVelocityInRadians();
    //   //     actor_angular_velocity = ActorGlobalRotation.UnrotateVector(actor_global_angular_velocity);
    //   //     actor_linear_velocity = RootComponent->GetPhysicsLinearVelocity();
    //   // } else {
    //   //     actor_angular_velocity = FVector::ZeroVector;
    //   //     actor_linear_velocity = FVector::ZeroVector;
    //   // }

    //   // const FVector Acceleration = (LinearVelocity - PreviousLinearVelocity) / DeltaSeconds;
    //   // PreviousLinearVelocity = LinearVelocity;
    //   // const carla::geom::Vector3D apollo_actor_acceleration =  carla::geom::Vector3D{Acceleration.X, -Acceleration.Y, Acceleration.Z};
    //   // const carla::geom::Vector3D apollo_angular_velocity = carla::geom::Vector3D{actor_angular_velocity.X, -actor_angular_velocity.Y, -actor_angular_velocity.Z};
    //   // const carla::geom::Vector3D apollo_linear_velocity = carla::geom::Vector3D{actor_linear_velocity.X, -actor_linear_velocity.Y, actor_linear_velocity.Z};

    //   // FCarlaActor carla_actor = GetEpisode().FindCarlaActor(actor);
    //   const auto episode = GetEpisode();
    //   const carla::rpc::Actor actor_info = episode.SerializeActor(actor);
    //   // bbox
    //   carla::rpc::ActorId actor_id = actor_info.id;
    //   carla::geom::BoundingBox actor_bbox = actor_info.bounding_box;
    //   std::string actor_type = actor_info.description.id; //TODO:Check

    //   const carla::client::detail::ActorVariant actor_variant = carla::client::detail::ActorVariant(actor_info);
    //   const carla::SharedPtr<carla::client::Actor> actor_ptr = actor_variant.Get(episode);
    //   // const auto actor_variant = carla::client::detail::ActorVariant(actor_info);
    //   // const auto actor_ptr = actor_variant.Get(episode);

    //   // location, rotation, velocity, acceleration
    //   const carla::geom::Location actor_location = actor_ptr.GetLocation();
    //   const carla::geom::Rotation actor_rotation = actor_ptr.GetRotation();
    //   const carla::geom::Vector3D actor_velocity = actor_ptr.GetVelocity();
    //   const carla::geom::Vector3D actor_acceleration = actor_ptr.GetAcceleration();

    //   const carla::geom::Location apollo_actor_location = carla::geom::Location(actor_location.x, -actor_location.y, actor_location.z);
    //   const carla::geom::Vector3D apollo_actor_velocity = carla::geom::Vector3D(actor_velocity.x, -actor_velocity.y, actor_velocity.z);
    //   const carla::geom::Vector3D apollo_actor_acceleration = carla::geom::Vector3D(actor_acceleration.x, -actor_acceleration.y, actor_acceleration.z);

    //   const carla::rpc::PerceptionUnit actor_perception_unit = carla::rpc::PerceptionUnit(actor_id, actor_bbox, actor_type, actor_location, actor_rotation, actor_velocity, actor_acceleration);

    //   EnvObjectIdsSet.Emplace(actor_perception_unit);
    // }
  
    {
      auto Stream = GetDataStream(*this);
      Stream.Send(*this, GetEpisode(), DetectedActors);
    }
  }
}

  void AApolloStateSensor::BeginPlay()
  {
    Super::BeginPlay();
  }

