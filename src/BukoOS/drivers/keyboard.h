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
#ifndef BUKO_SERVER_BUILD
extern DisplayInfo display;
#endif
namespace Driver {
   void keyboard(); 
};
