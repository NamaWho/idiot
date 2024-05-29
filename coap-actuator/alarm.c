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



static int rotation_status = 0;
static int voltage_status = 0;
static int pressure_status = 0;
static int vibration_status = 0;

/* ------ CoAP resources ------ */
#define SERVER_EP "coap://[fd00::1]:5683"

#define TOGGLE_INTERVAL 30

/*----------------------------------------------------------------------------*/

static coap_endpoint_t main_server_ep;
static coap_endpoint_t obs_rotation_ep;
static coap_endpoint_t obs_voltage_ep;
static coap_endpoint_t obs_pressure_ep;
static coap_endpoint_t obs_vibration_ep;
static coap_observee_t *obs_rotation;
static coap_observee_t *obs_voltage;
static coap_observee_t *obs_pressure;
static coap_observee_t *obs_vibration;


static char res_uri_pressure[50];
static char res_uri_vibration[50];
static char res_uri_voltage[50];
static char res_uri_rotation[50];

/*----------------------------------------------------------------------------*/

PROCESS(alarm_client, "Alarm Actuator Client");
AUTOSTART_PROCESSES(&alarm_client);

/*------------------------SENSOR VALUES---------------------------------------------*/
static DynamicQueue rotation_queue;
static DynamicQueue voltage_queue;
static DynamicQueue pressure_queue;
static DynamicQueue vibration_queue;
/*-----------------------------------------------------------------------------------*/

static coap_message_t request[1];

/* ----------------------- SIGNATURES ----------------------- */
static void client_chunk_handler_registration(coap_message_t *response);


static void notification_callback(coap_observee_t *obs, void *notification, coap_notification_flag_t flag);
static void toggle_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri);
static void actuator_chunk_handler(coap_message_t *response);
static int store_value(char *sensor, double value);
static void check_sensor(coap_endpoint_t *obs_ep, char *res_uri);
static void sensor_status_handler(coap_message_t *response);
/*----------------------------------------------------------------*/

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
}

/*
 * Toggle the observation of the remote resource
 */
static void toggle_observation(coap_observee_t *obs, coap_endpoint_t *server_ep, char *res_uri)
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

    if (len > 0)
    {

      LOG_INFO("Received response: %s\n", (char *)payload);

      /*  Parse the payload
          Example Payload: 202:2:2:2;204:4:4:4;205:5:5:5;203:3:3:3
          1. Split the payload by ";"
          2. For each sensor:
              - add prefix "coap://[fd00::"
              - add suffix "]:5683"

          Recalling order of sensors:
          1. Pressure
          2. Vibration
          3. Voltage
          4. Rotation

          Recalling example:
          1. Pressure: coap://[fd00::202:2:2:2]:5683
          2. Vibration: coap://[fd00::204:4:4:4]:5683
          3. Voltage: coap://[fd00::205:5:5:5]:5683
          4. Rotation: coap://[fd00::203:3:3:3]:5683
      */
      char *payload_copy = strdup((char *)payload);
      char *sensor = strtok((char *)payload_copy, ";");
      int i = 0;
      while (sensor != NULL)
      {

        switch (i)
        {
        case 0:
          snprintf(res_uri_pressure, sizeof(res_uri_pressure), "coap://[fd00::%s]:5683", sensor);
          LOG_INFO("Pressure URI: %s\n", res_uri_pressure);
          coap_endpoint_parse(res_uri_pressure, strlen(res_uri_pressure), &obs_pressure_ep);
          toggle_observation(obs_pressure, &obs_pressure_ep, "/pressure");
          break;
        case 1:
          snprintf(res_uri_vibration, sizeof(res_uri_vibration), "coap://[fd00::%s]:5683", sensor);
          LOG_INFO("Vibration URI: %s\n", res_uri_vibration);
          coap_endpoint_parse(res_uri_vibration, strlen(res_uri_vibration), &obs_vibration_ep);
          toggle_observation(obs_vibration, &obs_vibration_ep, "/vibration");
          break;
        case 2:
          snprintf(res_uri_voltage, sizeof(res_uri_voltage), "coap://[fd00::%s]:5683", sensor);
          LOG_INFO("Voltage URI: %s\n", res_uri_voltage);
          coap_endpoint_parse(res_uri_voltage, strlen(res_uri_voltage), &obs_voltage_ep);
          toggle_observation(obs_voltage, &obs_voltage_ep, "/voltage");
          break;
        case 3:
          snprintf(res_uri_rotation, sizeof(res_uri_rotation), "coap://[fd00::%s]:5683", sensor);
          LOG_INFO("Rotation URI: %s\n", res_uri_rotation);
          coap_endpoint_parse(res_uri_rotation, strlen(res_uri_rotation), &obs_rotation_ep);
          toggle_observation(obs_rotation, &obs_rotation_ep, "/rotation");
          break;
        default:
          break;
        }
        i++;

        sensor = strtok(NULL, ";");
      }

      free(payload_copy);

      // if all the sensors are observed, set max_retry to 0
      max_retry = 0;
      return;
    }
    else
    {
      LOG_INFO("Empty payload\n");
    }
  }

  // If I'm at this point, there was some problem in the registration phase, so we decide to try again until max_registration_retry != 0
  max_retry--;
  if (max_retry == 0)
    max_retry = -1;
}

