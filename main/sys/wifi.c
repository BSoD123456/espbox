#include <stdio.h>
#include <string.h>

#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ebx_wifi.h"

static const char* TAG = "ebx_wifi";

void ebx_wifi_init(void) {
    ESP_LOGI(TAG, "Initializing WIFI AP");
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EBX_WIFI_AP_SSID,
            .ssid_len = strlen(EBX_WIFI_AP_SSID),
            .channel = EBX_WIFI_AP_CHANNEL,
            .max_connection = EBX_WIFI_AP_MAXCONN,
            .authmode = WIFI_AUTH_OPEN,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WIFI AP: ssid=" EBX_WIFI_AP_SSID " channel=%d", EBX_WIFI_AP_CHANNEL);
}
