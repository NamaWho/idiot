#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "coap-engine.h"
#include "utils/randomize.h"
#include "math.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define MEAN 458.528826
#define STDDEV 38.801713

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_rotation,
               "title=\"Observable resource\";rotation",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

static double current_rotation = 0;

static void
res_event_handler(void)
{
  current_rotation = generate_gaussian(MEAN, STDDEV);
  LOG_INFO("Payload to be sent: {\"sensor\":\"rotation\", \"value\":%.2f}\n", current_rotation);
  coap_notify_observers(&res_rotation);
}

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_set_header_content_format(response, APPLICATION_JSON);
  int payload_len = snprintf((char *)buffer, preferred_size, "{\"sensor\":\"rotation\", \"value\":%.2f}", current_rotation);
  coap_set_payload(response, buffer, payload_len);

  LOG_INFO("Payload: %s\n", buffer);
}