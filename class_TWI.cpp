/*
 * twi.c
 *
 * Created: 14.02.2019 15:48:03
 *  Author: Rodin
 */ 
#include "class_TWI.h"

//Инициализация twi
void TWI::Init(void)
{
	TWSR = 0; //Очищаются биты TWPS0 и TWPS1
	TWBR = (unsigned char)( F_CPU / SCL_F - 16 ) / 2;
}

// Запись по twi
unsigned char TWI::write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT))) {};
	// check value of TWI Status Register. Mask prescaler bits.

	return ((TWSR & 0xF8) != MT_ADR_ACK) ? 1 : 0;
}

void TWI::stop(void)
{
	TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

unsigned char TWI::start(unsigned char addr) {
	TWCR=(1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // START
	while (!(TWCR & (1<<TWINT))) {};

	unsigned char twst = (TWSR & 0xF8); // check value of TWI Status Register. Mask prescaler bits.
	if ((twst != START) && (twst != REP_START))
	return 1;
	unsigned char ret = write(addr);
	return ret;
}

// Чтение
unsigned char TWI::read(unsigned char END)
{
	if (END)
	TWCR = (1<<TWINT)|(1<<TWEN);
	else
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);

	while(!(TWCR & (1<<TWINT)));

	return TWDR; // return data
}