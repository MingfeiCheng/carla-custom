#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "carla/geom/GeoLocation.h"
#include "Carla/Actor/ActorDescription.h"

#include "ApolloStateSensor.generated.h"

UCLASS()
class CARLA_API AApolloStateSensor : public ASensor
{
  GENERATED_BODY()

public:

  AApolloStateSensor(const FObjectInitializer &ObjectInitializer);

  static FActorDefinition GetSensorDefinition();

  void Set(const FActorDescription &ActorDescription);

  void SetOwner(AActor *Owner) override;

  virtual void PostPhysTick(UWorld *World, ELevelTick TickType, float DeltaSeconds) override;

  void SetLatitudeDeviation(float Value);
  void SetLongitudeDeviation(float Value);
  void SetAltitudeDeviation(float Value);

  void SetLatitudeBias(float Value);
  void SetLongitudeBias(float Value);
  void SetAltitudeBias(float Value);

  float GetLatitudeDeviation() const;
  float GetLongitudeDeviation() const;
  float GetAltitudeDeviation() const;

  float GetLatitudeBias() const;
  float GetLongitudeBias() const;
  float GetAltitudeBias() const;

private:

  carla::geom::GeoLocation CurrentGeoReference;

  float LatitudeDeviation;
  float LongitudeDeviation;
  float AltitudeDeviation;

  float LatitudeBias;
  float LongitudeBias;
  float AltitudeBias;

  virtual void BeginPlay() override;
  
};