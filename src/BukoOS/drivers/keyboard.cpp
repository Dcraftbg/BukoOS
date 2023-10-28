#include "keyboard.h"
void Driver::keyboard() {
    size_t i=0;
    for(; i<3; ++i) {
        char keyboard = Kernel::inb(0x64);
        if(keyboard&0x01) break;
    }
    if(i==3) return;
    char c = Kernel::inb(0x60);
    int result=0;
    static bool shifted=false;

    BukoKeyboardAction actionType=BUKO_KEYBOARD_ACTION_PRESS;
    if(c!=0) {
       switch(c) {
       case QWERTY_US_CHARCODE_A_PRESS : result='A';                  break;
       case QWERTY_US_CHARCODE_B_PRESS : result='B';                  break;
       case QWERTY_US_CHARCODE_C_PRESS : result='C';                  break;
       case QWERTY_US_CHARCODE_D_PRESS : result='D';                  break;
       case QWERTY_US_CHARCODE_E_PRESS : result='E';                  break;
       case QWERTY_US_CHARCODE_F_PRESS : result='F';                  break;
       case QWERTY_US_CHARCODE_G_PRESS : result='G';                  break;
       case QWERTY_US_CHARCODE_H_PRESS : result='H';                  break;
       case QWERTY_US_CHARCODE_I_PRESS : result='I';                  break;
       case QWERTY_US_CHARCODE_J_PRESS : result='J';                  break;
       case QWERTY_US_CHARCODE_K_PRESS : result='K';                  break;
       case QWERTY_US_CHARCODE_L_PRESS : result='L';                  break;
       case QWERTY_US_CHARCODE_M_PRESS : result='M';                  break;
       case QWERTY_US_CHARCODE_N_PRESS : result='N';                  break;
       case QWERTY_US_CHARCODE_O_PRESS : result='O';                  break;
       case QWERTY_US_CHARCODE_P_PRESS : result='P';                  break;
       case QWERTY_US_CHARCODE_Q_PRESS : result='Q';                  break;
       case QWERTY_US_CHARCODE_R_PRESS : result='R';                  break;
       case QWERTY_US_CHARCODE_S_PRESS : result='S';                  break;
       case QWERTY_US_CHARCODE_T_PRESS : result='T';                  break;
       case QWERTY_US_CHARCODE_U_PRESS : result='U';                  break;
       case QWERTY_US_CHARCODE_V_PRESS : result='V';                  break;
       case QWERTY_US_CHARCODE_W_PRESS : result='W';                  break;
       case QWERTY_US_CHARCODE_X_PRESS : result='X';                  break;
       case QWERTY_US_CHARCODE_Y_PRESS : result='Y';                  break;
       case QWERTY_US_CHARCODE_Z_PRESS : result='Z';                  break;
       case QWERTY_US_CHARCODE_1_PRESS : result='1';                  break; 
       case QWERTY_US_CHARCODE_2_PRESS : result='2';                  break;
       case QWERTY_US_CHARCODE_3_PRESS : result='3';                  break;
       case QWERTY_US_CHARCODE_4_PRESS : result='4';                  break;
       case QWERTY_US_CHARCODE_5_PRESS : result='5';                  break;
       case QWERTY_US_CHARCODE_6_PRESS : result='6';                  break;
       case QWERTY_US_CHARCODE_7_PRESS : result='7';                  break;
       case QWERTY_US_CHARCODE_8_PRESS : result='8';                  break;
       case QWERTY_US_CHARCODE_9_PRESS : result='9';                  break;
       case QWERTY_US_CHARCODE_0_PRESS : result='0';                  break;
       case QWERTY_US_LEFT_SHIFT_PRESS : result=BUKO_KEY_LEFT_SHIFT;  break;
       case QWERTY_US_RIGHT_SHIFT_PRESS: result=BUKO_KEY_RIGHT_SHIFT; break;
       case QWERTY_US_LEFT_CONTR_PRESS : result=BUKO_KEY_LEFT_CONTROL;break;
       case QWERTY_US_BACKTICK_PRESS   : result=BUKO_KEY_BACKTICK;    break;
       case QWERTY_US_ESCAPE_PRESS     : result=BUKO_KEY_ESCAPE;      break;
       case QWERTY_US_ENTER_PRESS      : result=BUKO_KEY_ENTER;       break;
       case QWERTY_US_BACKSPACE_PRESS  : result=BUKO_KEY_BACKSPACE;   break;
       case QWERTY_US_LEFT_ALT_PRESS   : result=BUKO_KEY_LEFT_ALT;    break;
       case QWERTY_US_SPACE_PRESS      : result=BUKO_KEY_SPACE;       break;

       case QWERTY_US_DOT_PRESS        : result=BUKO_KEY_DOT         ; break;
       case QWERTY_US_COMA_PRESS       : result=BUKO_KEY_COMA        ; break;
       case QWERTY_US_SINGLE_QUOTE_PRESS:result=BUKO_KEY_SINGLE_QUOTE; break;
       case QWERTY_US_OPEN_SQUARE_PRESS: result=BUKO_KEY_OPEN_SQUARE ; break;
       case QWERTY_US_CLOSE_SQUARE_PRESS:result=BUKO_KEY_CLOSE_SQUARE; break;
       case QWERTY_US_EQUALS_PRESS     : result=BUKO_KEY_EQUALS      ; break;
       case QWERTY_US_MINUS_PRESS      : result=BUKO_KEY_MINUS       ; break;
       case QWERTY_US_BACKSLASH_PRESS  : result=BUKO_KEY_BACKSLASH   ; break;
       //case QWERTY_US_SEMICOLON_PRESS  : result=BUKO_KEY_SEMICOLON   ; break;
       case QWERTY_US_CHARCODE_A_RELEASE : result='A';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_B_RELEASE : result='B';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_C_RELEASE : result='C';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_D_RELEASE : result='D';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_E_RELEASE : result='E';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_F_RELEASE : result='F';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_G_RELEASE : result='G';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_H_RELEASE : result='H';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_I_RELEASE : result='I';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_J_RELEASE : result='J';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_K_RELEASE : result='K';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_L_RELEASE : result='L';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_M_RELEASE : result='M';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_N_RELEASE : result='N';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_O_RELEASE : result='O';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_P_RELEASE : result='P';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_Q_RELEASE : result='Q';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_R_RELEASE : result='R';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_S_RELEASE : result='S';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_T_RELEASE : result='T';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_U_RELEASE : result='U';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_V_RELEASE : result='V';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_W_RELEASE : result='W';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_X_RELEASE : result='X';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_Y_RELEASE : result='Y';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_Z_RELEASE : result='Z';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_1_RELEASE : result='1';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break; 
       case QWERTY_US_CHARCODE_2_RELEASE : result='2';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_3_RELEASE : result='3';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_4_RELEASE : result='4';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_5_RELEASE : result='5';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_6_RELEASE : result='6';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_7_RELEASE : result='7';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_8_RELEASE : result='8';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_9_RELEASE : result='9';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CHARCODE_0_RELEASE : result='0';             actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_LEFT_SHIFT_RELEASE : result=BUKO_KEY_LEFT_SHIFT;   actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_RIGHT_SHIFT_RELEASE: result=BUKO_KEY_RIGHT_SHIFT;  actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_LEFT_CONTR_RELEASE : result=BUKO_KEY_LEFT_CONTROL; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_BACKTICK_RELEASE   : result=BUKO_KEY_BACKTICK;     actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_ESCAPE_RELEASE     : result=BUKO_KEY_ESCAPE;       actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_ENTER_RELEASE      : result=BUKO_KEY_ENTER;        actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_BACKSPACE_RELEASE  : result=BUKO_KEY_BACKSPACE;    actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_LEFT_ALT_RELEASE   : result=BUKO_KEY_LEFT_ALT;     actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_SPACE_RELEASE      : result=BUKO_KEY_SPACE;        actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_DOT_RELEASE         : result=BUKO_KEY_DOT         ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_COMA_RELEASE        : result=BUKO_KEY_COMA        ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_SINGLE_QUOTE_RELEASE: result=BUKO_KEY_SINGLE_QUOTE; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_OPEN_SQUARE_RELEASE : result=BUKO_KEY_OPEN_SQUARE ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_CLOSE_SQUARE_RELEASE: result=BUKO_KEY_CLOSE_SQUARE; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_EQUALS_RELEASE      : result=BUKO_KEY_EQUALS      ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_MINUS_RELEASE       : result=BUKO_KEY_MINUS       ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       case QWERTY_US_BACKSLASH_RELEASE   : result=BUKO_KEY_BACKSLASH   ; actionType=BUKO_KEYBOARD_ACTION_RELEASE;break;
       }
   }
   switch(result) {
        case 0: break;
        case BUKO_KEY_LEFT_SHIFT:
        case BUKO_KEY_RIGHT_SHIFT:
             shifted=actionType!=BUKO_KEYBOARD_ACTION_RELEASE;
        break;
        case BUKO_KEY_LEFT_ALT:
        case BUKO_KEY_LEFT_CONTROL:
        break;
        case BUKO_KEY_ENTER:
            if(actionType) putC(display, '\n');
        break;
        case BUKO_KEY_SPACE:
            if(actionType) putC(display, ' ');
        break;
        case BUKO_KEY_COMA:
            if(actionType) {if(shifted) putC(display, '<'); else putC(display, ',');} break;
        case BUKO_KEY_DOT:
            if(actionType) {if(shifted) putC(display, '>'); else putC(display, '.');} break;
        case BUKO_KEY_SINGLE_QUOTE:
            if(actionType) {if(shifted) putC(display, '"'); else putC(display, '\'');} break;
        case BUKO_KEY_SEMICOLON:
            if(actionType) {if(shifted) putC(display, ':'); else putC(display, ';');} break;
        case BUKO_KEY_OPEN_SQUARE:
            if(actionType) {if(shifted) putC(display, '{'); else putC(display, '[');} break;
        case BUKO_KEY_CLOSE_SQUARE:
            if(actionType) {if(shifted) putC(display, '}'); else putC(display, ']');} break;
        case BUKO_KEY_MINUS:
            if(actionType) {if(shifted) putC(display, '_'); else putC(display, '-');} break;
        case BUKO_KEY_EQUALS:
            if(actionType) {if(shifted) putC(display, '+'); else putC(display, '=');} break;
        default: {
           if(result >= 'A' && result <= 'Z' && actionType!=BUKO_KEYBOARD_ACTION_RELEASE) {
                putC(display, shifted ? result : result-'A'+'a');
           }
           else if(result >= '0' && result <= '9' && actionType!=BUKO_KEYBOARD_ACTION_RELEASE) {
                putC(display, result);
           }
        } 
   }
   Kernel::outb(0x20, 0x20);
}
