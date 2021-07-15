/*
 * Created: 24.06.2019 10:23:54
 * Author : Rodin
 */ 

#define F_CPU 4000000UL
#define uchar unsigned char
#define false 0
#define true !false
#define BitIsSet(reg, bit)  ((reg & (1<<(bit))) != 0)
#define ClearBit(reg, bit)	(reg) &= (~(1<<(bit)))
#define SetBit(reg, bit)	(reg) |= (1<<(bit))

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "class_RTOS.h"
#include "class_Encoder.h"
#include "class_Menu.h"
#include "class_HD44780.h"
#include "class_TWI.h"
#include "ui.h"

#define RTC		0xd0

#define CTRL_FILL 7
#define CTRL_DDR DDRD
#define CTRL_PORT PORTD
#define WATER_SENSOR 6
#define RAIN_SENSOR 7
#define SENSOR_PIN PINB

#define CountOfZones 4
#define EmergencyStop 45

UI::DATE_TIME_STRUCT CurDT;
UI::FILL_STRUCT FILL;
UI::ZONE_STRUCT ZONE[CountOfZones];
uint8_t ZoneCurDelay[CountOfZones];
volatile UI::FLAGS_STRUCT flags;
static struct {
	uint8_t ZONE;
	uint8_t DOWBit;
} CurZone;
uint8_t zoneDOWSetter;
char StartDate[16];

UI::FILL_STRUCT EEMEM sFILL = { 0, 3, 0, 10 }; // Enabled, H, M, Delay
UI::ZONE_STRUCT EEMEM sZONE[CountOfZones] = {  // Enabled, RainDepend, H, M, Delay, DoW Byte
	{ 0, 1, 21, 0, 10, 0b01111111 },
	{ 0, 1, 21, 0, 10, 0b01111111 },
	{ 0, 1, 21, 0, 10, 0b01111111 },
	{ 0, 1, 21, 0, 10, 0b01111111 }
};

void watchdog(void)
{
	RTOS::SetTimerTask(watchdog, 500);
	wdt_reset();
}

void FILL_STOP(void)
{
	ClearBit(CTRL_PORT, CTRL_FILL);
	flags.FILLING = false;
}

void FILL_EMERGENCY_TIMER(void)
{
	static uint8_t timer;
	if (!flags.FILLING) {
		timer = 0;
		return;
	}
	RTOS::SetTimerTask(FILL_EMERGENCY_TIMER, 60000);
	timer++;
	if (timer > EmergencyStop) {
		RTOS::SetTask(FILL_STOP);	
	}
}

void FILL_TIMER(void)
{
	static uint8_t timer;
	if (!flags.FILLING) {
		timer = 0;
		return;
	}
	RTOS::SetTimerTask(FILL_TIMER, 1000);
	if (!flags.FULL_TANK) return;
	timer++;
	if (timer > FILL.Delay) {
		RTOS::SetTask(FILL_STOP);
	}
}

void FILL_START(void)
{
	if ( flags.FULL_TANK || flags.FILLING ) return;
	
	flags.FILLING = true;
	SetBit(CTRL_PORT,CTRL_FILL);
	RTOS::SetTask(FILL_TIMER);
	RTOS::SetTask(FILL_EMERGENCY_TIMER);
}

void Zone_Close(void)
{
	ClearBit(CTRL_PORT, CurZone.ZONE);
}

void Zone_Timer(void)
{
	uint8_t cz;
	for(uint8_t i=0;i<CountOfZones;i++) {
		if(ZoneCurDelay[i] > 0) {
			ZoneCurDelay[i]--;
			RTOS::SetTimerTask(Zone_Timer, 60000);
		} else {
			cz = CurZone.ZONE;
			CurZone.ZONE = i;
			Zone_Close();
			CurZone.ZONE = cz;
		}
	}
}

void Zone_Open(void)
{
	SetBit(CTRL_PORT, CurZone.ZONE);
}

