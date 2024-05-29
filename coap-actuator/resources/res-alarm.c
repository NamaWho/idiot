#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "coap-engine.h"
#include "global.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

extern int component_number;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_alarm,
               "title=\"Observable resource\";alarm",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

static void
res_event_handler(void)
{
    LOG_INFO("Payload to be sent: {\"sensor\":\"alarm\", \"value\":%d}\n", component_number);
    coap_notify_observers(&res_alarm);
}

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    coap_set_header_content_format(response, APPLICATION_JSON);
    int payload_len = snprintf((char *)buffer, preferred_size, "{\"sensor\":\"alarm\", \"value\":%d}", component_number);
    coap_set_payload(response, buffer, payload_len);
}