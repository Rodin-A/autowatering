#ifndef __CLASS_ENCODER_H__
#define __CLASS_ENCODER_H__

#include <avr/io.h>
#include <stdint.h>

/* ==== Pin configuration ==== */
#define ENC_PORT   PORTC
#define ENC_DDR    DDRC
#define ENC_PINR   PINC

#define ENC_A      PC0
#define ENC_B      PC1
#define ENC_BTN    PC2

#define BUTTON_THRESHOLD 10

class Encoder
{
public:

    enum Rotation
    {
        None = 0,
        Up,
        Down
    };

    struct State
    {
        uint8_t rotation;      // Rotation
        uint8_t button;        // 1 = pressed event
    };

    static void Init(void);
    static void Check(void);

    static State state;

private:

    static uint8_t prev_state;
    static int8_t step_count;
    static uint8_t button_counter;

    static uint8_t ReadAB(void);
};

#endif