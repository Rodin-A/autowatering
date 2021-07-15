/*
 * ui.h
 *
 * Created: 16.01.2021 0:40:09
 * Author: Rodin
 */ 


#ifndef UI_H_
#define UI_H_

#include <stdio.h>
#include <avr/pgmspace.h>
#include "class_RTOS.h"
#include "class_HD44780.h"
#include "class_Menu.h"


class UI 
{
public:
	static enum DS{ none, menu, get_int, get_date, get_time, show_str } DisplayState;
		
	struct FLAGS_STRUCT{
		unsigned FULL_TANK: 1;
		unsigned FILLING: 1;
		unsigned WAS_RAIN: 1;
	};

	struct TIME_STRUCT{
		uint8_t H;
		uint8_t M;
	};

	struct DATE_STRUCT{
		uint8_t Day;
		uint8_t Month;
		uint8_t Year;
	};

	struct DATE_TIME_STRUCT{
		TIME_STRUCT Time;
		DATE_STRUCT Date;
		uint8_t DOW;
	};

	struct FILL_STRUCT{
		uint8_t Enabled;
		TIME_STRUCT Time;
		uint8_t Delay;
	};
	
	struct ZONE_STRUCT{
		uint8_t Enabled;
		uint8_t RainDepend;
		TIME_STRUCT Start;
		uint8_t Delay;
		uint8_t DOW;
	};

	static uint8_t blink;		

protected:
private:
	static uint8_t *set_Ptr;
	static RTOS::TPTR set_CallBack;
	static UI::TIME_STRUCT *set_time;
	static UI::DATE_STRUCT *set_date;
	static uint8_t setIntMin;
	static uint8_t setIntMax;

public:

	static void ShowMainMenu(void);
	static void ShowDesktop(void);

	static void SetDateUP(void);
	static void SetDateDown(void);
	static void SetDateBUT(void);
	static void SetDate(DATE_STRUCT *ptr, RTOS::TPTR callBack);
	
	static void SetTimeUP(void);
	static void SetTimeDown(void);
	static void SetTimeBUT(void);
	static void SetTime(TIME_STRUCT *ptr, RTOS::TPTR callBack);
	
	static void SetIntUp(void);
	static void SetIntDown(void);
	static void SetIntBUT(void);
	static void SetInt(uint8_t *val, const uint8_t min, const uint8_t max, RTOS::TPTR callBack);
	
	static void EncUP(void);
	static void EncDown(void);
	static void EncBUT(void);

private:
	static void DisplayTime(void);
	static void SetDate_Blink(void);
	static void SetTime_Blink(void);
	static void SetInt_Blink(void);
};

#endif /* UI_H_ */