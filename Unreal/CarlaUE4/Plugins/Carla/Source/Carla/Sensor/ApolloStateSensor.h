#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"

#include "ApolloStateSensor.generated.h"

UCLASS()
class CARLA_API AApolloStateSensor : public ASensor
{
  GENERATED_BODY()

public:

  AApolloStateSensor(const FObjectInitializer &ObjectInitializer);

  static FActorDefinition GetSensorDefinition();

  void SetOwner(AActor *Owner) override;

  virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds) override;

private:
  virtual void BeginPlay() override;
};