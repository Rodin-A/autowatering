/* 
* class_Encoder.cpp
*
* Created: 25.03.2020 9:38:21
* Author: Rodin
*/


#include "class_Encoder.h"

encoder::enc_state encoder::state;

void encoder::init(void) 
{
	ENC_PORT |= (1<<ENC_P1) | (1<<ENC_P2) | (1<<ENC_BUT);	// Включаем подтягивающие резисторы
}
void encoder::check(void) {
	static uint8_t comp = 0;
	static uint8_t prev_state = 0;
	static uint8_t up_state = 0;
	static uint8_t down_state = 0;
	state.rotation = none;
    uint8_t next_state = (ENC_PIN) & (ENC_MASK);			// Считываем текущее значение битов
    if (next_state != prev_state) {
	    switch (prev_state) {
		    case 2:
		    {
			    if (next_state == 3) up_state++;
			    if (next_state == 0) down_state++;
			    break;
		    }
		    case 0:
		    {
			    if (next_state == 2) up_state++;
			    if (next_state == 1) down_state++;
			    break;
		    }
		    case 1:
		    {
			    if (next_state == 0) up_state++;
			    if (next_state == 3) down_state++;
			    break;
		    }
		    case 3:
		    {
			    if (next_state == 1) up_state++;
			    if (next_state == 2) down_state++;
			    break;
		    }
		    default:
		    {
			    break;
		    }
	    }
	    prev_state = next_state;    // Текущее состояние становится предыдущим
		
		if (up_state >= 4) state.rotation = up;
		if (down_state >= 4) state.rotation = down;
		if (state.rotation != none) {
			down_state = 0;
			up_state = 0;
		}
    }
	
	state.but_pressed = 0;
	// Проверяем нажата ли кнопка
	if ( (ENC_PIN & (1<<(ENC_BUT))) == 0 ) {
		
		//и если кнопка удерживается долго
		if (comp == THRESHOLD) {
			comp = THRESHOLD + 10;
			state.but_pressed = 1;
			return;
		}
		else if (comp < (THRESHOLD+5)) {
			comp++;
		}
	}
	else {
		comp = 0;
	}	
}