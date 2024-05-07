#include "Nhut.h"
#include "stdio.h"

#define START_1_MOTOR 1
#define START_2_MOTOR 8
#define START_3_MOTOR 15
#define START_4_MOTOR 22
#define START_5_MOTOR 29
#define START_6_MOTOR 38

#define END_1_MOTOR 5
#define END_2_MOTOR 12
#define END_3_MOTOR 19
#define END_4_MOTOR 26
#define END_5_MOTOR 33
#define END_6_MOTOR 40

#define NUM_BYTES_DATA 5

void Split_String_UART(uint8_t data_in[], uint8_t len, uint8_t t1[], uint8_t t2[], uint8_t t3[], uint8_t t4[], uint8_t t5[], uint8_t t6[])
{
	int position_a = 0;
	int position_b = 0;
	int position_c = 0;
	int position_d = 0;
	int position_e = 0;
	int position_f = 0;
	for(int i = 0; i < len; i++)
	{
		switch(data_in[i]){
		case 'a': position_a = i;
		break;
		case 'b': position_b = i;
		break;
		case 'c': position_c = i;
		break;
		case 'd': position_d = i;
		break;
		case 'e': position_e = i;
		break;
		case 'f': position_f = i;
		break;
		}
	}
	for(int i = 0; i < position_a; i++)
		t1[i] = data_in[i];

	for(int i = position_a + 1; i < position_b; i++)
		t2[i - (position_a + 1)] = data_in[i];

	for(int i = position_b + 1; i < position_c; i++)
		t3[i - (position_b + 1)] = data_in[i];

	for(int i = position_c + 1; i < position_d; i++)
		t4[i - (position_c + 1)] = data_in[i];

	for(int i = position_d + 1; i < position_e; i++)
		t5[i - (position_d + 1)] = data_in[i];

	for(int i = position_e + 1; i < position_f; i++)
		t6[i - (position_e + 1)] = data_in[i];
}
void Handles_UART_6_motor(uint8_t data_encoder_1_motor[],uint8_t data_encoder_2_motor[],uint8_t data_encoder_3_motor[],uint8_t data_encoder_4_motor[],uint8_t data_encoder_5_motor[],uint8_t data_encoder_6_motor[], uint8_t Data_Encoder[])
{
	for(int i = START_1_MOTOR; i <= END_1_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_1_motor[END_1_MOTOR - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_1_MOTOR-1] = data_encoder_1_motor[NUM_BYTES_DATA];
	Data_Encoder[END_1_MOTOR+1] = 'a';

	for(int i = START_2_MOTOR; i <= END_2_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_2_motor[END_2_MOTOR - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_2_MOTOR-1] = data_encoder_2_motor[NUM_BYTES_DATA];
	Data_Encoder[END_2_MOTOR+1] = 'b';

	for(int i = START_3_MOTOR; i <= END_3_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_3_motor[END_3_MOTOR - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_3_MOTOR-1] = data_encoder_3_motor[NUM_BYTES_DATA];
	Data_Encoder[END_3_MOTOR+1] = 'c';

	for(int i = START_4_MOTOR; i <= END_4_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_4_motor[END_4_MOTOR - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_4_MOTOR] = data_encoder_4_motor[NUM_BYTES_DATA];
	Data_Encoder[END_4_MOTOR+1] = 'd';

	for(int i = START_5_MOTOR; i <= END_5_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_5_motor[NUM_BYTES_DATA - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_5_MOTOR-1] = data_encoder_5_motor[NUM_BYTES_DATA];
	Data_Encoder[END_5_MOTOR+1] = 'e';

	for(int i = START_6_MOTOR; i <= END_6_MOTOR; i++)
	{
		char tem[12];
		sprintf(&tem[0], "%d", data_encoder_6_motor[END_6_MOTOR - i]);
		Data_Encoder[i] = tem[0];
	}
	Data_Encoder[START_6_MOTOR-1] = data_encoder_6_motor[NUM_BYTES_DATA];
	Data_Encoder[END_6_MOTOR+1] = 'f';

	Data_Encoder[END_6_MOTOR+2] = 's';
}
