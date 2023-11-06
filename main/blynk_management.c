

#include "esp_log.h"
#include "blynk_management.h"


static const char *TAG = "blynk_management";

enum {
	VP_COUNTER 			= 0,
	VP_TEMPERATURE		= 1,
	VP_HUMIDITY			= 2,
	VP_VERSION			= 3,
	VP_RESTART_CMD		= 4,
	VP_RESTART_REASON	= 5

};

extern char WorkTime[64];
extern float Temperature;
extern float Humidity;
extern char  Ver[16];
extern uint8_t   addr;

char *getesp_reset_reason_str(esp_reset_reason_t reason)
{
	static char reasonStr[12][64]={"Reason not in diapason",
									"Reset reason can not be determined",
									"Reset due to power-on event",
									"Reset by external pin (not applicable for ESP32)",
									"Software reset via esp_restart",
									"Software reset due to exception/panic",
									"Reset (software or hardware) due to interrupt watchdog",
									"Reset due to task watchdog",
									"Reset due to other watchdogs",
									"Reset after exiting deep sleep mode",
									"Brownout reset (software or hardware)",
									"Reset over SDIO"};


	return (reason>=12)? reasonStr[0]:reasonStr[reason+1];

}

/* Blynk client state handler */
void state_handler(blynk_client_t *c, const blynk_state_evt_t *ev, void *data) {
	ESP_LOGI(TAG, "state: %d\n", ev->state);
}

/* Virtual write handler */
void vw_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {

	if (argc<=1) {
		return;
	}

	int pin = atoi(argv[0]);


	switch (pin) {
			case VP_RESTART_CMD:
				{
					if (atoi(argv[1])==1)
						esp_restart();

					break;

				}
	}

}

/* Blynk client state handler */
/* Virtual read handler */
void vr_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {

	if (!argc) {
		return;
	}

	int pin = atoi(argv[0]);


	switch (pin) {
		case VP_TEMPERATURE:
		{
			/* Get ADC value */
			float value = Temperature;

			/* Respond with `virtual write' command */
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sif", "vw", VP_TEMPERATURE, value);
			break;
		}

		case VP_HUMIDITY:
		{
			float value = Humidity;

			/* Respond with `virtual write' command */
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sif", "vw", VP_HUMIDITY, value);
			break;
		}
		case VP_COUNTER:
		{
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sis", "vw", VP_COUNTER, WorkTime);
			break;
		}
		case VP_VERSION:
		{
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sis", "vw", VP_VERSION, Ver);
			break;
		}
		case VP_RESTART_REASON:
		{
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sis", "vw", VP_RESTART_REASON, getesp_reset_reason_str(esp_reset_reason()));
			break;
		}



	}
}


void BlynkInit(void)
{

	blynk_client_t *client = malloc(sizeof(blynk_client_t));
	char tokens[5][64]={BLYNK_TOKEN_1,BLYNK_TOKEN_2,BLYNK_TOKEN_3,BLYNK_TOKEN_4,BLYNK_TOKEN_5};

	    blynk_init(client);

	    	blynk_options_t opt = {
	    		.server = BLYNK_SERVER,
	    		/* Use default timeouts */
	    	};

	    	if (addr>=5)
	    		ESP_LOGE(TAG,"error addr %d",addr);
	    	else
	    		snprintf(opt.token,sizeof(opt.token),"%s",tokens[addr]);


	    	blynk_set_options(client, &opt);

	    	/* Subscribe to state changes and errors */
	    	blynk_set_state_handler(client, state_handler, NULL);

	    	/* blynk_set_handler sets hardware (BLYNK_CMD_HARDWARE) command handler */
	    	blynk_set_handler(client, "vw", vw_handler, NULL);
	    	blynk_set_handler(client, "vr", vr_handler, NULL);

	    	/* Start Blynk client task */
	    	blynk_start(client);
}
