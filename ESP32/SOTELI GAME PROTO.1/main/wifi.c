#include "esp_mac.h"
#include "esp_wifi.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "wifi.h"

static const char * roverhubAddress = "10.83.82.254";

static const unsigned short roverhubPort = 12345;


static wifi_config_t wifi_config =
{
    .sta =
    {
        .ssid               = "rover",
        .password           = "$Ol4r-R0v3r",
        .threshold.authmode = WIFI_AUTH_WPA2_PSK
    }
};

static uint8_t wifi_up_p = 0;


static void event_handler(void * arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void * event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
            case WIFI_EVENT_STA_DISCONNECTED:

                wifi_up_p = 0;
                esp_wifi_connect();
                break;

            default:
                printf("Unhandled WIFI_EVENT %ld!\n", event_id);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        wifi_up_p = 1;
    }
    else
    {
        printf("Unhandled event %s.%ld!\n", event_base, event_id);
    }
}


int wifi_init(void)
{
    if (esp_netif_init() != ESP_OK)
    {
        printf("Unable to initialize ESP32 networking stack.\n"); return 0;
    }


    esp_netif_t * sta_netif = esp_netif_create_default_wifi_sta();

    if (!sta_netif)
    {
        printf("Wi-Fi: unable to create default interface.\n"); return 0;
    }


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    if (esp_wifi_init(&cfg) != ESP_OK)
    {
        printf("Wi-Fi: initialization (with default config) failed.\n");

        return 0;
    }


    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK
        (esp_event_handler_instance_register(WIFI_EVENT,
                                             ESP_EVENT_ANY_ID,
                                             &event_handler,
                                             NULL,
                                             &instance_any_id));

    ESP_ERROR_CHECK
        (esp_event_handler_instance_register(IP_EVENT,
                                             IP_EVENT_STA_GOT_IP,
                                             &event_handler,
                                             NULL,
                                             &instance_got_ip));

    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK)
    {
        printf("Wi-Fi: unable to set 'station'-mode\n"); return 0;
    }


    if (esp_wifi_set_config(WIFI_IF_STA, &wifi_config) != ESP_OK)
    {
        printf("Wi-Fi: unable to configure!\n"); return 0;
    }

    if (esp_wifi_start() != ESP_OK)
    {
        printf("Wi-Fi: unable to start!\n"); return 0;

    }

    return 1;
}


int wifi_send_if_associated(uint16_t color,
                            uint16_t value,
                            uint8_t  value_decplaces)
{
 /* Stuurt een kleur en een (sensor)waarde naar het dashboard.

    `decplaces` geeft het aantal getallen achter de komma aan.

    Getal 123.45 kan je dus oversturen als integer 12345 met `decplaces` 2. */

    if (!wifi_up_p)
    {
        printf("Wi-Fi: not associated (yet)\n");

        return 0;
    }

    struct sockaddr_in dest_addr = {};

    dest_addr.sin_addr.s_addr = inet_addr(roverhubAddress);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(roverhubPort);
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;


    int s = socket(addr_family, SOCK_DGRAM, ip_protocol);

    if (s < 0)
    {
        printf("Net: cannot create a new socket.\n"); return 0;
    }


    uint8_t message[32] = {};

    message[0] = 'R';
    message[1] = 'V';
    message[2] = 'H';
    message[3] = 'v';

    message[20] = color >> 8;
    message[21] = color & 0xff;

    message[22] = value >> 8;
    message[23] = value & 0xff;

    message[28] = value_decplaces;

    int rs = sendto(s, message, 32, 0,
                    (struct sockaddr *) &dest_addr, sizeof(dest_addr));

    if (rs < 0)
    {
        printf("Net: cannot send packet to dashboard.\n");

        close(s); return 0;
    }

 // printf("Net: packet sent\n");

    close(s); return 1;
}
