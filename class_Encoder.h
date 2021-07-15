/*
* class_Encoder.h
*
* Created: 25.03.2020 9:38:22
* Author: Rodin
*/


#ifndef __CLASS_ENCODER_H__
#define __CLASS_ENCODER_H__

#include <avr/io.h>

#define ENC_PORT PORTC
#define ENC_PIN PINC
#define ENC_P1 0
#define ENC_P2 1
#define ENC_BUT 2

//сколько циклов опроса кнопка должна удерживаться
#define THRESHOLD 20

#define ENC_MASK (1<<ENC_P1) | (1<<ENC_P2)


class encoder
{
//variables
public:
	enum rot_state{ none, up, down };
	static struct enc_state {
		rot_state rotation: 2;
		unsigned but_pressed: 1;
	} state;
protected:
private:
//functions
public:
	static void init(void);
	static void check(void);
protected:
private:


}; //encoder

#endif //__CLASS_ENCODER_H__
