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
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "wifi.h"
#include "DHT22.h"
#include "ota.h"
#include "blynk_management.h"
#include "Server_Exchange.h"


#define GPIO_OUTPUT_IO_0    GPIO_NUM_13
#define GPIO_OUTPUT_IO_1    GPIO_NUM_12
#define GPIO_OUTPUT_IO_2    GPIO_NUM_14
#define GPIO_OUTPUT_IO_3    GPIO_NUM_26
#define GPIO_OUTPUT_IO_4    GPIO_NUM_25


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1) | (1ULL<<GPIO_OUTPUT_IO_2) | (1ULL<<GPIO_OUTPUT_IO_3) | (1ULL<<GPIO_OUTPUT_IO_4))

extern "C" {void app_main(void);}

static const char *TAG = "sensor";

uint32_t   counter=0;
char WorkTime[64];
float Temperature=0;
float Humidity=0;
char  Ver[16];
uint8_t   addr=0xFF;

COtaUpdate  OtaUpdate;

void Work_counter(void *pvParameter)
{

	while(1) {
		counter++;

		time_t seconds(counter);
		tm *p = gmtime(&seconds);
		snprintf(WorkTime,sizeof(WorkTime),"%d days %d:%d:%d",p->tm_yday,p->tm_hour,p->tm_min,p->tm_sec);

		vTaskDelay(1000 / portTICK_PERIOD_MS );
	}
}

void DHT_reader_task(void *pvParameter)
{
		setDHTgpio(GPIO_NUM_27);

	while(1) {

		int ret = readDHT();

		errorHandler(ret);

		Temperature=getTemperature();
		Humidity=getHumidity();

		vTaskDelay(15000 / portTICK_PERIOD_MS );
	}
}


void Init(void)
{
	   gpio_config_t io_conf;
	   memset((void*)&io_conf,0,sizeof(io_conf));

	   io_conf.mode = GPIO_MODE_INPUT;
	   io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	   io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	   io_conf.pull_up_en = GPIO_PULLUP_ENABLE;


	   gpio_config(&io_conf);


	   if (gpio_get_level(GPIO_OUTPUT_IO_0)==0)
		   addr=0;
	   if (gpio_get_level(GPIO_OUTPUT_IO_1)==0)
		   addr=1;
	   if (gpio_get_level(GPIO_OUTPUT_IO_2)==0)
		   addr=2;
	   if (gpio_get_level(GPIO_OUTPUT_IO_3)==0)
		   addr=3;
	   if (gpio_get_level(GPIO_OUTPUT_IO_4)==0)
		   addr=4;


	   esp_log_level_set("ota", ESP_LOG_NONE);
	   esp_log_level_set("esp_ota_ops", ESP_LOG_NONE);
	   esp_log_level_set("HTTP_CLIENT", ESP_LOG_NONE);
	   esp_log_level_set("esp_https_ota", ESP_LOG_NONE);

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

    ESP_ERROR_CHECK(esp_netif_init());



    xTaskCreate(&DHT_reader_task, "DHT_reader_task", 2048, NULL, 5, NULL );
    xTaskCreate(&Work_counter, "Work_counter", 2048, NULL, 5, NULL );
    xTaskCreate(&Socket_Recv, "Socket_Recv", 4096, NULL, 5, NULL );



    Init();
    WifiInit();
    BlynkInit();

    OtaUpdate.Init();

    esp_ota_mark_app_valid_cancel_rollback();


}
