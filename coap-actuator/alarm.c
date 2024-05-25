#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "sys/etimer.h"
#include "DT_model.h"
#include "json.h"

/*------Queue functionalities*--------*/
#include "dynamic_queue.h"
/*------------------------------------*/

// button library
#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

// led library
#include "dev/leds.h"

/* ------- Log configuration ------- */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/* ------------------------------- */


/* ------ CoAP resources ------ */
#define SERVER_EP "coap://[fd00::1]:5683"

#define TOGGLE_INTERVAL 30

/*----------------------------------------------------------------------------*/

static coap_observee_t *obs_rotation;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static coap_observee_t *obs_voltage;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static coap_observee_t *obs_pressure;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static coap_observee_t *obs_vibration;

/*----------------------------------------------------------------------------*/

// static char *service_url = "/telemetry";

static bool registered = false;

PROCESS(alarm_client, "Alarm Actuator Client");
AUTOSTART_PROCESSES(&alarm_client);

/*------------------------SENSOR VALUES---------------------------------------------*/
static DynamicQueue rotation_queue;
static DynamicQueue voltage_queue;
static DynamicQueue pressure_queue;
static DynamicQueue vibration_queue;
/*-----------------------------------------------------------------------------------*/

/*
* Handle the value received from the sensor
*/
static int store_value(char *sensor, double value)
{
  if (strcmp(sensor, "rotation") == 0)
  {
    LOG_INFO("Rotation: %.2f\n", value);
    // Insert the value in the queue
    enqueue(&rotation_queue, value);
  }
  else if (strcmp(sensor, "voltage") == 0)
  {
    LOG_INFO("Voltage: %.2f\n", value);
    // Insert the value in the queue
    enqueue(&voltage_queue, value);
  }
  else if (strcmp(sensor, "pressure") == 0)
  {
    LOG_INFO("Pressure: %.2f\n", value);
    // Insert the value in the queue
    enqueue(&pressure_queue, value);
  }
  else if (strcmp(sensor, "vibration") == 0)
  {
    LOG_INFO("Vibration: %.2f\n", value);
    // Insert the value in the queue
    enqueue(&vibration_queue, value);
  }
  else
  {
    LOG_INFO("Invalid sensor\n");
    return -1;
  }

  return 0;
}
/*
 * Handle the response to the observe request and the following notifications
 */
static void
notification_callback(coap_observee_t *obs, void *notification,
                      coap_notification_flag_t flag)
{
  int len = 0;
  const uint8_t *payload = NULL;

  LOG_INFO("Notification handler\n");
  LOG_INFO("Observee URI: %s\n", obs->url);

  if (notification){
    len = coap_get_payload(notification, &payload);

  }
  switch(flag) {
    
  case NOTIFICATION_OK:
    LOG_INFO("NOTIFICATION OK: %*s\n", len, (char *)payload);

    char *sensor = json_parse_string((char *)payload, "sensor");
    double value = json_parse_number((char *)payload, "value");

    LOG_INFO("Sensor: %s\n", sensor);
    LOG_INFO("Value: %.2f\n", value);

    // verify if the values are valid: sesnor must not be null and value must be greater than 0
    if(sensor == NULL || value < 0){
      LOG_INFO("Invalid sensor or value\n");
      return;
    }

    // call a function to handle the store the value
    store_value(sensor, value);
    
    break;

  case OBSERVE_OK:
    LOG_INFO("OBSERVE_OK: %*s\n", len, (char *)payload);
    break;
  case OBSERVE_NOT_SUPPORTED:
    LOG_INFO("OBSERVE_NOT_SUPPORTED: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case ERROR_RESPONSE_CODE:
    LOG_INFO("ERROR_RESPONSE_CODE: %*s\n", len, (char *)payload);
    obs = NULL;
    break;
  case NO_REPLY_FROM_SERVER:
    LOG_INFO("NO_REPLY_FROM_SERVER: "
             "removing observe registration with token %x%x\n",
             obs->token[0], obs->token[1]);
    obs = NULL;
    break;
  }
}

/*
 * Toggle the observation of the remote resource
 */
void
toggle_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri)
{
  if (obs)
  {
    LOG_INFO("Stopping observation\n");
    coap_obs_remove_observee(obs);
    obs = NULL;
  }
  else
  {
    LOG_INFO("Starting observation at %s\n", res_uri);
    obs = coap_obs_request_registration(server_ep, res_uri, notification_callback, NULL);
  }
}