uchar RTC_ConvertFrom(uchar c)
{
	uchar ch = ((c>>4)*10+(0b00001111&c));
	return ch;
}

uchar RTC_ConvertTo(uchar c)
{
	uchar ch = ((c/10)<<4)|(c%10);
	return ch;
}

void SetDT(void)
{
	TWI::start(RTC);
	TWI::write(0x00);
	TWI::write( RTC_ConvertTo( 0 ) );				// SEC
	TWI::write( RTC_ConvertTo( CurDT.Time.M ) );	// MIN
	TWI::write( RTC_ConvertTo( CurDT.Time.H ) );	// HOUR
	TWI::write( RTC_ConvertTo( CurDT.DOW ) );		// WEEKDAY
	TWI::write( RTC_ConvertTo( CurDT.Date.Day ) );	// DAYS
	TWI::write( RTC_ConvertTo( CurDT.Date.Month ) );// MONTH
	TWI::write( RTC_ConvertTo( CurDT.Date.Year ) ); // YEAR
	TWI::stop();
}

void SetDefaultDT(void)
{
	TWI::start(RTC);
	TWI::write(0x00);
	for (uint8_t i=0;i<7;++i)
	{
		TWI::write( RTC_ConvertTo( i ) );
	}
	TWI::stop();
}

void DisplayMenu(void)
{
	UI::DisplayState = UI::menu;
	Menu::draw();
}

void SetDateDone(void)
{
	SetDT();
	RTOS::SetTask(DisplayMenu);
}

void SetTimeDone(void)
{
	SetDT();
	RTOS::SetTask(DisplayMenu);
}

void SaveFILL(void)
{
	eeprom_write_block( (const void*)&FILL, (void*)&sFILL, sizeof(FILL) );
}

void SetFillEnableDone(void)
{
	RTOS::SetTask(SaveFILL);
	RTOS::SetTask(DisplayMenu);
}

void SetFillTimeDone(void)
{
	RTOS::SetTask(SaveFILL);
	RTOS::SetTask(DisplayMenu);
}

void SetFillDelayDone(void)
{
	RTOS::SetTask(SaveFILL);
	RTOS::SetTask(DisplayMenu);
}

void SaveZONE(void)
{
	eeprom_write_block((const void*)&ZONE[CurZone.ZONE], (void*)&sZONE[CurZone.ZONE], sizeof(ZONE));	
}

void SetZoneEnabledDone(void)
{
	SaveZONE();
	RTOS::SetTask(DisplayMenu);
}

void SetZoneStartTimeDone(void)
{
	SaveZONE();
	RTOS::SetTask(DisplayMenu);	
}

void SetZoneDelayDone(void)
{
	SaveZONE();
	RTOS::SetTask(DisplayMenu);
}

void SetZoneRainDependDone(void)
{
	SaveZONE();
	RTOS::SetTask(DisplayMenu);
}

void SetZoneDOWDone(void)
{
	if (zoneDOWSetter) {
		SetBit(ZONE[CurZone.ZONE].DOW,CurZone.DOWBit);
	} else {
		ClearBit(ZONE[CurZone.ZONE].DOW,CurZone.DOWBit);
	}
	SaveZONE();
	RTOS::SetTask(DisplayMenu);
}

void ManualZoneOpen(void)
{
	Zone_Open();	
	RTOS::SetTask(DisplayMenu);
}

void ManualZoneClose(void)
{
	Zone_Close();
	RTOS::SetTask(DisplayMenu);
}

