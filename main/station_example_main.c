/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "wifi.h"
#include "DHT22.h"
#include "blynk.h"

#define BLYNK_TOKEN "4z1rZ4LauY2lGRIaaAdSaIWBKqXiKknH"
#define BLYNK_SERVER "109.194.141.27"





static const char *TAG = "sensor";

uint32_t   counter=0;

enum {
	VP_COUNTER = 0,
	VP_TEMPERATURE,
	VP_HUMIDITY,
};


void Work_counter(void *pvParameter)
{

	while(1) {
		counter++;
		if (counter==10000)
			counter=0;


		vTaskDelay(1000 / portTICK_PERIOD_MS );
	}
}

void DHT_reader_task(void *pvParameter)
{
		setDHTgpio(GPIO_NUM_27);

	while(1) {

		printf("DHT Sensor Readings\n" );
		int ret = readDHT();

		errorHandler(ret);

		printf("Humidity %.2f %%\n", getHumidity());
		printf("Temperature %.2f degC\n\n", getTemperature());

		vTaskDelay(15000 / portTICK_PERIOD_MS );
	}
}


/* Blynk client state handler */
static void state_handler(blynk_client_t *c, const blynk_state_evt_t *ev, void *data) {
	ESP_LOGI(TAG, "state: %d\n", ev->state);
}

/* Virtual write handler */
static void vw_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {


	printf("vw_handler\n");
/*
	if (argc > 1 && atoi(argv[0]) == VP_PWM) {
		uint32_t value = atoi(argv[1]);



	}
*/
}

/* Virtual read handler */
static void vr_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {

	if (!argc) {
		return;
	}

	int pin = atoi(argv[0]);


	printf("vr_handler pin=%d\n",pin);

	switch (pin) {
		case VP_TEMPERATURE:
		{
			/* Get ADC value */
			float value = getTemperature();

			/* Respond with `virtual write' command */
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sif", "vw", VP_TEMPERATURE, value);
			break;
		}

		case VP_HUMIDITY:
		{
			float value = getHumidity();

			/* Respond with `virtual write' command */
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sif", "vw", VP_HUMIDITY, value);
			break;
		}
		case VP_COUNTER:
		{
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sii", "vw", VP_COUNTER, counter);
			break;
		}


	}
}


void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    xTaskCreate(&DHT_reader_task, "DHT_reader_task", 2048, NULL, 5, NULL );
    xTaskCreate(&Work_counter, "Work_counter", 2048, NULL, 5, NULL );

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    WifiInit();


    blynk_client_t *client = malloc(sizeof(blynk_client_t));

    blynk_init(client);

    	blynk_options_t opt = {
    		.token = BLYNK_TOKEN,
    		.server = BLYNK_SERVER,
    		/* Use default timeouts */
    	};

    	blynk_set_options(client, &opt);

    	/* Subscribe to state changes and errors */
    	blynk_set_state_handler(client, state_handler, NULL);

    	/* blynk_set_handler sets hardware (BLYNK_CMD_HARDWARE) command handler */
    	blynk_set_handler(client, "vw", vw_handler, NULL);
    	blynk_set_handler(client, "vr", vr_handler, NULL);

    	/* Start Blynk client task */
    	blynk_start(client);
}
