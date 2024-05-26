#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "coap-engine.h"


#include "voltage_status.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP





static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
//static void res_event_handler(void);

RESOURCE(res_voltage_status,
               "title=\"Coap Voltage Status\";rt=\"voltage_status\"",
               res_get_handler,
               NULL,
               NULL,
               NULL);


/* static void
res_event_handler(void)
{
    
    LOG_INFO("Payload to be sent: {\"sensor\":\"voltage\", \"status\":%d}\n", status);
    coap_notify_observers(&res_voltage_status);
}*/

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    coap_set_header_content_format(response, APPLICATION_JSON);
    int payload_len = snprintf((char *)buffer, preferred_size, "{\"sensor\":\"voltage\", \"status\":%d}", status);
    coap_set_payload(response, buffer, payload_len);

    LOG_INFO("Payload: %s\n", buffer);
}