void GetTime(void)
{
	uchar buf;
	TWI::start(RTC);
	buf = TWI::write(0x00);
	TWI::stop();
	TWI::start(RTC|READ);
	// „итаем данные и преобразуем из BCD в двоичную систему
	buf = TWI::read(READ_NOEND);			// sec
	buf = TWI::read(READ_NOEND);			// min
	CurDT.Time.M = RTC_ConvertFrom(buf);
	buf = TWI::read(READ_NOEND);			// hour
	CurDT.Time.H = RTC_ConvertFrom(buf);
	buf = TWI::read(READ_NOEND);			// workday
	CurDT.DOW = RTC_ConvertFrom(buf);
	buf = TWI::read(READ_NOEND);			// day
	CurDT.Date.Day = RTC_ConvertFrom(buf);
	buf = TWI::read(READ_NOEND);			// month
	CurDT.Date.Month = RTC_ConvertFrom(buf);
	buf = TWI::read(READ_END);				// year
	CurDT.Date.Year = RTC_ConvertFrom(buf);
	TWI::stop();
}

void CheckKeys(void)
{
	encoder::check();
	
	encoder::enc_state est = encoder::state;
	if (est.but_pressed) RTOS::SetTask(UI::EncBUT);
	switch (est.rotation)
	{
		case encoder::up:
		{
			RTOS::SetTask(UI::EncUP);
			break;
		}
		case encoder::down:
		{
			RTOS::SetTask(UI::EncDown);
			break;
		}
		default:
		break;
	}
	RTOS::SetTimerTask(CheckKeys,1);
	
}

void MinutePassed(void);
void TICK(void)
{
	RTOS::SetTimerTask(TICK, 1000);
	if ( ( UI::DisplayState != UI::get_time ) && ( UI::DisplayState != UI::get_date ) ) {
		RTOS::SetTask(GetTime);
		RTOS::SetTask(MinutePassed);
	}
}

void Show_StartDate(void)
{
	UI::DisplayState = UI::show_str;
	lcd::clear();
	lcd::home();
	lcd::str(StartDate);
}

void menuCallback(uint8_t id)
{
	switch(id) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7: // Zone DOW
		{
			CurZone.ZONE = Menu::getPreviousId() - 1;
			CurZone.DOWBit = id - 1;
			zoneDOWSetter = BitIsSet(ZONE[CurZone.ZONE].DOW, CurZone.DOWBit);
			UI::SetInt(&zoneDOWSetter, 0, 1, SetZoneDOWDone);
			break;
		}
		case 8: // Zone Start Time
		{
			CurZone.ZONE = Menu::getPreviousId() - 1;
			UI::SetTime(&ZONE[CurZone.ZONE].Start, SetZoneStartTimeDone);
			break;
		}
		case 9: // Zone Delay
		{
			CurZone.ZONE = Menu::getPreviousId() - 1;
			UI::SetInt(&ZONE[CurZone.ZONE].Delay, 1, 60, SetZoneDelayDone);
			break;			
		}
		case 10: // Zone Enabled
		{
			CurZone.ZONE = Menu::getPreviousId() - 1;
			UI::SetInt(&ZONE[CurZone.ZONE].Enabled, 0, 1, SetZoneEnabledDone);
			break;
		}
		case 11: // Zone Rain depend
		{
			CurZone.ZONE = Menu::getPreviousId() - 1;
			UI::SetInt(&ZONE[CurZone.ZONE].RainDepend, 0, 1, SetZoneRainDependDone);
			break;
		}
		case 21: // Manual Fill start
		{
			RTOS::SetTask(FILL_START);
			break;	
		}
		case 22: // Manual Fill stop
		{
			RTOS::SetTask(FILL_STOP);
			break;
		}
		case 23: // Manual Zone open
		{
			UI::SetInt(&CurZone.ZONE, 0, CountOfZones - 1, ManualZoneOpen);
			break;
		}
		case 24: // Manual Zone close
		{
			UI::SetInt(&CurZone.ZONE, 0, CountOfZones - 1, ManualZoneClose);
			break;
		}
		case 99: // Start date
		{
			RTOS::SetTask(Show_StartDate);
			break;
		}
		case 100: // Exit menu
		{
			RTOS::SetTask(UI::ShowDesktop);
			break;
		}
		case 111: // AutoFILL Enabled
		{
			UI::SetInt(&FILL.Enabled, 0, 1, SetFillEnableDone);
			break;			
		}
		case 112: // AutoFILL Set time
		{
			UI::SetTime(&FILL.Time, SetFillTimeDone);
			break;
		}
		case 113: // AutoFILL Delay
		{
			UI::SetInt(&FILL.Delay, 0, 240, SetFillDelayDone);
			break;
		}			
		case 201: // Set Date
		{
			UI::SetDate(&CurDT.Date, SetDateDone);
			break;
		}
		case 202: // Set Time
		{
			UI::SetTime(&CurDT.Time, SetTimeDone);
			break;
		}
		case 221:
		case 222:
		case 223:
		case 224:
		case 225:
		case 226:
		case 227: // Set DoW
		{
			CurDT.DOW = id - 220;
			SetDT();
			Menu::goBack();
			break;
		}
	}
}

