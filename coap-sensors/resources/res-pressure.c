#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "coap-engine.h"
#include "utils/randomize.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define MEAN 100.327230
#define STDDEV 10.330774

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_pressure,
               "title=\"Observable resource\";pressure",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

static double current_pressure = 0;

static void
res_event_handler(void)
{
    current_pressure = generate_gaussian(MEAN, STDDEV);
    LOG_INFO("Payload to be sent: {\"sensor\":\"pressure\", \"value\":%.2f}\n", current_pressure);
    coap_notify_observers(&res_pressure);
}

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    coap_set_header_content_format(response, APPLICATION_JSON);
    int payload_len = snprintf((char *)buffer, preferred_size, "{\"sensor\":\"pressure\", \"value\":%.2f}", current_pressure);
    coap_set_payload(response, buffer, payload_len);

    LOG_INFO("Payload: %s\n", buffer);
}