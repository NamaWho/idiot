#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "os/dev/leds.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fd00::1]:5683"

bool registered = false;

void client_chunk_handler(coap_message_t *response)
{
  const uint8_t *chunk;

  if (response == NULL)
  {
    LOG_INFO("Request timed out");
    return;
  }
  registered = true;
  int len = coap_get_payload(response, &chunk);
  LOG_INFO("|%.*s \n", len, (char *)chunk);
}

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_pressure;
static struct etimer e_timer;

PROCESS(pressure_server, "Pressure Sensor CoAP Server");
AUTOSTART_PROCESSES(&pressure_server);

PROCESS_THREAD(pressure_server, ev, data)
{
  static coap_endpoint_t main_server_ep;
  static coap_message_t request[1];

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  LOG_INFO("Starting Pressure Server\n");

  coap_activate_resource(&res_pressure, "pressure");

  
  coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
  coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
  coap_set_header_uri_path(request, "register/");
  const char msg[] = "pressure";
  coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
  leds_single_on(LEDS_YELLOW);
  
  COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler);
  LOG_INFO("--Registered--\n");

  // set a timer to send the pressure value every 10 seconds
  etimer_set(&e_timer, CLOCK_SECOND * 10);

  while (1)
  {
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER && data == &e_timer)
    {
      if (registered)
        res_pressure.trigger();

      LOG_INFO("Pressure event triggered\n");
      etimer_reset(&e_timer);
    }
  }

  PROCESS_END();
}
