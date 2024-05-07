/*
 * Nhut.h
 *
 *  Created on: April 18, 2023 //k20
 *
 *	Author: ntien, datk19
 */

#ifndef NHUT_H_
#define NHUT_H_

#ifdef __cplusplus
extern "C"{
#endif

// Place function Viet Nam hand made
#include "stdint.h"
void Split_String_UART(uint8_t data_in[], uint8_t len, uint8_t t1[], uint8_t t2[],
		             uint8_t t3[], uint8_t t4[], uint8_t t5[], uint8_t t6[]);
void Handles_UART_6_motor(uint8_t data_encoder_1_motor[],uint8_t data_encoder_2_motor[],uint8_t data_encoder_3_motor[],
		uint8_t data_encoder_4_motor[],uint8_t data_encoder_5_motor[],uint8_t data_encoder_6_motor[], uint8_t Data_Encoder[]);
#ifdef __cplusplus
}
#endif

#endif /* NHUT_H_ */


