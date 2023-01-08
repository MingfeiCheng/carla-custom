// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <exception>
#include <fstream>

#include "Carla.h"
#include "Carla/Sensor/WorldObserver.h"
#include "Carla/Sensor/ApolloPerceptionSensor.h"

#include "Carla/Game/CarlaStatics.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Util/BoundingBox.h"
#include "Carla/Actor/CarlaActor.h"
// #include "Carla/Vehicle/CarlaWheeledVehicle.h"

#include "CoreGlobals.h"

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
      
      // const FActorRegistry &registry = episode.GetActorRegistry();
      // const UCarlaEpisode* Episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
      // const auto &Episode = GetEpisode();
      ObstacleArray ApolloObstacles;
      constexpr float TO_METERS = 100.0f;

      for(auto& It : DetectedActors){
        // const FCarlaActor* View = It.Value.Get();
        const FCarlaActor* ActorView = Episode->FindCarlaActor(It);
        const FActorInfo* ActorInfo = ActorView->GetActorInfo();

        const carla::rpc::ActorId ApolloActorId = ActorView->GetActorId();
        FBoundingBox ActorBBox = UBoundingBoxCalculator::GetActorBoundingBox(It);
        const carla::geom::Location ActorBBoxLocation = carla::geom::Location(ActorBBox.Origin.X, ActorBBox.Origin.Y, ActorBBox.Origin.Z);
        const carla::geom::Vector3D ActorBBoxExtent = carla::geom::Vector3D(ActorBBox.Extent.X, ActorBBox.Extent.Y, ActorBBox.Extent.Z);
        const carla::geom::Rotation ActorBBoxRotation = carla::geom::Rotation(ActorBBox.Rotation.Pitch, ActorBBox.Rotation.Yaw, ActorBBox.Rotation.Roll);
        const carla::geom::BoundingBox ApolloActorBBox = carla::geom::BoundingBox(ActorBBoxLocation, ActorBBoxExtent, ActorBBoxRotation);
        FCarlaActor::ActorType ActorType = ActorView->GetActorType();
        std::string ApolloActorType = "unknown";
        
        if (FCarlaActor::ActorType::Vehicle == ActorType){
          ApolloActorType = "dynamic.vehicle";
        }else if (FCarlaActor::ActorType::TrafficSign == ActorType){
          ApolloActorType = "static.traffic_sign";
        }else if (FCarlaActor::ActorType::TrafficLight == ActorType){
          ApolloActorType = "static.traffic_light";
        }else if (FCarlaActor::ActorType::Walker == ActorType){
          ApolloActorType = "dynamic.walker";
        }       

        FTransform ActorTransform;
        FVector Velocity(0.0f);
        // FVector Acceleration(0.0f);

        check(ActorView);

        if(ActorView->IsDormant())
        {
          const FActorData* ActorData = ActorView->GetActorData();
          Velocity = TO_METERS * ActorData->Velocity;
          // AngularVelocity = carla::geom::Vector3D
          //                   {ActorData->AngularVelocity.X,
          //                   ActorData->AngularVelocity.Y,
          //                   ActorData->AngularVelocity.Z};
          // Acceleration = FWorldObserver_GetAcceleration(*View, Velocity, DeltaSeconds);
          // State = FWorldObserver_GetDormantActorState(*View, Registry);
        }
        else
        {
          Velocity = TO_METERS * ActorView->GetActor()->GetVelocity();
          // AngularVelocity = FWorldObserver_GetAngularVelocity(*View->GetActor());
          // Acceleration = FWorldObserver_GetAcceleration(*View, Velocity, DeltaSeconds);
          // State = FWorldObserver_GetActorState(*View, Registry);
        }

        FVector &PreviousVelocity = ActorView->GetActorInfo()->Velocity;
        const FVector Acceleration = (Velocity - PreviousVelocity) / DeltaSeconds;
        PreviousVelocity = Velocity;
        ActorTransform = ActorView->GetActorGlobalTransform();
        const FVector ActorLocation = ActorTransform.GetLocation();
        const FRotator ActorRotation = ActorTransform.GetRotation().Rotator();    

        carla::geom::Vector3D ApolloActorAcceleration = carla::geom::Vector3D(Acceleration.X, -Acceleration.Y, Acceleration.Z);
        carla::geom::Vector3D ApolloActorVelocity = carla::geom::Vector3D(Velocity.X, -Velocity.Y, Velocity.Z);
        const carla::geom::Location ApolloActorLocation = carla::geom::Location(ActorLocation.X, -ActorLocation.Y, ActorLocation.Z);
        const carla::geom::Rotation ApolloActorRotation = carla::geom::Rotation(ActorRotation.Pitch, ActorRotation.Yaw, ActorRotation.Roll);

        ApolloObstacles.push_back(carla::sensor::data::ApolloObstacle(ApolloActorId, 
                                                                      ApolloActorType,
                                                                      ApolloActorBBox,
                                                                      ApolloActorRotation,
                                                                      ApolloActorLocation,
                                                                      ApolloActorVelocity,
                                                                      ApolloActorAcceleration));
                
        // FCarlaActor *carla_actor = episode.FindCarlaActor(actor);        
        // const carla::rpc::ActorId apollo_id = carla_actor->GetActorId();
        //  // add logic 
        // FCarlaActor::ActorType type = carla_actor->GetActorType();
        // std::string apollo_type = "unknown";
        // if (AType::Vehicle == type){
        //   apollo_type = "dynamic.vehicle";
        // }else if (AType::TrafficSign == type){
        //   apollo_type = "static.traffic_sign";
        // }else if (AType::TrafficLight == type){
        //   apollo_type = "static.traffic_light";
        // }else if (AType::Walker == type){
        //   apollo_type = "dynamic.walker";
        // }
        // const carla::geom::BoundingBox apollo_bbox = UBoundingBoxCalculator::GetActorBoundingBox(actor);
        // // carla
        // FVector velocity = carla_actor->GetActorVelocity();
        // FVector angular_velocity = carla_actor->GetActorAngularVelocity();

        // const FActorData* actor_data = carla_actor->GetActorData();
        // velocity = actor_data->Velocity;
        // FVector &previous_velocity = carla_actor->GetActorInfo()->Velocity;
        // const FVector acceleration = (velocity - previous_velocity) / DeltaSeconds;
        // previous_velocity = velocity;

        // const carla::geom::Vector3D acceleration = FWorldObserver_GetAcceleration(carla_actor, velocity, DeltaSeconds);
        // const FTransform transform = carla_actor->GetActorGlobalTransform();
        // const FVector location = transform.GetLocation();
        // const FRotator rotation = transform.GetRotation().Rotator();       

        // // apollo
        // const carla::geom::Location apollo_location = carla::geom::Location(location.X, -location.Y, location.Z);
        // const carla::geom::Rotation apollo_rotation = carla::geom::Rotation(rotation.Pitch, rotation.Yaw, rotation.Roll);
        // const carla::geom::Vector3D apollo_velocity = carla::geom::Vector3D(velocity.X, -velocity.Y, velocity.Z);
        // const carla::geom::Vector3D apollo_acceleration = carla::geom::Vector3D(acceleration.x, -acceleration.y, acceleration.z);
        // const carla::geom::Vector3D apollo_angular_velocity = carla::geom::Vector3D(angular_velocity.X, angular_velocity.Y, angular_velocity.Z);

      }

      auto Stream = GetDataStream(*this);
      auto Buffer = Stream.PopBufferFromPool();
      Stream.Send(*this, ApolloObstacles, std::move(Buffer));
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
    Episode = UCarlaStatics::GetCurrentEpisode(GetWorld());
  }