void Init_Menu(void)
{
	static MenuItem AF[] = {
		MenuItem( PSTR("Enabled"), 111 ),
		MenuItem( PSTR("Time"), 112 ),
		MenuItem( PSTR("Delay (Sec)"), 113 ),
		MenuItem( PSTR("<<< Back"), 0 )
	};

	static MenuItem WD[] = {
		MenuItem( PSTR("Monday"), 221 ),
		MenuItem( PSTR("Tuesday"), 222 ),
		MenuItem( PSTR("Wednesday"), 223 ),
		MenuItem( PSTR("Thursday"), 224 ),
		MenuItem( PSTR("Friday"), 225 ),
		MenuItem( PSTR("Saturday"), 226 ),
		MenuItem( PSTR("Sunday"), 227 ),
		MenuItem( PSTR("<<< Back"), 0 )
	};

	static MenuItem DT[] = {
		MenuItem( PSTR("Set Date"), 201 ),
		MenuItem( PSTR("Set Time"), 202 ),
		MenuItem( PSTR("Day of week"), 0, WD, MenuCount( WD ) ),
		MenuItem( PSTR("<<< Back"), 0 )
	};
	
	static MenuItem ZONE[] = {
		MenuItem( PSTR("Enabled"), 10 ),
		MenuItem( PSTR("Start Time"), 8 ),	
		MenuItem( PSTR("Delay (Min)"), 9 ),
		MenuItem( PSTR("Rain depend"), 11 ),
		MenuItem( PSTR("Monday"), 1 ),
		MenuItem( PSTR("Tuesday"), 2 ),
		MenuItem( PSTR("Wednesday"), 3 ),
		MenuItem( PSTR("Thursday"), 4 ),
		MenuItem( PSTR("Friday"), 5 ),
		MenuItem( PSTR("Saturday"), 6 ),
		MenuItem( PSTR("Sunday"), 7 ),
		MenuItem( PSTR("<<< Back"), 0 )
	};

	static MenuItem ZZ[] = {
		MenuItem( PSTR("Zone 0"), 1, ZONE, MenuCount( ZONE ) ),
		MenuItem( PSTR("Zone 1"), 2, ZONE, MenuCount( ZONE ) ),
		MenuItem( PSTR("Zone 2"), 3, ZONE, MenuCount( ZONE ) ),
		MenuItem( PSTR("Zone 3"), 4, ZONE, MenuCount( ZONE ) ),
		MenuItem( PSTR("<<< Back"), 0 )
	};

	static MenuItem MA[] = {
		MenuItem( PSTR("Fill start"), 21 ),
		MenuItem( PSTR("Fill stop"), 22 ),
		MenuItem( PSTR("Zone[n] Open"), 23 ),
		MenuItem( PSTR("Zone[n] Close"), 24 ),
		MenuItem( PSTR("<<< Back"), 0 )
	};

	static MenuItem MM[] = {
		MenuItem( PSTR("Manual control"), 0, MA, MenuCount(MA) ),
		MenuItem( PSTR("Zones setup"), 0, ZZ, MenuCount(ZZ) ),
		MenuItem( PSTR("Auto fill setup"), 0, AF, MenuCount( AF ) ),
		MenuItem( PSTR("Date & Time"), 0, DT, MenuCount( DT ) ),
		MenuItem( PSTR("Start date"), 99 ),
		MenuItem( PSTR("Exit"), 100 )
	};

	Menu::InitMenu( MM, menuCallback, MenuCount( MM ) );
}

