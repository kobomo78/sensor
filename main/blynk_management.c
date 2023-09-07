

#include "esp_log.h"
#include "blynk_management.h"


static const char *TAG = "blynk_management";

enum {
	VP_COUNTER = 0,
	VP_TEMPERATURE,
	VP_HUMIDITY,
	VP_VERSION,
};

extern uint32_t   counter;
extern float Temperature;
extern float Humidity;
extern char  Ver[16];

/* Blynk client state handler */
void state_handler(blynk_client_t *c, const blynk_state_evt_t *ev, void *data) {
	ESP_LOGI(TAG, "state: %d\n", ev->state);
}

/* Virtual write handler */
void vw_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {


	printf("vw_handler\n");
/*
	if (argc > 1 && atoi(argv[0]) == VP_PWM) {
		uint32_t value = atoi(argv[1]);



	}
*/
}

/* Blynk client state handler */
/* Virtual read handler */
void vr_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data) {

	if (!argc) {
		return;
	}

	int pin = atoi(argv[0]);


	//printf("vr_handler pin=%d\n",pin);

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
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sii", "vw", VP_COUNTER, counter);
			break;
		}
		case VP_VERSION:
		{
			blynk_send(c, BLYNK_CMD_HARDWARE, 0, "sis", "vw", VP_VERSION, Ver);
			break;
		}


	}
}


void BlynkInit(void)
{

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
