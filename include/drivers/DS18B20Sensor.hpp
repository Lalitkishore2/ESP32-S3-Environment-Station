#ifndef DS18B20_SENSOR_HPP
#define DS18B20_SENSOR_HPP

#include <OneWire.h>
#include <DallasTemperature.h>
#include <driver/gpio.h>

namespace drivers {

class DS18B20Sensor {
public:
    explicit DS18B20Sensor(gpio_num_t pin);
    bool init();
    bool readTemperature(float& outTempC);

private:
    gpio_num_t pin_;
    OneWire oneWire_;
    DallasTemperature sensors_;
    bool initialized_;
};

} // namespace drivers

#endif // DS18B20_SENSOR_HPP
