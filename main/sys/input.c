#include <stdio.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_input.h"

#define BTN_PIN_UP      2
#define BTN_PIN_DOWN    13
#define BTN_PIN_LEFT    27
#define BTN_PIN_RIGHT   35
#define BTN_PIN_A       34
#define BTN_PIN_B       12

#define INIT_BTN(pin)   \
    ESP_ERROR_CHECK(gpio_set_direction(pin, GPIO_MODE_INPUT));  \
    ESP_ERROR_CHECK(gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY))

//static const char* TAG = "ebx_input";

void ebx_input_init(void) {
    INIT_BTN(BTN_PIN_UP);
    INIT_BTN(BTN_PIN_DOWN);
    INIT_BTN(BTN_PIN_LEFT);
    INIT_BTN(BTN_PIN_RIGHT);
    INIT_BTN(BTN_PIN_A);
    INIT_BTN(BTN_PIN_B);
}
