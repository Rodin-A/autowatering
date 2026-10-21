
#include "class_Encoder.hpp"

#define ENC_MASK ((1 << ENC_A) | (1 << ENC_B))
#define BTN_MASK (1 << ENC_BTN)

Encoder::State Encoder::state;
uint8_t Encoder::prev_state = 0;
int8_t Encoder::step_count = 0;
uint8_t Encoder::button_counter = 0;

void Encoder::Init(void)
{
	ENC_DDR &= ~(ENC_MASK | BTN_MASK);
	ENC_PORT |= (ENC_MASK | BTN_MASK);

	prev_state = ReadAB();
	step_count = 0;
	button_counter = 0;
}

uint8_t Encoder::ReadAB(void)
{
	uint8_t pins = ENC_PINR;
	return ((pins >> ENC_A) & 1) |	(((pins >> ENC_B) & 1) << 1);
}

void Encoder::Check(void)
{
	state.rotation = None;
	state.button = 0;

	static const int8_t transitions[16] = {
		0,  1, -1,  0,
		-1,  0,  0,  1,
		1,  0,  0, -1,
		0, -1,  1,  0
	};

	uint8_t next = ReadAB();

	if (next != prev_state)
	{
		uint8_t index = (prev_state << 2) | next;
		int8_t delta = transitions[index];

		if (delta == 0)
		step_count = 0;
		else
		step_count += delta;

		prev_state = next;

		if (step_count >= 4)
		{
			state.rotation = Up;
			step_count = 0;
		}
		else if (step_count <= -4)
		{
			state.rotation = Down;
			step_count = 0;
		}
	}

	// Êíîïêà
	if ((ENC_PINR & BTN_MASK) == 0)
	{
		if (button_counter < BUTTON_THRESHOLD) button_counter++;
	}
	else
	{
		if (button_counter == BUTTON_THRESHOLD) state.button = 1;
		button_counter = 0;
	}
}
