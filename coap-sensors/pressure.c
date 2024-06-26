#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "os/dev/leds.h"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

#include "pressure_status.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SERVER_EP "coap://[fd00::1]:5683"

#define MAX_REGISTRATION_RETRY 3

static int max_registration_retry = MAX_REGISTRATION_RETRY;

void client_chunk_handler(coap_message_t *response)
{
  if (response == NULL)
  {

    LOG_ERR("Request timed out\n");
  }
  else
  {

    LOG_INFO("Registration successful\n");
    max_registration_retry = 0; // if = 0 --> registration ok!

    return;
  }

  // If I'm at this point, there was some problem in the registration phasse, so we decide to try again until max_registration_retry != 0
  max_registration_retry--;
  if (max_registration_retry == 0)
    max_registration_retry = -1;
}

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t res_pressure;
extern coap_resource_t res_pressure_status;

static struct etimer e_timer, sleep_timer;

PROCESS(pressure_server, "Pressure Sensor CoAP Server");
AUTOSTART_PROCESSES(&pressure_server);

int status = 1;

PROCESS_THREAD(pressure_server, ev, data)
{
  static coap_endpoint_t main_server_ep;
  static coap_message_t request[1];

  PROCESS_BEGIN();

#if PLATFORM_HAS_BUTTON
#if !PLATFORM_SUPPORTS_BUTTON_HAL
  SENSORS_ACTIVATE(button_sensor);
#endif
  printf("Press a button to switch the pressure status\n");
#endif

  LOG_INFO("Starting Pressure Server\n");

  coap_activate_resource(&res_pressure, "pressure");
  coap_activate_resource(&res_pressure_status, "pressure/status");

  while (max_registration_retry != 0)
  {
    /* -------------- REGISTRATION --------------*/
    // Populate the coap_endpoint_t data structure
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
    // Prepare the message
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, "register/");
    const char msg[] = "pressure";
    // Set payload
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

    leds_single_on(LEDS_YELLOW);

    COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler);

    /* -------------- END REGISTRATION --------------*/
    if (max_registration_retry == -1)
    { // something goes wrong more MAX_REGISTRATION_RETRY times, node goes to sleep then try again
      etimer_set(&sleep_timer, 30 * CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
      max_registration_retry = MAX_REGISTRATION_RETRY;
    }
  }

  LOG_INFO("REGISTRATION SUCCESS\n");
  leds_single_off(LEDS_YELLOW);

  // set a timer to send the pressure value every 10 seconds
  etimer_set(&e_timer, CLOCK_SECOND * 10);

  while (1)
  {

    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER && data == &e_timer)
    {
      if (status == 1)
      {
        res_pressure.trigger();
        LOG_INFO("Pressure event triggered\n");
      }
      etimer_reset(&e_timer);

#if PLATFORM_HAS_BUTTON
#if PLATFORM_SUPPORTS_BUTTON_HAL
    }
    else if (ev == button_hal_release_event)
    {
#else
    }
    else if (ev == sensors_event && data == &button_sensor)
    {
#endif

      LOG_INFO("Button pressed: switch the pressure status from %d to %d\n", status, !status);

      status = !status;

      if (status == 1)
      {
        // set a timer to send the pressure value every 10 seconds
        etimer_set(&e_timer, CLOCK_SECOND * 10);
      }

#endif /* PLATFORM_HAS_BUTTON */
    }
  }

  PROCESS_END();
}
