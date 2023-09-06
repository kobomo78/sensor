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
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "wifi.h"
#include "DHT22.h"
#include "ota.h"
#include "blynk_management.h"



extern "C" {void app_main(void);}

static const char *TAG = "sensor";

uint32_t   counter=0;
float Temperature=0;
float Humidity=0;

COtaUpdate  OtaUpdate;

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

		Temperature=getTemperature();
		Humidity=getHumidity();

		printf("Humidity %.2f %%\n", Humidity);
		printf("Temperature %.2f degC\n\n", Temperature);

		vTaskDelay(15000 / portTICK_PERIOD_MS );
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




    WifiInit();
    BlynkInit();

    OtaUpdate.Init();

    esp_ota_mark_app_valid_cancel_rollback();


}
