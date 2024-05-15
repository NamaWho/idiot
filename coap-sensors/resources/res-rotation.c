#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);
// static uint8_t generate_rotation(void);

EVENT_RESOURCE(res_rotation,
         "title=\"Observable resource\";rotation",
         res_get_handler,
         NULL,
         NULL,
         NULL, 
		 res_event_handler);

static int counter = 0;

static void
res_event_handler(void)
{
	counter++;
    // Notify all the observers
    coap_notify_observers(&res_rotation);
}


static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_set_header_content_format(response, TEXT_PLAIN);
  coap_set_payload(response, buffer, snprintf((char *)buffer, preferred_size, "EVENT %lu", (unsigned long) counter));
}

// static uint8_t generate_rotation(){
//     // generate a random value
//     uint8_t rotation = rand() % 100;
//     return rotation;
// }