void client_chunk_handler_registration(coap_message_t *response)
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

  // Extract the IP addresses from the response
  char *rotation_ip = json_parse_string((char *)chunk, "rotation_ip_port");
  char *voltage_ip = json_parse_string((char *)chunk, "voltage_ip_port");
  char *pressure_ip = json_parse_string((char *)chunk, "pressure_ip_port");
  char *vibration_ip = json_parse_string((char *)chunk, "vibration_ip_port");

  // Parse the extracted IP addresses to coap_endpoint_t structures
  static coap_endpoint_t rotation_server_ep;
  static coap_endpoint_t voltage_server_ep;
  static coap_endpoint_t pressure_server_ep;
  static coap_endpoint_t vibration_server_ep;

  coap_endpoint_parse(rotation_ip, strlen(rotation_ip), &rotation_server_ep);
  coap_endpoint_parse(voltage_ip, strlen(voltage_ip), &voltage_server_ep);
  coap_endpoint_parse(pressure_ip, strlen(pressure_ip), &pressure_server_ep);
  coap_endpoint_parse(vibration_ip, strlen(vibration_ip), &vibration_server_ep);

  // Observe the resources
  toggle_observation(obs_rotation, &rotation_server_ep, "/rotation");
  toggle_observation(obs_voltage, &voltage_server_ep, "/voltage");
  toggle_observation(obs_pressure, &pressure_server_ep, "/pressure");
  toggle_observation(obs_vibration, &vibration_server_ep, "/vibration");
}

PROCESS_THREAD(alarm_client, ev, data)
{
  static coap_endpoint_t main_server_ep;
  static struct etimer et, check_timer;
  static coap_message_t request[1];

    PROCESS_BEGIN();

  coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);

  coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
  coap_set_header_uri_path(request, "register/");
  const char msg[] = "alarm";
  coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);
  leds_single_on(LEDS_YELLOW);
  COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler_registration);
  LOG_INFO("--Registered--\n");
  leds_single_off(LEDS_YELLOW);
  leds_single_on(LEDS_GREEN);

  // set the timer
  etimer_set(&et, 5 * CLOCK_SECOND);

    while (1){
      PROCESS_YIELD();

      if(etimer_expired(&et) || etimer_expired(&check_timer)){

        // if all the queues have 24 elements, make a prediction

        if(rotation_queue.size == 24 && voltage_queue.size == 24 && pressure_queue.size == 24 && vibration_queue.size == 24){
          
          // prepare vector of float values: calculate the mean of the values in the queue
          float rotation_mean = 0;
          float voltage_mean = 0;
          float pressure_mean = 0;
          float vibration_mean = 0;

          for(int i = 0; i < 24; i++){
            rotation_mean += rotation_queue.buffer[i];
            voltage_mean += voltage_queue.buffer[i];
            pressure_mean += pressure_queue.buffer[i];
            vibration_mean += vibration_queue.buffer[i];
          }

          rotation_mean /= 24;
          voltage_mean /= 24;
          pressure_mean /= 24;
          vibration_mean /= 24;

          float values[9] = {rotation_mean, voltage_mean, pressure_mean, vibration_mean, 1.0, 0.0, 0.0, 1.0, 0.0};
  
          // make prediction
          int prediction = model_predict(values, 9);

          LOG_INFO("Prediction: %d\n", prediction);

          // reset the timer
          etimer_reset(&et);
        }
        else{
          // set a smaller timer to check the values again
          etimer_set(&check_timer, 2 * CLOCK_SECOND);
        }
      } 
  
    }

    PROCESS_END();
}
