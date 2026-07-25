#ifndef DHT_SENSOR_HPP
#define DHT_SENSOR_HPP

#include <DHT.h>
#include <driver/gpio.h>

namespace drivers {

class DHTSensor {
public:
    explicit DHTSensor(gpio_num_t pin);
    bool init();
    bool readData(float& outTempC, float& outHumidity);

private:
    gpio_num_t pin_;
    DHT dht_;
    bool initialized_;
};

} // namespace drivers

#endif // DHT_SENSOR_HPP
