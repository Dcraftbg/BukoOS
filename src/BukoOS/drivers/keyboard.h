#pragma once
#include <libs/memory.h>
#include <libs/display.h>
#include <libs/terminal.h>
#include <libs/gdt.h>
#include <libs/idt.h>
#include <libs/portlib.h>
#include <libs/p2keyboard.h>
#include <libs/stdkeyboard.h>
#include <libs/memory.h>
#include <libs/string.h>
#include <config.h>
void keyboard_handler(BukoKeyboardAction actionType, int key); 
extern "C" void _driver_ps2_keyboard();
extern "C" void _base_driver_ps2_keyboard();

