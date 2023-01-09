#pragma once

#include "carla/sensor/data/ApolloObstacle.h"
#include "carla/sensor/SensorData.h"

#include "carla/sensor/s11n/ApolloPerceptionSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ApolloObstacleArray : public SensorData {
    
    using Super = SensorData;

  protected:

    using Serializer = s11n::ApolloPerceptionSerializer;
    using ObstacleArray = std::vector<data::ApolloObstacle>;

    friend Serializer;

    explicit ApolloObstacleArray(const RawData &&data)
      : Super(data) {
        // how to put in this
        ObstacleArray obstacles = Serializer::DeserializeRawData(data);
        _obstacles = obstacles;
      }

  public:
    auto GetSize() const {
      return _obstacles.size();
    }

    data::ApolloObstacle GetObstacle(int index) const {
      return _obstacles[static_cast<ObstacleArray::size_type>(index)];
    }

  private:
    ObstacleArray _obstacles;
  };

} // namespace data
} // namespace sensor
} // namespace carla
