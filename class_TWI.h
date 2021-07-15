/*
 * twi.h
 *
 * Created: 14.02.2019 15:44:57
 *  Author: Rodin
 */ 

#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>

#define SCL_F              100000UL  // Скорость, 400кГц - максимум для twi

//General Master status codes
#define START			0x08	//START has been transmitted
#define	REP_START		0x10
//Master Transmitter status codes
#define	MT_ADR_ACK		0x18	//SLA+W has been transmitted and ACK received
#define	MT_ADR_NACK		0x20	//SLA+W has been transmitted and NACK received
#define	MT_DATA_ACK		0x28
#define	MT_DATA_NACK	0x30	//Data byte has been transmitted and NACK received
#define	MT_ARB_LOST		0x38	//Arbitration lost in SLA+W or data bytes

#define WRITE		0x00
#define READ		0x01

#define READ_END	0x01
#define READ_NOEND	0x00

class TWI
{
public:
	//Инициализация twi
	static void Init(void);

	static void stop(void);
	
	static unsigned char start(unsigned char addr);

	// Запись по twi, reg_adr - адрес регистра, data - значение, если не нужно пишем Skip_Flag
	static unsigned char write(unsigned char data);

	// Чтение по twi
	static unsigned char read(unsigned char END);
}; //class_TWI

#endif /* TWI_H_ */