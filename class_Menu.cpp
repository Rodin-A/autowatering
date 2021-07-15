/* 
* class_Menu.cpp
*
* Created: 17.03.2020 14:41:19
* Author: Rodin
*/

#include "class_Menu.h"

uint8_t Menu::_count;
uint8_t Menu::_index;
uint8_t  Menu::_s_count;
MenuItem  *Menu::_items;
MenuItem  *Menu::_cur_item;
uint8_t Menu::_pId;
void (*Menu::_callback)(uint8_t);

// default constructor
MenuItem::MenuItem(const char *Name, uint8_t Id)
{
	this->_name = Name;
	this->_id = Id;
	this->_s_count = 0;
	this->SubItems = NULL;
}
MenuItem::MenuItem(const char *Name, uint8_t Id, MenuItem *SubItems, const uint8_t len)
{
	this->_name = Name;
	this->_id = Id;
	this->_s_count = len;
	this->SubItems = SubItems;
 	for (uint8_t i=0;i<len;i++) {
 		this->SubItems[i].Parent = this;
 	}
}
void MenuItem::GetName(char *retval)
{
	memcpy_P(retval, _name, MENU_COLS);
}
const uint8_t MenuItem::id(void) 
{
	return _id;	
}
const uint8_t MenuItem::s_count(void)
{
	return _s_count;
}

void Menu::InitMenu(MenuItem *Items, void (*callback)(uint8_t), const uint8_t len)
{
	_count = len;
	_s_count = len;
	_index = 0;
	_items = Items;
	_cur_item = Items;
	_callback = callback;
}
void Menu::draw(void)
{
	lcd::clear();
	lcd::home();
	
	char buf[MENU_COLS];
	char cursor[] = { MENU_CURSOR };
	uint8_t pos = _index - _index / MENU_ROWS * MENU_ROWS; // —читаем на какой строке текущий пункт

	for(uint8_t i=0;i<MENU_ROWS;++i) {
		uint8_t index = _index - pos + i;
		if (index == _count) break;
		lcd::setcursor(0,i);
		if (i == pos) lcd::str( cursor );
		_cur_item[index].GetName(buf);
		lcd::str( buf );
	}
}
void Menu::ShowMain(void)
{
	_index = 0;
	_cur_item = _items;
	_count = _s_count; 
	draw();
}
void Menu::goDown(void)
{
	_index++;
	if(_index >= _count) _index = 0;
	draw();
}
void Menu::goUp(void)
{
	if(_index - 1 < 0) {
		_index = _count - 1;
	} else {
		_index--;
	}
	draw();
}
void Menu::goBack(void)
{
	MenuItem *Parrent = _cur_item->Parent->Parent;
			
	if (Parrent != NULL) {
		_count = Parrent->s_count();
		_cur_item = Parrent->SubItems;
		_index = 0;
		draw();	
	} else {
		ShowMain();
	}
}
void Menu::Select(void)
{
	if ( _cur_item[_index].s_count() > 0 ) {
		_count = _cur_item[_index].s_count();
		_pId = _cur_item[_index].id();
		_cur_item = _cur_item[_index].SubItems;
		_index = 0;
		draw();
	} else {
		if ( _cur_item[_index].id() > 0) {
			_callback( _cur_item[_index].id() );
		} else {
			goBack();
		}
	}
}
uint8_t Menu::getPreviousId(void)
{
	return _pId;
}