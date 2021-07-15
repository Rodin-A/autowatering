/* 
* class_Menu.h
*
* Created: 17.03.2020 14:41:19
* Author: Rodin
*/


#ifndef __CLASS_MENU_H__
#define __CLASS_MENU_H__

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "class_HD44780.h"

#define  MENU_ROWS 2 // Кол-во строк
#define  MENU_COLS 16 // Кол-во столбцов
#define  MENU_CURSOR 126 // Курсор

#define MenuCount(items) sizeof(items)/sizeof(MenuItem)

class MenuItem
{
//variables
public:
	MenuItem *SubItems;
	MenuItem *Parent;
protected:
private:
	const char *_name;
	uint8_t _id;
	uint8_t _s_count;
		
//functions
public:
	MenuItem(const char *Name, uint8_t Id);
	MenuItem(const char *Name, uint8_t Id, MenuItem *SubItems, const uint8_t len);
	void GetName(char *retval);
	const uint8_t id(void);
	const uint8_t s_count(void);
protected:
private:
}; //MenuItem

class Menu
{
//variables
public:
private:
	static uint8_t _count; // count of cur_item
	static uint8_t _index; // index of cur_item
	static uint8_t _s_count; // count of top items
	static MenuItem *_items;
	static MenuItem *_cur_item;
	static uint8_t _pId;
	static void (*_callback)(uint8_t);
//functions
public:
	static void InitMenu(MenuItem *Items, void (*callback)(uint8_t), const uint8_t len);
	static void ShowMain(void);
	static void goDown(void);
	static void goUp(void);
	static void goBack(void);
	static void Select(void);
	static void draw(void);
	static uint8_t getPreviousId(void);
}; //Menu

#endif //__CLASS_MENU_H__
