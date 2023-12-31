/*
 * blynk_management.h
 *
 *  Created on: 24 авг. 2023 г.
 *      Author: boyarkin.k
 */

#ifndef INCLUDE_BLYNK_MANAGEMENT_H_
#define INCLUDE_BLYNK_MANAGEMENT_H_

#include "blynk.h"

#define BLYNK_TOKEN_1 "4z1rZ4LauY2lGRIaaAdSaIWBKqXiKknH"
#define BLYNK_TOKEN_2 "TaxTSm7kKkvdvMJxer846KiFRTMbFTHP"
#define BLYNK_TOKEN_3 "nSSjtEfSrm6uqpBLmbX9dALYz2LrLxJ6"
#define BLYNK_TOKEN_4 "6yw2tOZkh1aHZxRKbIIuRHh0Zyjbr3S6"
#define BLYNK_TOKEN_5 "555"

#define BLYNK_SERVER "109.194.141.27"

#ifdef __cplusplus
extern "C" {
#endif

void BlynkInit(void);
void state_handler(blynk_client_t *c, const blynk_state_evt_t *ev, void *data);
void vw_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data);
void vr_handler(blynk_client_t *c, uint16_t id, const char *cmd, int argc, char **argv, void *data);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BLYNK_MANAGEMENT_H_ */
