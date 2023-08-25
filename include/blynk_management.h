/*
 * blynk_management.h
 *
 *  Created on: 24 авг. 2023 г.
 *      Author: boyarkin.k
 */

#ifndef INCLUDE_BLYNK_MANAGEMENT_H_
#define INCLUDE_BLYNK_MANAGEMENT_H_

#include "blynk.h"

#define BLYNK_TOKEN "4z1rZ4LauY2lGRIaaAdSaIWBKqXiKknH"
#define BLYNK_SERVER "109.194.141.27"


void BlynkInit(void);
void state_handler(blynk_client_t *c, const blynk_state_evt_t *ev, void *data);
void vw_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data);
void vr_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data);


#endif /* INCLUDE_BLYNK_MANAGEMENT_H_ */
