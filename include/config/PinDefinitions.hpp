#ifndef PIN_DEFINITIONS_HPP
#define PIN_DEFINITIONS_HPP

#include <driver/gpio.h>

namespace config {
namespace pins {

// Sensor GPIOs
constexpr gpio_num_t DHT11_PIN     = GPIO_NUM_4;
constexpr gpio_num_t DS18B20_PIN   = GPIO_NUM_5;

// TFT ST7735 Display SPI GPIOs
constexpr gpio_num_t TFT_BL_PIN    = GPIO_NUM_7;
constexpr gpio_num_t TFT_RST_PIN   = GPIO_NUM_8;
constexpr gpio_num_t TFT_DC_PIN    = GPIO_NUM_9;
constexpr gpio_num_t TFT_CS_PIN    = GPIO_NUM_10;
constexpr gpio_num_t TFT_MOSI_PIN  = GPIO_NUM_11;
constexpr gpio_num_t TFT_SCK_PIN   = GPIO_NUM_12;

// Indicator Outputs
constexpr gpio_num_t GREEN_LED_PIN = GPIO_NUM_16;
constexpr gpio_num_t WHITE_LED_PIN = GPIO_NUM_17;
constexpr gpio_num_t BUZZER_PIN    = GPIO_NUM_18;

} // namespace pins
} // namespace config

#endif // PIN_DEFINITIONS_HPP