static void sensor_status_handler(coap_message_t *response)
{
  if (response == NULL)
  {
    LOG_ERR("Request timed out\n");
  }
  else
  {
    const uint8_t *payload = NULL;

    int len = coap_get_payload(response, &payload);

    if (len > 0)
    {
      LOG_INFO("Received response: %s\n", (char *)payload);

      char *sensor = json_parse_string((char *)payload, "sensor");
      int status = (int) json_parse_number((char *)payload, "status");

      // verify if the values are valid: sesnor must not be null and value must be greater than 0
      if (sensor == NULL || status < 0)
      {
        LOG_INFO("Invalid data recieved\n");
        return;
      }

      if (strcmp(sensor, "rotation") == 0)
      {
        LOG_INFO("Rotation: %d\n", status);
        rotation_status = status;
      }
      else if (strcmp(sensor, "voltage") == 0)
      {
        LOG_INFO("Voltage: %d\n", status);
        voltage_status = status;
      }
      else if (strcmp(sensor, "pressure") == 0)
      {
        LOG_INFO("Pressure: %d\n", status);
        pressure_status = status;
      }
      else if (strcmp(sensor, "vibration") == 0)
      {
        LOG_INFO("Vibration: %d\n", status);
        vibration_status = status;
      }

      max_retry = 0;
      return;
    }
  }
  max_retry--;
  if(max_retry == 0)
    max_retry = -1;
}

static void check_sensor(coap_endpoint_t *obs_ep, char *res_uri)
{
  max_retry = 3;

  while (max_retry > 0)
  {
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, res_uri);

    const char msg[] = "alarm";
    // Set payload
    coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);


    COAP_BLOCKING_REQUEST(obs_ep, request, sensor_status_handler);

    if (max_retry == -1)
    {
      // after 3 tries, set the status to 0
      if (strcmp(res_uri, "/rotation/status") == 0)
      {
        rotation_status = 0;
      }
      else if (strcmp(res_uri, "/voltage/status") == 0)
      {
        voltage_status = 0;
      }
      else if (strcmp(res_uri, "/pressure/status") == 0)
      {
        pressure_status = 0;
      }
      else if (strcmp(res_uri, "/vibration/status") == 0)
      {
        vibration_status = 0;
      }
    }
  }

}

PROCESS_THREAD(alarm_client, ev, data)
{
  static struct etimer actuator_timer, sleep_timer, check_timer;
  static int actuator_status = 0;

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
      etimer_set(&sleep_timer, 10 * CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleep_timer));
      max_retry = 3;
    }
  }

  LOG_INFO("IPs RETRIEVED\n");
  leds_single_off(LEDS_YELLOW);
  leds_single_on(LEDS_GREEN);

  // set the timer
  etimer_set(&actuator_timer, 60 * CLOCK_SECOND);
  etimer_set(&check_timer, 10 * CLOCK_SECOND); // every 10 seconds check if the sensors are still active
  initQueue(&rotation_queue);
  initQueue(&voltage_queue);
  initQueue(&pressure_queue);
  initQueue(&vibration_queue);

  while (1)
  {
    PROCESS_YIELD();
    LOG_INFO("Alarm actuator\n");
    if (etimer_expired(&actuator_timer))
    {
      LOG_INFO("Timer expired\n");
      // if all the queues have 24 elements, make a prediction

      if (rotation_queue.is_full && voltage_queue.is_full && pressure_queue.is_full && vibration_queue.is_full)
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
      etimer_reset(&actuator_timer);
    }
    if (etimer_expired(&check_timer))
    {
      LOG_INFO("Check timer expired\n");
      // check if the sensors are still active
      check_sensor(&obs_rotation_ep, "/rotation/status");
      check_sensor(&obs_voltage_ep, "/voltage/status");
      check_sensor(&obs_pressure_ep, "/pressure/status");
      check_sensor(&obs_vibration_ep, "/vibration/status");

      if (rotation_status != 1)
      {
        LOG_INFO("Rotation sensor is not active\n");
      
      }

      if (voltage_status != 1)
      {
        LOG_INFO("Voltage sensor is not active\n");
      }

      if (pressure_status != 1)
      {
        LOG_INFO("Pressure sensor is not active\n");
      }

      if (vibration_status != 1)
      {
        LOG_INFO("Vibration sensor is not active\n");
      }

      // reset the timer
      etimer_reset(&check_timer);
    }
  }

  freeQueue(&rotation_queue);
  freeQueue(&voltage_queue);
  freeQueue(&pressure_queue);
  freeQueue(&vibration_queue);
  PROCESS_END();
}
