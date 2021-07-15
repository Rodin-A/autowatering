/*
 * ui.cpp
 *
 * Created: 16.01.2021 0:40:24
 *  Author: Rodin
 */ 

#include "ui.h"

extern UI::DATE_TIME_STRUCT CurDT;
extern UI::FLAGS_STRUCT flags;
UI::DS UI::DisplayState;
uint8_t UI::blink;
uint8_t *UI::set_Ptr;
RTOS::TPTR UI::set_CallBack;
UI::TIME_STRUCT *UI::set_time;
UI::DATE_STRUCT *UI::set_date;
uint8_t UI::setIntMin;
uint8_t UI::setIntMax;

void UI::DisplayTime(void) {
	RTOS::SetTimerTask(DisplayTime,500);
	if (UI::DisplayState != UI::none) return;
	char data[16];
	static const char DOW[][4] = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
	lcd::home();
	sprintf_P(data, PSTR("%s %02u.%02u  %02u:%02u"), DOW[CurDT.DOW - 1], CurDT.Date.Day, CurDT.Date.Month, CurDT.Time.H, CurDT.Time.M);
	if (UI::blink) {
		data[13] = 32;
	} else {
		lcd::clear();
	}
	
	lcd::str(data);
	
	memset(data,0,sizeof(data)/sizeof(char));
	sprintf_P(data, PSTR("F R"));
	
	
	if (flags.FILLING) {
		if (UI::blink) data[0] = 32;
	} else {
		if (!flags.FULL_TANK) {
			data[0] = 32;
		}
	}
	
	if (!flags.WAS_RAIN) {
		data[2] = 32;
	}

	lcd::setcursor(0,1);
	lcd::str(data);
	
	UI::blink = !UI::blink;
	
// 	if (!UI::blink) return;
// 	static uint8_t i = 0;
// 	i++;
// 	data[0]=i;
// 	data[1]=0;
// 	lcd::home();
// 	lcd::str(data);
// 	lcd::setcursor(0,1);
// 	sprintf(data,"%i", i);
// 	lcd::str(data);
}

void UI::ShowMainMenu(void) {
	DisplayState = menu;
	Menu::ShowMain();
}

void UI::ShowDesktop(void) {
	DisplayState = none;
	lcd::clear();
	RTOS::SetTask(DisplayTime);
}

// SET DATE FUNCTIONS
void UI::SetDate_Blink(void) {
	if (DisplayState != get_date) return;

	RTOS::SetTimerTask(SetDate_Blink,400);
	
	char data[8];
	lcd::setcursor(4,0);
	sprintf_P(data, PSTR("%02u.%02u.%02u"), set_date->Day, set_date->Month, set_date->Year);
	lcd::str(data);
	
	if (blink) {
		if (set_Ptr == &set_date->Day) {
			lcd::setcursor(4,0);
		} else if (set_Ptr == &set_date->Month) {
			lcd::setcursor(7,0);
		} else if (set_Ptr == &set_date->Year) {
			lcd::setcursor(10,0);
		}
		lcd::str("  ");
	}
	
	blink = !blink;
}

void UI::SetDateDown(void) {
	(*set_Ptr)++;
	if (set_Ptr == &set_date->Day) {
		if (set_date->Day > 31) set_date->Day = 1;
	} else if (set_Ptr == &set_date->Month) {
		if (set_date->Month > 12) set_date->Month = 1;
	} else if (set_Ptr == &set_date->Year) {
		if (set_date->Year > 99) set_date->Year = 0;
	}
}

void UI::SetDateUP(void) {
	(*set_Ptr)--;
	if (set_Ptr == &set_date->Day) {
		if ((set_date->Day > 31) || (!set_date->Day)) set_date->Day = 31;
	} else if (set_Ptr == &set_date->Month) {
		if ((set_date->Month > 12) || (!set_date->Month)) set_date->Month = 12;
	} else if (set_Ptr == &set_date->Year) {
		if (set_date->Year > 99) set_date->Year = 99;
	}
}

void UI::SetDateBUT(void) {
	if (set_Ptr == &set_date->Day) {
		set_Ptr = &set_date->Month;
	} else if (set_Ptr == &set_date->Month) {
		set_Ptr = &set_date->Year;
	} else if (set_Ptr == &set_date->Year) {
		RTOS::SetTask(set_CallBack);
	}
}

void UI::SetDate(DATE_STRUCT *ptr, RTOS::TPTR callBack) {
	DisplayState = get_date;
	set_date = ptr;
	set_CallBack = callBack;
	set_Ptr = &set_date->Day;
	lcd::clear();
	RTOS::SetTask(SetDate_Blink);
}



