#ifndef __INC_EBX_WIFI_H__
#define __INC_EBX_WIFI_H__

#define EBX_WIFI_AP_SSID    "espbox_ap"
#define EBX_WIFI_AP_MAXCONN 1
#define EBX_WIFI_AP_CHANNEL CONFIG_ESP_WIFI_CHANNEL

void ebx_wifi_init(void);

#endif /*__INC_EBX_WIFI_H__*/
