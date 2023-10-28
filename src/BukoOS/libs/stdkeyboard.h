#pragma once
#define BUKO_KEY_ESCAPE 256 
#define BUKO_KEY_ENTER 257
#define BUKO_KEY_BACKSPACE 259
#define BUKO_KEY_LEFT_SHIFT         340
#define BUKO_KEY_LEFT_CONTROL       341
#define BUKO_KEY_LEFT_ALT           342

#define BUKO_KEY_RIGHT_SHIFT 344

#define BUKO_KEY_SPACE ' '
#define BUKO_KEY_A 'A'
#define BUKO_KEY_B 'B'
#define BUKO_KEY_C 'C'
#define BUKO_KEY_D 'D'
#define BUKO_KEY_E 'E'
#define BUKO_KEY_F 'F'
#define BUKO_KEY_G 'G'
#define BUKO_KEY_H 'H'
#define BUKO_KEY_I 'I'
#define BUKO_KEY_J 'J'
#define BUKO_KEY_K 'K'
#define BUKO_KEY_L 'L'
#define BUKO_KEY_M 'M'
#define BUKO_KEY_N 'N'
#define BUKO_KEY_O 'O'
#define BUKO_KEY_P 'P'
#define BUKO_KEY_Q 'Q'
#define BUKO_KEY_R 'R'
#define BUKO_KEY_S 'S'
#define BUKO_KEY_T 'T'
#define BUKO_KEY_U 'U'
#define BUKO_KEY_V 'V'
#define BUKO_KEY_W 'W'
#define BUKO_KEY_X 'X'
#define BUKO_KEY_Y 'Y'
#define BUKO_KEY_Z 'Z'
#define BUKO_KEY_1 '1'
#define BUKO_KEY_2 '2'
#define BUKO_KEY_3 '3'
#define BUKO_KEY_4 '4'
#define BUKO_KEY_5 '5'
#define BUKO_KEY_6 '6'
#define BUKO_KEY_7 '7'
#define BUKO_KEY_8 '8'
#define BUKO_KEY_9 '9'
#define BUKO_KEY_0 '0'
#define BUKO_KEY_DOT '.'
#define BUKO_KEY_COMA ','
#define BUKO_KEY_SINGLE_QUOTE '\''
#define BUKO_KEY_SEMICOLON ';'
#define BUKO_KEY_OPEN_SQUARE '['
#define BUKO_KEY_CLOSE_SQUARE ']'
#define BUKO_KEY_EQUALS '='
#define BUKO_KEY_MINUS '-'
#define BUKO_KEY_BACKSLASH '\\'
#define BUKO_KEY_BACKTICK '`'

typedef enum {
  BUKO_KEYBOARD_ACTION_RELEASE=0,
  BUKO_KEYBOARD_ACTION_PRESS=1,
  BUKO_KEYBOARD_ACTION_HOLD=2,
} BukoKeyboardAction;