// SET TIME FUNCTIONS
void UI::SetTime_Blink(void) {
	if (DisplayState != get_time) return;

	RTOS::SetTimerTask(SetTime_Blink,400);

	char data[5];
	lcd::setcursor(6,0);
	sprintf_P(data, PSTR("%02u:%02u"), set_time->H, set_time->M);
	lcd::str(data);
	
	if (blink) {
		if (set_Ptr == &set_time->H) {
			lcd::setcursor(6,0);
		} else if (set_Ptr == &set_time->M) {
			lcd::setcursor(9,0);
		}
		lcd::str("  ");
	}
	
	blink = !blink;
}

void UI::SetTimeDown(void) {
	(*set_Ptr)++;
	if (set_Ptr == &set_time->H) {
		if (set_time->H > 23) set_time->H = 0;
	} else if (set_Ptr == &set_time->M) {
		if (set_time->M > 59) set_time->M = 0;
	}
}

void UI::SetTimeUP(void) {
	(*set_Ptr)--;
	if (set_Ptr == &set_time->H) {
		if (set_time->H > 23) set_time->H = 23;
	} else if (set_Ptr == &set_time->M) {
		if (set_time->M > 59) set_time->M = 59;
	}
}

void UI::SetTimeBUT(void) {
	if (set_Ptr == &set_time->H) {
		set_Ptr = &set_time->M;
	} else if (set_Ptr == &set_time->M) {
		RTOS::SetTask(set_CallBack);
	}
}

void UI::SetTime(TIME_STRUCT *ptr, RTOS::TPTR callBack) {
		DisplayState = get_time;
		set_time = ptr;
		set_CallBack = callBack;
		set_Ptr = &set_time->H;
		lcd::clear();
		RTOS::SetTask(SetTime_Blink);
}



// SET INT FUNCTIONS
void UI::SetInt_Blink(void) {
	if (DisplayState != get_int) return;
	
	RTOS::SetTimerTask(SetInt_Blink,400);

	char data[5];
	lcd::clear();
	lcd::home();
	if (blink) {
		sprintf_P(data, PSTR("%u"), *set_Ptr);
	}
	lcd::str(data);
	blink = !blink;
}

void UI::SetIntDown(void) {
	if (*set_Ptr + 1 > setIntMax) {
		*set_Ptr = setIntMin;
	} else {
		(*set_Ptr)++;
	}
}

void UI::SetIntUp(void) {
	if (*set_Ptr - 1 < setIntMin) {
		*set_Ptr = setIntMax;
	} else {
		(*set_Ptr)--;
	}
}

void UI::SetIntBUT(void) {
	RTOS::SetTask(set_CallBack);
}

void UI::SetInt(uint8_t *ptr, const uint8_t min, const uint8_t max, RTOS::TPTR callBack) {
	DisplayState = get_int;
	set_Ptr = ptr;
	set_CallBack = callBack;
	setIntMin = min;
	setIntMax = max;
	lcd::clear();
	RTOS::SetTask(SetInt_Blink);
}



// ENCODER FUNCTIONS
void UI::EncUP(void) {
	RTOS::SetTimerTask(ShowDesktop,60000);
	switch (UI::DisplayState) {
		case UI::none:
		{
			break;
		}
		case menu:
		{
			Menu::goUp();
			break;
		}
		case get_int:
		{
			RTOS::SetTask(SetIntUp);
			break;
		}
		case get_time:
		{
			RTOS::SetTask(SetTimeUP);
			break;
		}
		case get_date:
		{
			RTOS::SetTask(SetDateUP);
			break;
		}
		default:
		break;
	}
}

void UI::EncDown(void) {
	RTOS::SetTimerTask(ShowDesktop,60000);
	switch (UI::DisplayState) {
		case UI::none:
		{
			break;
		}
		case menu:
		{
			Menu::goDown();
			break;
		}
		case get_int:
		{
			RTOS::SetTask(SetIntDown);
			break;
		}
		case get_time:
		{
			RTOS::SetTask(SetTimeDown);
			break;
		}
		case get_date:
		{
			RTOS::SetTask(SetDateDown);
			break;
		}
		default:
		break;
	}
}

void UI::EncBUT(void) {
	RTOS::SetTimerTask(ShowDesktop,60000);
	switch (UI::DisplayState) {
		case none:
		{
			RTOS::SetTask(ShowMainMenu);
			break;
		}
		case UI::menu:
		{
			Menu::Select();
			break;
		}
		case get_int:
		{
			RTOS::SetTask(SetIntBUT);
			break;
		}
		case UI::get_time:
		{
			RTOS::SetTask(SetTimeBUT);
			break;
		}
		case get_date:
		{
			RTOS::SetTask(SetDateBUT);
			break;
		}
		case show_str:
		{
			RTOS::SetTask(ShowDesktop);
			break;
		}
		default:
		break;
	}
}