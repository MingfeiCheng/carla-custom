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
#include "Carla/Game/CarlaStatics.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Actor/CarlaActor.h"
#include "Carla/Sensor/WorldObserver.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/geom/Location.h>
#include <carla/geom/Rotation.h>
#include <carla/geom/Vector3D.h>
#include <carla/geom/BoundingBox.h>
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
  TRACE_CPUPROFILER_EVENT_SCOPE(AApolloPerceptionSensor::PostPhysTick);

  TSet<AActor *> DetectedActors;
  Box->GetOverlappingActors(DetectedActors, ACarlaWheeledVehicle::StaticClass());
  DetectedActors.Remove(GetOwner());

  if (DetectedActors.Num() > 0){
    {
      TRACE_CPUPROFILER_EVENT_SCOPE_STR("AApolloPerceptionSensor Stream Send");
      const auto &episode = GetEpisode();

      FActorRegistry registry = episode.GetActorRegistry();
      ObstacleArray apollo_obstacles;

      for(AActor *actor : DetectedActors){
        FCarlaActor *carla_actor = episode.FindCarlaActor(actor);        
        const carla::rpc::ActorId apollo_id = carla_actor->GetActorId();
         // add logic 
        FCarlaActor::ActorType type = carla_actor->GetActorType();
        std::string apollo_type = "unknown";
        if (AType::Vehicle == type){
          apollo_type = "dynamic.vehicle";
        }else if (AType::TrafficSign == type){
          apollo_type = "static.traffic_sign";
        }else if (AType::TrafficLight == type){
          apollo_type = "static.traffic_light";
        }else if (AType::Walker == type){
          apollo_type = "dynamic.walker";
        }
        const carla::geom::BoundingBox apollo_bbox = UBoundingBoxCalculator::GetActorBoundingBox(actor);
        // carla
        FVector velocity = carla_actor->GetActorVelocity();
        FVector angular_velocity = carla_actor->GetActorAngularVelocity();
        const carla::geom::Vector3D acceleration = FWorldObserver_GetAcceleration(carla_actor, velocity, DeltaSeconds);
        const FTransform transform = carla_actor->GetActorGlobalTransform();
        const FVector location = transform.GetLocation();
        const FRotator rotation = transform.GetRotation().Rotator();       

        // apollo
        const carla::geom::Location apollo_location = carla::geom::Location(location.X, -location.Y, location.Z);
        const carla::geom::Rotation apollo_rotation = carla::geom::Rotation(rotation.Pitch, rotation.Yaw, rotation.Roll);
        const carla::geom::Vector3D apollo_velocity = carla::geom::Vector3D(velocity.X, -velocity.Y, velocity.Z);
        const carla::geom::Vector3D apollo_acceleration = carla::geom::Vector3D(acceleration.x, -acceleration.y, acceleration.z);
        const carla::geom::Vector3D apollo_angular_velocity = carla::geom::Vector3D(angular_velocity.X, angular_velocity.Y, angular_velocity.Z);

        apollo_obstacles.push_back(carla::sensor::data::ApolloObstacle(apollo_id, 
                                                                         apollo_type,
                                                                         apollo_bbox,
                                                                         apollo_rotation,
                                                                         apollo_location,
                                                                         apollo_velocity,
                                                                         apollo_angular_velocity,
                                                                         apollo_acceleration));
      }

      auto Stream = GetDataStream(*this);
      Stream.Send(*this, apollo_obstacles);
    }
  }

  // next pesudo codes
  // FActorRegistry registry = GetEpisode().GetActorRegistry();
  // FCarlaActor carla_actor = GetEpisode().FindCarlaActor(actor);
  // FWorldObserver_GetAcceleration(carla_actor, velocity, DeltaSeconds);
  //GetActorId(), GetActorType(), GetActorGlobalTransform(), GetActorVelocity(), GetActorAngularVelocity()

}

  void AApolloPerceptionSensor::BeginPlay()
  {
    Super::BeginPlay();
  }