void CheckZoneStart(void)
{
	uint8_t cz;
		
	if (UI::DisplayState == UI::get_time) return;

	for(uint8_t i=0;i<CountOfZones;i++) {
		if ( !ZONE[i].Enabled ) continue;
		if ( ZONE[i].RainDepend ) {
			if ( flags.WAS_RAIN ) continue;
		}
		if ( !BitIsSet(ZONE[i].DOW, CurDT.DOW) ) continue;
		if( (ZONE[i].Start.H == CurDT.Time.H) && (ZONE[i].Start.M == CurDT.Time.M) ) {
			cz = CurZone.ZONE;
			CurZone.ZONE = i;
			ZoneCurDelay[i] = ZONE[i].Delay;
			Zone_Open();
			CurZone.ZONE = cz;
			RTOS::SetTask(Zone_Timer);
		}
	}
	
}

void CheckFillTime(void)
{
	if (!FILL.Enabled) return;
	
	if ((CurDT.Time.H == FILL.Time.H) && (CurDT.Time.M == FILL.Time.M)) {
		RTOS::SetTask(FILL_START);	
	}
}

void MinutePassed(void)
{
	static uint8_t LastMin;
	if ( LastMin != CurDT.Time.M ) {
		LastMin = CurDT.Time.M;
		RTOS::SetTask(CheckFillTime);
		RTOS::SetTask(CheckZoneStart);
	}
}

void Check_Rain(void)
{
	RTOS::SetTimerTask(Check_Rain,60000);
	flags.WAS_RAIN = BitIsSet(SENSOR_PIN, RAIN_SENSOR);
}

void Check_Water_Level(void)
{
	RTOS::SetTimerTask(Check_Water_Level,300);
	if (BitIsSet(SENSOR_PIN,WATER_SENSOR)) {
		flags.FULL_TANK = true;
	} else {
		flags.FULL_TANK = false;
	}
}

void Set_StartDate(void)
{
	sprintf_P(StartDate, PSTR("%02u.%02u.%02u  %02u:%02u"), CurDT.Date.Day, CurDT.Date.Month, CurDT.Date.Year, CurDT.Time.H, CurDT.Time.M);
}

int main(void)
{
	eeprom_read_block((void*)&FILL, (const void*)&sFILL, sizeof(FILL));
	for (uint8_t i=0;i<CountOfZones;i++) {
		eeprom_read_block((void*)&ZONE[i],(const void*)&sZONE[i], sizeof(ZONE));
		CTRL_DDR |= (1<<i);
	}
	
	CTRL_DDR |= (1<<CTRL_FILL);
	
    lcd::init();
	TWI::Init();
	encoder::init();
	//SetDefaultDT();
	RTOS::Init();
	Init_Menu();
	RTOS::SetTask(TICK);
	RTOS::SetTask(CheckKeys);
	RTOS::SetTask(Check_Water_Level);
	RTOS::SetTask(watchdog);
	RTOS::SetTimerTask(Check_Rain,50);
	RTOS::SetTimerTask(UI::ShowDesktop,100);
	RTOS::SetTimerTask(Set_StartDate, 300);
	RTOS::Run();
	wdt_enable(WDTO_1S);
	
	while (1)
    {
		RTOS::TaskManager();
    }
}
