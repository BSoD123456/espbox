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

static struct {
    gpio_num_t pin;
    bool pullup;
} g_keymap[] = {
    {BTN_PIN_UP, true},
    {BTN_PIN_DOWN, true},
    {BTN_PIN_LEFT, true},
    {BTN_PIN_RIGHT, false},
    {BTN_PIN_A, false},
    {BTN_PIN_B, true},
};

uint32_t ebx_ipt_get(void) {
    uint32_t keys = 0;
    for(int i = 0; i < EBX_IPT_NUM_KEYS; i++) {
        int kv = gpio_get_level(g_keymap[i].pin);
        if(!kv) {
            keys |= (1 << i);
        }
    }
    return keys;
}

void ebx_ipt_init(void) {
    for(int i = 0; i < EBX_IPT_NUM_KEYS; i++) {
        ESP_ERROR_CHECK(gpio_set_direction(g_keymap[i].pin, GPIO_MODE_INPUT));
        if(g_keymap[i].pullup) {
            ESP_ERROR_CHECK(gpio_set_pull_mode(g_keymap[i].pin, GPIO_PULLUP_ONLY));
        }
    }
}
