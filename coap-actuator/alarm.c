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

#define MAX_REGISTRATION_RETRY 3

static int max_registration_retry = MAX_REGISTRATION_RETRY;
static int max_retry = 3;

/*static int status = 0;

static double rotation_status = 0.0;
static double voltage_status = 0.0;
static double pressure_status = 0.0;
static double vibration_status = 0.0;*/

/* ------ CoAP resources ------ */
#define SERVER_EP "coap://[fd00::1]:5683"

#define TOGGLE_INTERVAL 30

/*----------------------------------------------------------------------------*/

static coap_endpoint_t main_server_ep;
/*static coap_observee_t *obs_rotation;
static coap_observee_t *obs_voltage;
static coap_observee_t *obs_pressure;
static coap_observee_t *obs_vibration;
static coap_observee_t *obs_control;*/

/*----------------------------------------------------------------------------*/

PROCESS(alarm_client, "Alarm Actuator Client");
AUTOSTART_PROCESSES(&alarm_client);

/*------------------------SENSOR VALUES---------------------------------------------*/
static DynamicQueue rotation_queue;
static DynamicQueue voltage_queue;
static DynamicQueue pressure_queue;
static DynamicQueue vibration_queue;
/*-----------------------------------------------------------------------------------*/

/* ----------------------- SIGNATURES ----------------------- */
static void client_chunk_handler_registration(coap_message_t *response);

//static void toggle_server_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri);
//sstatic void notification_callback(coap_observee_t *obs, void *notification, coap_notification_flag_t flag);

static void actuator_chunk_handler(coap_message_t *response);
//static int store_value(char *sensor, double value);
/*----------------------------------------------------------------*/

/*
 * Handle the value received from the sensor
 */
/*static int store_value(char *sensor, double value)
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
}*/

/*
 * Handle the response to the observe request and the following notifications
 */
/*static void
notification_callback(coap_observee_t *obs, void *notification,
                      coap_notification_flag_t flag)
{
  int len = 0;
  const uint8_t *payload = NULL;

  LOG_INFO("Notification handler\n");
  LOG_INFO("Observee URI: %s\n", obs->url);

  if (notification)
  {
    len = coap_get_payload(notification, &payload);
  }
  switch (flag)
  {

  case NOTIFICATION_OK:
    LOG_INFO("NOTIFICATION OK: %*s\n", len, (char *)payload);

    char *sensor = json_parse_string((char *)payload, "sensor");
    double value = json_parse_number((char *)payload, "value");

    LOG_INFO("Sensor: %s\n", sensor);
    LOG_INFO("Value: %.2f\n", value);

    // verify if the values are valid: sesnor must not be null and value must be greater than 0
    if (sensor == NULL || value < 0)
    {
      LOG_INFO("Invalid sensor or value\n");
      return;
    }

    // call a function to handle the store of the value
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
}*/

/*
 * Toggle the observation of the remote resource
 */
/*static void toggle_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri)
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
}*/



static void client_chunk_handler_registration(coap_message_t *response)
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

static void actuator_chunk_handler(coap_message_t *response)
{
  if (response == NULL)
  {

    LOG_ERR("Request timed out\n");
  }
  else
  {
    const uint8_t *payload = NULL;

    int len = coap_get_payload(response, &payload);

    if(len > 0){
      
        LOG_INFO("Received response: %s\n", (char *)payload);
        char *sensor = json_parse_string((char *)payload, "pressure");
        LOG_INFO("Sensor: %s\n", sensor);
        max_retry = 0; // if = 0 --> data received!
        return;
    }
    else{
        LOG_INFO("Empty payload\n");
    }
  }

  // If I'm at this point, there was some problem in the registration phasse, so we decide to try again until max_registration_retry != 0
  max_retry--;
  if (max_retry == 0)
    max_retry = -1;
}

PROCESS_THREAD(alarm_client, ev, data)
{
  static struct etimer et, sleep_timer;
  static coap_message_t request[1];


  PROCESS_BEGIN();

  while (max_registration_retry != 0)
  {
    /* -------------- REGISTRATION --------------*/
    // Populate the coap_endpoint_t data structure
    coap_endpoint_parse(SERVER_EP, strlen(SERVER_EP), &main_server_ep);
    // Prepare the message
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, "register/");
    const char msg[] = "alarm";
    // Set payload
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

    leds_single_on(LEDS_YELLOW);

    COAP_BLOCKING_REQUEST(&main_server_ep, request, client_chunk_handler_registration);

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
  leds_single_on(LEDS_GREEN);
  
  

  // retrieve the IPs of the sensors
  LOG_INFO("Retrieving IPs of sensors\n");
  leds_single_off(LEDS_GREEN);
  leds_single_on(LEDS_YELLOW);

  while (max_retry != 0)
  {
    /* -------------- GETTING IPs OF SENSORS --------------*/
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, "control/");
    const char msg[] = "alarm";
    // Set payload
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

    leds_single_on(LEDS_YELLOW);

    COAP_BLOCKING_REQUEST(&main_server_ep, request, actuator_chunk_handler);

    /* -------------- END --------------*/
    if (max_retry == -1)
    { 
      etimer_set(&sleep_timer, 30 * CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
      max_retry = 3;
    }
  }

  LOG_INFO("IPs RETRIEVED\n");
  leds_single_off(LEDS_YELLOW);
  leds_single_on(LEDS_GREEN);



  // set the timer
  etimer_set(&et, 60 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_YIELD();

    if (etimer_expired(&et))
    {

      // if all the queues have 24 elements, make a prediction

      if (rotation_queue.size == 24 && voltage_queue.size == 24 && pressure_queue.size == 24 && vibration_queue.size == 24)
      {

        // prepare vector of float values: calculate the mean of the values in the queue
        float rotation_mean = 0;
        float voltage_mean = 0;
        float pressure_mean = 0;
        float vibration_mean = 0;

        for (int i = 0; i < 24; i++)
        {
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
      }
      // reset the timer
      etimer_reset(&et);
    }
  }

  PROCESS_END();
}
