#ifndef wifi_h
#define wifi_h

int wifi_init(void);

int wifi_send_if_associated(uint16_t color,
                            uint16_t value,
                            uint8_t  value_decplaces);

#endif
