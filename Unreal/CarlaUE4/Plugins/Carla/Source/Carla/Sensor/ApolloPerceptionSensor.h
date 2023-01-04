// #pragma once

// #include "Carla/Sensor/Sensor.h"

// #include "Carla/Actor/ActorDefinition.h"
// #include "Carla/Actor/ActorDescription.h"

// #include "Components/BoxComponent.h"

// #include "ApolloPerceptionSensor.generated.h"

// UCLASS()
// class CARLA_API AApolloPerceptionSensor : public ASensor
// {
//   GENERATED_BODY()

// public:

//   AApolloPerceptionSensor(const FObjectInitializer &ObjectInitializer);

//   static FActorDefinition GetSensorDefinition();

//   void Set(const FActorDescription &ActorDescription) override;

//   void SetOwner(AActor *Owner) override;

//   virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds) override;

// private:

//   UPROPERTY()
//   UBoxComponent *Box = nullptr;
// };