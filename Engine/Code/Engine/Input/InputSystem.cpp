#include "Engine/Input/InputSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "Engine/Renderer/Window.hpp"

#include <algorithm>
#include <sstream>

KeyCode& operator++(KeyCode& keycode) {
    using IntType = typename std::underlying_type_t<KeyCode>;
    keycode = static_cast<KeyCode>(static_cast<IntType>(keycode) + 1);
    return keycode;
}

KeyCode operator++(KeyCode& keycode, int) {
    KeyCode result = keycode;
    ++keycode;
    return result;
}

unsigned char InputSystem::ConvertKeyCodeToWinVK(const KeyCode& code) {
    switch(code) {
    case KeyCode::LButton: return VK_LBUTTON;
    case KeyCode::RButton: return VK_RBUTTON;
    case KeyCode::Cancel: return VK_CANCEL;
    case KeyCode::MButton: return VK_MBUTTON;
    case KeyCode::XButton1: return VK_XBUTTON1;
    case KeyCode::XButton2: return VK_XBUTTON2;
    case KeyCode::Back: return VK_BACK; /* Also BACKSPACE */
    case KeyCode::Tab: return VK_TAB;
    case KeyCode::Clear: return VK_CLEAR;
    case KeyCode::Return: return VK_RETURN; /* Also ENTER */
    case KeyCode::Shift: return VK_SHIFT;
    case KeyCode::Ctrl: return VK_CONTROL; /* Also Ctrl */
    case KeyCode::Menu: return VK_MENU; /* Also Alt */
    case KeyCode::Pause: return VK_PAUSE;
    case KeyCode::Capital: return VK_CAPITAL; /* Also CAPSLOCK */
    case KeyCode::Kana: return VK_KANA;
    case KeyCode::Hangul: return VK_HANGUL; /* Also HANGEUL */
    case KeyCode::Junja: return VK_JUNJA;
    case KeyCode::Final: return VK_FINAL;
    case KeyCode::Hanja: return VK_HANJA;
    case KeyCode::Kanji: return VK_KANJI;
    case KeyCode::Escape: return VK_ESCAPE; /* Also ESC */
    case KeyCode::Convert: return VK_CONVERT;
    case KeyCode::NonConvert: return VK_NONCONVERT;
    case KeyCode::Accept: return VK_ACCEPT;
    case KeyCode::ModeChange: return VK_MODECHANGE;
    case KeyCode::Space: return VK_SPACE; /* Also SPACEBAR */
    case KeyCode::Prior: return VK_PRIOR; /* Also PAGEUP */
    case KeyCode::Next: return VK_NEXT; /* Also PAGEDOWN, PAGEDN */
    case KeyCode::End: return VK_END;
    case KeyCode::Home: return VK_HOME;
    case KeyCode::Left: return VK_LEFT;
    case KeyCode::Up: return VK_UP;
    case KeyCode::Right: return VK_RIGHT;
    case KeyCode::Down: return VK_DOWN;
    case KeyCode::Select: return VK_SELECT;
    case KeyCode::Print: return VK_PRINT;
    case KeyCode::Execute: return VK_EXECUTE;
    case KeyCode::Snapshot: return VK_SNAPSHOT; /* Also PRINTSCREEN */
    case KeyCode::Insert: return VK_INSERT;
    case KeyCode::Delete: return VK_DELETE;
    case KeyCode::Help: return VK_HELP;
    case KeyCode::Numeric0: return '0';
    case KeyCode::Numeric1: return '1';
    case KeyCode::Numeric2: return '2';
    case KeyCode::Numeric3: return '3';
    case KeyCode::Numeric4: return '4';
    case KeyCode::Numeric5: return '5';
    case KeyCode::Numeric6: return '6';
    case KeyCode::Numeric7: return '7';
    case KeyCode::Numeric8: return '8';
    case KeyCode::Numeric9: return '9';
    case KeyCode::A: return 'A';
    case KeyCode::B: return 'B';
    case KeyCode::C: return 'C';
    case KeyCode::D: return 'D';
    case KeyCode::E: return 'E';
    case KeyCode::F: return 'F';
    case KeyCode::G: return 'G';
    case KeyCode::H: return 'H';
    case KeyCode::I: return 'I';
    case KeyCode::J: return 'J';
    case KeyCode::K: return 'K';
    case KeyCode::L: return 'L';
    case KeyCode::M: return 'M';
    case KeyCode::N: return 'N';
    case KeyCode::O: return 'O';
    case KeyCode::P: return 'P';
    case KeyCode::Q: return 'Q';
    case KeyCode::R: return 'R';
    case KeyCode::S: return 'S';
    case KeyCode::T: return 'T';
    case KeyCode::U: return 'U';
    case KeyCode::V: return 'V';
    case KeyCode::W: return 'W';
    case KeyCode::X: return 'X';
    case KeyCode::Y: return 'Y';
    case KeyCode::Z: return 'Z';
    case KeyCode::LWin: return VK_LWIN;
    case KeyCode::RWin: return VK_RWIN;
    case KeyCode::Apps: return VK_APPS;
    case KeyCode::Sleep: return VK_SLEEP;
    case KeyCode::NumPad0: return VK_NUMPAD0;
    case KeyCode::NumPad1: return VK_NUMPAD1;
    case KeyCode::NumPad2: return VK_NUMPAD2;
    case KeyCode::NumPad3: return VK_NUMPAD3;
    case KeyCode::NumPad4: return VK_NUMPAD4;
    case KeyCode::NumPad5: return VK_NUMPAD5;
    case KeyCode::NumPad6: return VK_NUMPAD6;
    case KeyCode::NumPad7: return VK_NUMPAD7;
    case KeyCode::NumPad8: return VK_NUMPAD8;
    case KeyCode::NumPad9: return VK_NUMPAD9;
    case KeyCode::Multiply: return VK_MULTIPLY;
    case KeyCode::Add: return VK_ADD;
    case KeyCode::Separator: return VK_SEPARATOR; /* Also KEYPADENTER */
    case KeyCode::Subtract: return VK_SUBTRACT;
    case KeyCode::Decimal: return VK_DECIMAL;
    case KeyCode::Divide: return VK_DIVIDE;
    case KeyCode::F1:  return VK_F1;
    case KeyCode::F2:  return VK_F2;
    case KeyCode::F3:  return VK_F3;
    case KeyCode::F4:  return VK_F4;
    case KeyCode::F5:  return VK_F5;
    case KeyCode::F6:  return VK_F6;
    case KeyCode::F7:  return VK_F7;
    case KeyCode::F8:  return VK_F8;
    case KeyCode::F9:  return VK_F9;
    case KeyCode::F10: return VK_F10;
    case KeyCode::F11: return VK_F11;
    case KeyCode::F12: return VK_F12;
    case KeyCode::F13: return VK_F13;
    case KeyCode::F14: return VK_F14;
    case KeyCode::F15: return VK_F15;
    case KeyCode::F16: return VK_F16;
    case KeyCode::F17: return VK_F17;
    case KeyCode::F18: return VK_F18;
    case KeyCode::F19: return VK_F19;
    case KeyCode::F20: return VK_F20;
    case KeyCode::F21: return VK_F21;
    case KeyCode::F22: return VK_F22;
    case KeyCode::F23: return VK_F23;
    case KeyCode::F24: return VK_F24;
    case KeyCode::NumLock: return VK_NUMLOCK;
    case KeyCode::Scroll: return VK_SCROLL; /* Also SCROLLLOCK */
    case KeyCode::Oem_Nec_Equal: return VK_OEM_NEC_EQUAL;
    case KeyCode::Oem_Fj_Jisho: return VK_OEM_FJ_JISHO;
    case KeyCode::Oem_Fj_Masshou: return VK_OEM_FJ_MASSHOU;
    case KeyCode::Oem_Fj_Touroku: return VK_OEM_FJ_TOUROKU;
    case KeyCode::Oem_Fj_Loya: return VK_OEM_FJ_LOYA;
    case KeyCode::Oem_Fj_Roya: return VK_OEM_FJ_ROYA;
    case KeyCode::LShift: return VK_LSHIFT;
    case KeyCode::RShift: return VK_RSHIFT;
    case KeyCode::LControl: return VK_LCONTROL; /* Also LCTRL */
    case KeyCode::RControl: return VK_RCONTROL; /* Also RCtrl */
    case KeyCode::RMenu: return VK_RMENU; /* Also RAlt */
    case KeyCode::Browser_Back: return VK_BROWSER_BACK;
    case KeyCode::Browser_Forward: return VK_BROWSER_FORWARD;
    case KeyCode::Browser_Refresh: return VK_BROWSER_REFRESH;
    case KeyCode::Browser_Stop: return VK_BROWSER_STOP;
    case KeyCode::Browser_Search: return VK_BROWSER_SEARCH;
    case KeyCode::Browser_Favorites: return VK_BROWSER_FAVORITES;
    case KeyCode::Browser_Home: return VK_BROWSER_HOME;
    case KeyCode::Volume_Mute: return VK_VOLUME_MUTE;
    case KeyCode::Volume_Down: return VK_VOLUME_DOWN;
    case KeyCode::Volume_Up: return VK_VOLUME_UP;
    case KeyCode::Media_Next_Track: return VK_MEDIA_NEXT_TRACK;
    case KeyCode::Media_Prev_Track: return VK_MEDIA_PREV_TRACK;
    case KeyCode::Media_Stop: return VK_MEDIA_STOP;
    case KeyCode::Media_Play_Pause: return VK_MEDIA_PLAY_PAUSE;
    case KeyCode::Launch_Mail: return VK_LAUNCH_MAIL;
    case KeyCode::Launch_Media_Select: return VK_LAUNCH_MEDIA_SELECT;
    case KeyCode::Launch_App1: return VK_LAUNCH_APP1;
    case KeyCode::Launch_App2: return VK_LAUNCH_APP2;
    case KeyCode::Oem_1: return VK_OEM_1; /* ;: */
    case KeyCode::Oem_Plus: return VK_OEM_PLUS; /* =+ */
    case KeyCode::Oem_Comma: return VK_OEM_COMMA; /* ,< */
    case KeyCode::Oem_Minus: return VK_OEM_MINUS; /* -_ */
    case KeyCode::Oem_Period: return VK_OEM_PERIOD; /* .> */
    case KeyCode::Oem_2: return VK_OEM_2; /* /? */
    case KeyCode::Oem_3: return VK_OEM_3; /* `~ */
    case KeyCode::Oem_4: return VK_OEM_4; /* [{ */
    case KeyCode::Oem_5: return VK_OEM_5; /* \| */
    case KeyCode::Oem_6: return VK_OEM_6; /* ]} */
    case KeyCode::Oem_7: return VK_OEM_7; /* '" */
    case KeyCode::Oem_8: return VK_OEM_8; /* misc. unknown */
    case KeyCode::Oem_Ax: return VK_OEM_AX;
    case KeyCode::Oem_102: return VK_OEM_102; /* RT 102's "<>" or "\|" */
    case KeyCode::Ico_Help: return VK_ICO_HELP; /* Help key on ICO keyboard */
    case KeyCode::Ico_00: return VK_ICO_00; /* 00 key on ICO keyboard */
    case KeyCode::ProcessKey: return VK_PROCESSKEY;
    case KeyCode::Ico_Clear: return VK_ICO_CLEAR; /* Clear key on ICO keyboard */
    case KeyCode::Packet: return VK_PACKET; /* Key is packet of data */
    case KeyCode::Oem_Reset: return VK_OEM_RESET;
    case KeyCode::Oem_Jump: return VK_OEM_JUMP;
    case KeyCode::Oem_Pa1: return VK_OEM_PA1;
    case KeyCode::Oem_Pa2: return VK_OEM_PA2;
    case KeyCode::Oem_Pa3: return VK_OEM_PA3;
    case KeyCode::Oem_WsCtrl: return VK_OEM_WSCTRL;
    case KeyCode::Oem_CuSel: return VK_OEM_CUSEL;
    case KeyCode::Oem_Attn: return VK_OEM_ATTN;
    case KeyCode::Oem_Finish: return VK_OEM_FINISH;
    case KeyCode::Oem_Copy: return VK_OEM_COPY;
    case KeyCode::Oem_Auto: return VK_OEM_AUTO;
    case KeyCode::Oem_EnlW: return VK_OEM_ENLW;
    case KeyCode::Oem_BackTab: return VK_OEM_BACKTAB;
    case KeyCode::Attn: return VK_ATTN;
    case KeyCode::CrSel: return VK_CRSEL;
    case KeyCode::ExSel: return VK_EXSEL;
    case KeyCode::ErEof: return VK_EREOF;
    case KeyCode::Play: return VK_PLAY;
    case KeyCode::Zoom: return VK_ZOOM;
    case KeyCode::NoName: return VK_NONAME;
    case KeyCode::Pa1: return VK_PA1;
    case KeyCode::Oem_Clear: return VK_OEM_CLEAR;
    case KeyCode::Unknown: return 0xFF;
    default: return 0xFF;
    }
}

KeyCode InputSystem::ConvertWinVKToKeyCode(unsigned char winVK) {
    switch(winVK) {
    case VK_LBUTTON: return KeyCode::LButton;
    case VK_RBUTTON: return KeyCode::RButton;
    case VK_CANCEL: return KeyCode::Cancel;
    case VK_MBUTTON: return KeyCode::MButton;
    case VK_XBUTTON1: return KeyCode::XButton1;
    case VK_XBUTTON2: return KeyCode::XButton2;
    case VK_BACK:  return KeyCode::Back;
    case VK_TAB: return KeyCode::Tab;
    case VK_CLEAR: return KeyCode::Clear;
    case VK_RETURN: return KeyCode::Return;
    case VK_SHIFT: return KeyCode::Shift;
    case VK_CONTROL: return KeyCode::Ctrl;
    case VK_MENU: return KeyCode::Alt;
    case VK_PAUSE: return KeyCode::Pause;
    case VK_CAPITAL: return KeyCode::Capital;
    case VK_KANA: return KeyCode::Kana;
    //case VK_HANGUL: return KeyCode::HANGUL;
    case VK_JUNJA: return KeyCode::Junja;
    case VK_FINAL:  return KeyCode::Final;
    case VK_HANJA:  return KeyCode::Hanja;
    //case VK_KANJI:  return KeyCode::KANJI;
    case VK_ESCAPE: return KeyCode::Escape;
    case VK_CONVERT:  return KeyCode::Convert;
    case VK_NONCONVERT:  return KeyCode::NonConvert;
    case VK_ACCEPT:  return KeyCode::Accept;
    case VK_MODECHANGE:  return KeyCode::ModeChange;
    case VK_SPACE: return KeyCode::Space;
    case VK_PRIOR: return KeyCode::Prior;
    case VK_NEXT: return KeyCode::Next;
    case VK_END:  return KeyCode::End;
    case VK_HOME:  return KeyCode::Home;
    case VK_LEFT:  return KeyCode::Left;
    case VK_UP:  return KeyCode::Up;
    case VK_RIGHT:  return KeyCode::Right;
    case VK_DOWN:  return KeyCode::Down;
    case VK_SELECT:  return KeyCode::Select;
    case VK_PRINT:  return KeyCode::Print;
    case VK_EXECUTE:  return KeyCode::Execute;
    case VK_SNAPSHOT: return KeyCode::Snapshot;
    case VK_INSERT:  return KeyCode::Insert;
    case VK_DELETE: return KeyCode::Delete;
    case VK_HELP:  return KeyCode::Help;
    case '0':  return KeyCode::Numeric0;
    case '1':  return KeyCode::Numeric1;
    case '2':  return KeyCode::Numeric2;
    case '3':  return KeyCode::Numeric3;
    case '4':  return KeyCode::Numeric4;
    case '5':  return KeyCode::Numeric5;
    case '6':  return KeyCode::Numeric6;
    case '7':  return KeyCode::Numeric7;
    case '8':  return KeyCode::Numeric8;
    case '9':  return KeyCode::Numeric9;
    case 'A':  return KeyCode::A;
    case 'B':  return KeyCode::B;
    case 'C':  return KeyCode::C;
    case 'D':  return KeyCode::D;
    case 'E':  return KeyCode::E;
    case 'F':  return KeyCode::F;
    case 'G':  return KeyCode::G;
    case 'H':  return KeyCode::H;
    case 'I':  return KeyCode::I;
    case 'J':  return KeyCode::J;
    case 'K':  return KeyCode::K;
    case 'L':  return KeyCode::L;
    case 'M':  return KeyCode::M;
    case 'N':  return KeyCode::N;
    case 'O':  return KeyCode::O;
    case 'P':  return KeyCode::P;
    case 'Q':  return KeyCode::Q;
    case 'R':  return KeyCode::R;
    case 'S':  return KeyCode::S;
    case 'T':  return KeyCode::T;
    case 'U':  return KeyCode::U;
    case 'V':  return KeyCode::V;
    case 'W':  return KeyCode::W;
    case 'X':  return KeyCode::X;
    case 'Y':  return KeyCode::Y;
    case 'Z':  return KeyCode::Z;
    case VK_LWIN:  return KeyCode::LWin;
    case VK_RWIN:  return KeyCode::RWin;
    case VK_APPS:  return KeyCode::Apps;
    case VK_SLEEP:  return KeyCode::Sleep;
    case VK_NUMPAD0:  return KeyCode::NumPad0;
    case VK_NUMPAD1:  return KeyCode::NumPad1;
    case VK_NUMPAD2:  return KeyCode::NumPad2;
    case VK_NUMPAD3:  return KeyCode::NumPad3;
    case VK_NUMPAD4:  return KeyCode::NumPad4;
    case VK_NUMPAD5:  return KeyCode::NumPad5;
    case VK_NUMPAD6:  return KeyCode::NumPad6;
    case VK_NUMPAD7:  return KeyCode::NumPad7;
    case VK_NUMPAD8:  return KeyCode::NumPad8;
    case VK_NUMPAD9:  return KeyCode::NumPad9;
    case VK_MULTIPLY:  return KeyCode::Multiply;
    case VK_ADD:  return KeyCode::Add;
    case VK_SEPARATOR:  return KeyCode::Separator;
    case VK_SUBTRACT:  return KeyCode::Subtract;
    case VK_DECIMAL:  return KeyCode::Decimal;
    case VK_DIVIDE:  return KeyCode::Divide;
    case VK_F1:  return KeyCode::F1;
    case VK_F2:  return KeyCode::F2;
    case VK_F3:   return KeyCode::F3;
    case VK_F4:   return KeyCode::F4;
    case VK_F5:   return KeyCode::F5;
    case VK_F6:   return KeyCode::F6;
    case VK_F7:   return KeyCode::F7;
    case VK_F8:   return KeyCode::F8;
    case VK_F9:   return KeyCode::F9;
    case VK_F10:   return KeyCode::F10;
    case VK_F11:   return KeyCode::F11;
    case VK_F12:   return KeyCode::F12;
    case VK_F13:   return KeyCode::F13;
    case VK_F14:   return KeyCode::F14;
    case VK_F15:   return KeyCode::F15;
    case VK_F16:   return KeyCode::F16;
    case VK_F17:   return KeyCode::F17;
    case VK_F18:   return KeyCode::F18;
    case VK_F19:   return KeyCode::F19;
    case VK_F20:   return KeyCode::F20;
    case VK_F21:   return KeyCode::F21;
    case VK_F22:   return KeyCode::F22;
    case VK_F23:   return KeyCode::F23;
    case VK_F24:   return KeyCode::F24;
    case VK_NUMLOCK: return KeyCode::NumLock;
    case VK_SCROLL: return KeyCode::Scroll;
    case VK_OEM_NEC_EQUAL:  return KeyCode::Oem_Nec_Equal;
    //case VK_OEM_FJ_JISHO:  return KeyCode::OEM_FJ_JISHO;
    case VK_OEM_FJ_MASSHOU:  return KeyCode::Oem_Fj_Masshou;
    case VK_OEM_FJ_TOUROKU:  return KeyCode::Oem_Fj_Touroku;
    case VK_OEM_FJ_LOYA:  return KeyCode::Oem_Fj_Loya;
    case VK_OEM_FJ_ROYA:  return KeyCode::Oem_Fj_Roya;
    case VK_LSHIFT:  return KeyCode::LShift;
    case VK_RSHIFT:  return KeyCode::RShift;
    case VK_LCONTROL: return KeyCode::LControl;
    case VK_RCONTROL: return KeyCode::RControl;
    case VK_LMENU: return KeyCode::LAlt;
    case VK_RMENU: return KeyCode::RAlt;
    case VK_BROWSER_BACK:  return KeyCode::Browser_Back;
    case VK_BROWSER_FORWARD:  return KeyCode::Browser_Forward;
    case VK_BROWSER_REFRESH:  return KeyCode::Browser_Refresh;
    case VK_BROWSER_STOP:  return KeyCode::Browser_Stop;
    case VK_BROWSER_SEARCH:  return KeyCode::Browser_Search;
    case VK_BROWSER_FAVORITES:  return KeyCode::Browser_Favorites;
    case VK_BROWSER_HOME:  return KeyCode::Browser_Home;
    case VK_VOLUME_MUTE:  return KeyCode::Volume_Mute;
    case VK_VOLUME_DOWN:  return KeyCode::Volume_Down;
    case VK_VOLUME_UP:  return KeyCode::Volume_Up;
    case VK_MEDIA_NEXT_TRACK:  return KeyCode::Media_Next_Track;
    case VK_MEDIA_PREV_TRACK:  return KeyCode::Media_Prev_Track;
    case VK_MEDIA_STOP:  return KeyCode::Media_Stop;
    case VK_MEDIA_PLAY_PAUSE:  return KeyCode::Media_Play_Pause;
    case VK_LAUNCH_MAIL:  return KeyCode::Launch_Mail;
    case VK_LAUNCH_MEDIA_SELECT:  return KeyCode::Launch_Media_Select;
    case VK_LAUNCH_APP1:  return KeyCode::Launch_App1;
    case VK_LAUNCH_APP2:  return KeyCode::Launch_App2;
    case VK_OEM_1: return KeyCode::Semicolon;
    case VK_OEM_PLUS: return KeyCode::Equals;
    case VK_OEM_COMMA: return KeyCode::Comma;
    case VK_OEM_MINUS: return KeyCode::Minus;
    case VK_OEM_PERIOD: return KeyCode::Period;
    case VK_OEM_2: return KeyCode::ForwardSlash;
    case VK_OEM_3: return KeyCode::Tilde;
    case VK_GAMEPAD_A: return KeyCode::Gamepad_A;
    case VK_GAMEPAD_B: return KeyCode::Gamepad_B;
    case VK_GAMEPAD_X: return KeyCode::Gamepad_X;
    case VK_GAMEPAD_Y: return KeyCode::Gamepad_Y;
    case VK_GAMEPAD_RIGHT_SHOULDER: return KeyCode::Gamepad_Right_Shoulder;
    case VK_GAMEPAD_LEFT_SHOULDER: return KeyCode::Gamepad_Left_Shoulder;
    case VK_GAMEPAD_LEFT_TRIGGER: return KeyCode::Gamepad_Left_Trigger;
    case VK_GAMEPAD_RIGHT_TRIGGER: return KeyCode::Gamepad_Right_Trigger;
    case VK_GAMEPAD_DPAD_UP: return KeyCode::Gamepad_DPad_Up;
    case VK_GAMEPAD_DPAD_DOWN: return KeyCode::Gamepad_DPad_Down;
    case VK_GAMEPAD_DPAD_LEFT: return KeyCode::Gamepad_DPad_Left;
    case VK_GAMEPAD_DPAD_RIGHT: return KeyCode::Gamepad_DPad_Right;
    case VK_GAMEPAD_MENU: return KeyCode::Gamepad_Menu;
    case VK_GAMEPAD_VIEW: return KeyCode::Gamepad_View;
    case VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON: return KeyCode::Gamepad_Left_Thumbstick_Button;
    case VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON: return KeyCode::Gamepad_Right_Thumbstick_Button;
    case VK_GAMEPAD_LEFT_THUMBSTICK_UP: return KeyCode::Gamepad_Left_Thumbstick_Up;
    case VK_GAMEPAD_LEFT_THUMBSTICK_DOWN: return KeyCode::Gamepad_Left_Thumbstick_Down;
    case VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT: return KeyCode::Gamepad_Left_Thumbstick_Right;
    case VK_GAMEPAD_LEFT_THUMBSTICK_LEFT: return KeyCode::Gamepad_Left_Thumbstick_Left;
    case VK_GAMEPAD_RIGHT_THUMBSTICK_UP: return KeyCode::Gamepad_Right_Thumbstick_Up;
    case VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN: return KeyCode::Gamepad_Right_Thumbstick_Down;
    case VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT: return KeyCode::Gamepad_Right_Thumbstick_Right;
    case VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT: return KeyCode::Gamepad_Right_Thumbstick_Left;
    case VK_OEM_4: return KeyCode::LBracket;
    case VK_OEM_5: return KeyCode::Backslash;
    case VK_OEM_6: return KeyCode::RBracket;
    case VK_OEM_7: return KeyCode::SingleQuote;
    case VK_OEM_8: return KeyCode::Oem_8;
    case VK_OEM_AX:  return KeyCode::Oem_Ax;
    case VK_OEM_102: return KeyCode::Oem_102;
    case VK_ICO_HELP: return KeyCode::Ico_Help;
    case VK_ICO_00: return KeyCode::Ico_00;
    case VK_PROCESSKEY:  return KeyCode::ProcessKey;
    case VK_ICO_CLEAR: return KeyCode::Ico_Clear;
    case VK_PACKET: return KeyCode::Packet;
    case VK_OEM_RESET:  return KeyCode::Oem_Reset;
    case VK_OEM_JUMP:  return KeyCode::Oem_Jump;
    case VK_OEM_PA1:  return KeyCode::Oem_Pa1;
    case VK_OEM_PA2:  return KeyCode::Oem_Pa2;
    case VK_OEM_PA3:  return KeyCode::Oem_Pa3;
    case VK_OEM_WSCTRL:  return KeyCode::Oem_WsCtrl;
    case VK_OEM_CUSEL: return KeyCode::Oem_CuSel;
    case VK_OEM_ATTN:  return KeyCode::Oem_Attn;
    case VK_OEM_FINISH:  return KeyCode::Oem_Finish;
    case VK_OEM_COPY:  return KeyCode::Oem_Copy;
    case VK_OEM_AUTO:  return KeyCode::Oem_Auto;
    case VK_OEM_ENLW:  return KeyCode::Oem_EnlW;
    case VK_OEM_BACKTAB:  return KeyCode::Oem_BackTab;
    case VK_ATTN: return KeyCode::Attn;
    case VK_CRSEL: return KeyCode::CrSel;
    case VK_EXSEL: return KeyCode::ExSel;
    case VK_EREOF: return KeyCode::ErEof;
    case VK_PLAY: return KeyCode::Play;
    case VK_ZOOM: return KeyCode::Zoom;
    case VK_NONAME: return KeyCode::NoName;
    case VK_PA1: return KeyCode::Pa1;
    case VK_OEM_CLEAR: return KeyCode::Oem_Clear;
    default: return KeyCode::Unknown;
    }
}

Vector2 InputSystem::GetCursorWindowPosition(const Window& window_ref) const {
    POINT p;
    if(::GetCursorPos(&p)) {
        if(::ScreenToClient(window_ref.GetWindowHandle(), &p)) {
            return Vector2{ static_cast<float>(p.x), static_cast<float>(p.y) };
        }
    }
    return Vector2::ZERO;
}

Vector2 InputSystem::GetCursorScreenPosition() const {
    POINT p;
    if(::GetCursorPos(&p)) {
        return Vector2{ static_cast<float>(p.x), static_cast<float>(p.y) };
    }
    return Vector2::ZERO;
}

void InputSystem::SetCursorToScreenCenter() {
    HWND desktop_window = ::GetDesktopWindow();
    RECT desktop_client;
    if(::GetClientRect(desktop_window, &desktop_client)) {
        float center_x = (desktop_client.left + desktop_client.right) * 0.5f;
        float center_y = (desktop_client.top + desktop_client.bottom) * 0.5f;
        SetCursorScreenPosition(Vector2{ center_x, center_y });
    }
}

void InputSystem::SetCursorToWindowCenter(const Window& window_ref) {
    RECT client_area;
    if(::GetClientRect(window_ref.GetWindowHandle(), &client_area)) {
        float center_x = (client_area.left + client_area.right) * 0.5f;
        float center_y = (client_area.top + client_area.bottom) * 0.5f;
        SetCursorWindowPosition(window_ref, Vector2{ center_x, center_y });
    }
}

void InputSystem::SetCursorScreenPosition(const Vector2& screen_pos) {
    int x = static_cast<int>(screen_pos.x);
    int y = static_cast<int>(screen_pos.y);
    ::SetCursorPos(x, y);
}

void InputSystem::UpdateXboxConnectedState() {
    _connected_controller_count = 0;
    for(int i = 0; i < 4; ++i) {
        _xboxControllers[i].UpdateConnectedState(i);
        if(_xboxControllers[i].WasJustConnected() || _xboxControllers[i].IsConnected()) {
            ++_connected_controller_count;
        }
    }
}

Vector2 InputSystem::GetScreenCenter() const {
    RECT desktopRect;
    HWND desktopWindowHandle = ::GetDesktopWindow();
    if(::GetClientRect(desktopWindowHandle, &desktopRect)) {
        float center_x = (desktopRect.right + desktopRect.left) * 0.50f;
        float center_y = (desktopRect.bottom + desktopRect.top) * 0.50f;
        return Vector2{ center_x, center_y };
    }
    return Vector2::ZERO;
}

Vector2 InputSystem::GetWindowCenter(const Window& window) const {
    RECT rect;
    HWND windowHandle = window.GetWindowHandle();
    if(::GetClientRect(windowHandle, &rect)) {
        float center_x = (rect.right + rect.left) * 0.50f;
        float center_y = (rect.bottom + rect.top) * 0.50f;
        return Vector2{ center_x, center_y };
    }
    return Vector2::ZERO;
}

void InputSystem::HideMouseCursor() {
    while(::ShowCursor(FALSE) >= 0);
    _cursor_visible = false;
}

void InputSystem::ShowMouseCursor() {
    while(::ShowCursor(TRUE) < 0);
    _cursor_visible = true;
}

void InputSystem::ToggleMouseCursorVisibility() {
    _cursor_visible ? HideMouseCursor() : ShowMouseCursor();
}

void InputSystem::SetCursorWindowPosition(const Window& window, const Vector2& window_pos) {
    POINT p;
    p.x = static_cast<long>(window_pos.x);
    p.y = static_cast<long>(window_pos.y);
    if(::ClientToScreen(window.GetWindowHandle(), &p)) {
        SetCursorScreenPosition(Vector2{ static_cast<float>(p.x), static_cast<float>(p.y) });
    }
}

const Vector2& InputSystem::GetMouseCoords() const {
    return _mouseCoords;
}

int InputSystem::GetMouseWheelPosition() const {
    return _mouseWheelPosition;
}

int InputSystem::GetMouseWheelPositionNormalized() const {
    if(_mouseWheelPosition) {
        return _mouseWheelPosition / std::abs(_mouseWheelPosition);
    }
    return 0;
}

int InputSystem::GetMouseWheelHorizontalPosition() const {
    return _mouseWheelHPosition;
}

int InputSystem::GetMouseWheelHorizontalPositionNormalized() const {
    if(_mouseWheelHPosition) {
        return _mouseWheelHPosition / std::abs(_mouseWheelHPosition);
    }
    return 0;
}

IntVector2 InputSystem::GetMouseWheelPositionAsIntVector2() const {
    return IntVector2{_mouseWheelHPosition, _mouseWheelPosition};
}

void InputSystem::RegisterKeyDown(unsigned char keyIndex) {
    auto kc = ConvertWinVKToKeyCode(keyIndex);
    _currentKeys[(std::size_t)kc] = true;
}

void InputSystem::RegisterKeyUp(unsigned char keyIndex) {
    auto kc = ConvertWinVKToKeyCode(keyIndex);
    _currentKeys[(std::size_t)kc] = false;

}

bool InputSystem::ProcessSystemMessage(const EngineMessage& msg) {

    LPARAM lp = msg.lparam;
    WPARAM wp = msg.wparam;
    switch(msg.wmMessageCode) {
        case WindowsSystemMessage::Keyboard_KeyDown:
        {
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 0 for KEYDOWN
            //P: previous state 1 for already down
            //T: transition state 0 for KEYDOWN
            constexpr uint32_t repeat_count_mask     = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask        = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask     = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask         = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask     = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask   = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            constexpr uint16_t keystate_state_mask = 0b1000'0000'0000'0000;  //0x8000
            constexpr uint16_t keystate_toggle_mask = 0b0000'0000'0000'0001; //0x0001
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            auto my_key = ConvertWinVKToKeyCode(key);
            if(my_key == KeyCode::Unknown) {
                return true;
            }
            if(is_extended_key) {
                switch(my_key) {
                case KeyCode::Shift:
                    {
                    auto left_key = !!(::GetKeyState(VK_LSHIFT) & keystate_state_mask);
                    auto right_key = !!(::GetKeyState(VK_RSHIFT) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LSHIFT);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RSHIFT);
                    auto shift_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyDown(shift_key);
                    }
                    break;
                    }
                case KeyCode::Alt:
                    {
                        auto left_key = !!(::GetKeyState(VK_LMENU) & keystate_state_mask);
                        auto right_key = !!(::GetKeyState(VK_RMENU) & keystate_state_mask);
                        auto my_leftkey = ConvertWinVKToKeyCode(VK_LMENU);
                        auto my_rightkey = ConvertWinVKToKeyCode(VK_RMENU);
                        auto alt_key = ConvertKeyCodeToWinVK(my_key);
                        my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                        if(my_key != KeyCode::Unknown) {
                            RegisterKeyDown(alt_key);
                        }
                        break;
                    }
                case KeyCode::Ctrl:
                    {
                        auto left_key = !!(::GetKeyState(VK_LCONTROL) & keystate_state_mask);
                        auto right_key = !!(::GetKeyState(VK_RCONTROL) & keystate_state_mask);
                        auto my_leftkey = ConvertWinVKToKeyCode(VK_LCONTROL);
                        auto my_rightkey = ConvertWinVKToKeyCode(VK_RCONTROL);
                        auto ctrl_key = ConvertKeyCodeToWinVK(my_key);
                        my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                        if(my_key != KeyCode::Unknown) {
                            RegisterKeyDown(ctrl_key);
                        }
                        break;
                    }
                case KeyCode::Return: my_key = KeyCode::NumPadEnter; break;
                case KeyCode::LWin:
                    {
                        auto left_key = !!(::GetKeyState(VK_LWIN) & keystate_state_mask);
                        auto my_leftkey = ConvertWinVKToKeyCode(VK_LWIN);
                        my_key = left_key ? my_leftkey : KeyCode::Unknown;
                        break;
                    }
                case KeyCode::RWin:
                    {
                        auto right_key = !!(::GetKeyState(VK_RWIN) & keystate_state_mask);
                        auto my_rightkey = ConvertWinVKToKeyCode(VK_RWIN);
                        my_key = right_key ? my_rightkey : KeyCode::Unknown;
                        break;
                    }
                }
            }
            switch(my_key) {
            case KeyCode::Shift:
                {
                    auto left_key = !!(::GetKeyState(VK_LSHIFT) & keystate_state_mask);
                    auto right_key = !!(::GetKeyState(VK_RSHIFT) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LSHIFT);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RSHIFT);
                    auto shift_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyDown(shift_key);
                    }
                    break;
                }
            case KeyCode::Ctrl:
                {
                    auto left_key = !!(::GetKeyState(VK_LCONTROL) & keystate_state_mask);
                    auto right_key = !!(::GetKeyState(VK_RCONTROL) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LCONTROL);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RCONTROL);
                    auto ctrl_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyDown(ctrl_key);
                    }
                    break;
                }
            case KeyCode::Alt:
                {
                    auto left_key = !!(::GetKeyState(VK_LMENU) & keystate_state_mask);
                    auto right_key = !!(::GetKeyState(VK_RMENU) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LMENU);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RMENU);
                    auto alt_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyDown(alt_key);
                    }
                    break;
                }
            }
            key = ConvertKeyCodeToWinVK(my_key);
            RegisterKeyDown(key); return true;
        }
        case WindowsSystemMessage::Keyboard_KeyUp:
        {
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 0 for KEYUP
            //P: previous state 1 for already down
            //T: transition state 1 for KEYUP
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            constexpr uint16_t keystate_state_mask = 0b1000'0000'0000'0000;  //0x8000
            constexpr uint16_t keystate_toggle_mask = 0b0000'0000'0000'0001; //0x0001
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            auto my_key = ConvertWinVKToKeyCode(key);
            if(my_key == KeyCode::Unknown) {
                return true;
            }
            if(is_extended_key) {
                switch(my_key) {
                case KeyCode::Shift:
                {
                    auto left_down = this->IsKeyDown(KeyCode::LShift);
                    auto right_down = this->IsKeyDown(KeyCode::RShift);
                    auto left_key  = left_down && !!!(::GetKeyState(VK_LSHIFT) & keystate_state_mask);
                    auto right_key = right_down && !!!(::GetKeyState(VK_RSHIFT) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LSHIFT);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RSHIFT);
                    auto shift_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyUp(shift_key);
                    }
                    break;
                }
                case KeyCode::Alt:
                {
                    auto left_down = this->IsKeyDown(KeyCode::LAlt);
                    auto right_down = this->IsKeyDown(KeyCode::RAlt);
                    auto left_key = left_down && !!!(::GetKeyState(VK_LMENU) & keystate_state_mask);
                    auto right_key = right_down && !!!(::GetKeyState(VK_RMENU) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LMENU);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RMENU);
                    auto alt_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyUp(alt_key);
                    }
                    break;
                }
                case KeyCode::Ctrl:
                {
                    auto left_down = this->IsKeyDown(KeyCode::LControl);
                    auto right_down = this->IsKeyDown(KeyCode::RControl);
                    auto left_key = left_down && !!!(::GetKeyState(VK_LCONTROL) & keystate_state_mask);
                    auto right_key = right_down && !!!(::GetKeyState(VK_RCONTROL) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LCONTROL);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RCONTROL);
                    auto ctrl_key = ConvertKeyCodeToWinVK(my_key);
                    my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyUp(ctrl_key);
                    }
                    break;
                }
                case KeyCode::Return: my_key = KeyCode::NumPadEnter; break;
                case KeyCode::LWin:
                {
                    auto left_down = this->IsKeyDown(KeyCode::LWin);
                    auto left_key = left_down && !!!(::GetKeyState(VK_LWIN) & keystate_state_mask);
                    auto my_leftkey = ConvertWinVKToKeyCode(VK_LWIN);
                    my_key = left_key ? my_leftkey : KeyCode::Unknown;
                    break;
                }
                case KeyCode::RWin:
                {
                    auto right_down = this->IsKeyDown(KeyCode::RWin);
                    auto right_key = right_down && !!!(::GetKeyState(VK_RWIN) & keystate_state_mask);
                    auto my_rightkey = ConvertWinVKToKeyCode(VK_RWIN);
                    my_key = right_key ? my_rightkey : KeyCode::Unknown;
                    break;
                }
                }
            }
            switch(my_key) {
            case KeyCode::Shift:
            {
                auto left_down = this->IsKeyDown(KeyCode::LShift);
                auto right_down = this->IsKeyDown(KeyCode::RShift);
                auto left_key = left_down && !!!(::GetKeyState(VK_LSHIFT) & keystate_state_mask);
                auto right_key = right_down && !!!(::GetKeyState(VK_RSHIFT) & keystate_state_mask);
                auto my_leftkey = ConvertWinVKToKeyCode(VK_LSHIFT);
                auto my_rightkey = ConvertWinVKToKeyCode(VK_RSHIFT);
                auto shift_key = ConvertKeyCodeToWinVK(my_key);
                my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                if(my_key != KeyCode::Unknown) {
                    RegisterKeyUp(shift_key);
                }
                break;
            }
            case KeyCode::Ctrl:
            {
                auto left_down = this->IsKeyDown(KeyCode::LControl);
                auto right_down = this->IsKeyDown(KeyCode::RControl);
                auto left_key = left_down && !!!(::GetKeyState(VK_LCONTROL) & keystate_state_mask);
                auto right_key = right_down && !!!(::GetKeyState(VK_RCONTROL) & keystate_state_mask);
                auto my_leftkey = ConvertWinVKToKeyCode(VK_LCONTROL);
                auto my_rightkey = ConvertWinVKToKeyCode(VK_RCONTROL);
                auto ctrl_key = ConvertKeyCodeToWinVK(my_key);
                my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                if(my_key != KeyCode::Unknown) {
                    RegisterKeyUp(ctrl_key);
                }
                break;
            }
            case KeyCode::Alt:
            {
                auto left_down = this->IsKeyDown(KeyCode::LAlt);
                auto right_down = this->IsKeyDown(KeyCode::RAlt);
                auto left_key = left_down && !!!(::GetKeyState(VK_LMENU) & keystate_state_mask);
                auto right_key = right_down && !!!(::GetKeyState(VK_RMENU) & keystate_state_mask);
                auto my_leftkey = ConvertWinVKToKeyCode(VK_LMENU);
                auto my_rightkey = ConvertWinVKToKeyCode(VK_RMENU);
                auto alt_key = ConvertKeyCodeToWinVK(my_key);
                my_key = left_key ? my_leftkey : (right_key ? my_rightkey : KeyCode::Unknown);
                if(my_key != KeyCode::Unknown) {
                    RegisterKeyUp(alt_key);
                }
                break;
            }
            }
            key = ConvertKeyCodeToWinVK(my_key);
            RegisterKeyUp(key);
            return true;
        }
        case WindowsSystemMessage::Keyboard_SysKeyDown:
        {
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 1 for Alt while key pressed; 0 posted to active window if no window has keyboard focus
            //P: previous state 1 for already down
            //T: transition state 0 for SYSKEYDOWN
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            constexpr uint16_t keystate_state_mask = 0b1000'0000'0000'0000;  //0x8000
            constexpr uint16_t keystate_toggle_mask = 0b0000'0000'0000'0001; //0x0001
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            auto my_key = ConvertWinVKToKeyCode(key);
            if(my_key == KeyCode::Unknown) {
                return true;
            }
            if(is_extended_key) {
                switch(my_key) {
                    case KeyCode::Alt: //Right Alt
                    {
                        auto is_key_down = !!(::GetKeyState(key) & keystate_state_mask);
                        auto my_rightkey = KeyCode::RAlt;
                        auto alt_key = ConvertKeyCodeToWinVK(KeyCode::Alt);
                        my_key = is_key_down ? my_rightkey : KeyCode::Unknown;
                        if(my_key != KeyCode::Unknown) {
                            RegisterKeyDown(alt_key);
                        }
                        break;
                    }
                }
            }
            switch(my_key) {
                case KeyCode::Alt: //Left Alt
                {
                    auto is_key_down = !!(::GetKeyState(VK_LMENU) & keystate_state_mask);
                    auto my_leftkey = KeyCode::LAlt;
                    auto alt_key = ConvertKeyCodeToWinVK(KeyCode::Alt);
                    my_key = is_key_down ? my_leftkey : KeyCode::Unknown;
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyDown(alt_key);
                    }
                    break;
                }
                case KeyCode::F10:
                {
                    auto is_key_down = !!(::GetKeyState(VK_F10) & keystate_state_mask);
                    my_key = is_key_down ? KeyCode::F10 : KeyCode::Unknown;
                }
            }
            key = ConvertKeyCodeToWinVK(my_key);
            RegisterKeyDown(key);
            return true;
        }
        case WindowsSystemMessage::Keyboard_SysKeyUp:
        {
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 1 for Alt down while released; 0 if SYSKEYDOWN posted to active window because no window has keyboard focus.
            //P: previous state 1 for SYSKEYUP
            //T: transition state 1 for SYSKEYUP
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            constexpr uint16_t keystate_state_mask = 0b1000'0000'0000'0000;  //0x8000
            constexpr uint16_t keystate_toggle_mask = 0b0000'0000'0000'0001; //0x0001
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            auto my_key = ConvertWinVKToKeyCode(key);
            if(my_key == KeyCode::Unknown) {
                return true;
            }
            if(is_extended_key) {
                switch(my_key) {
                    case KeyCode::Alt: //Right Alt
                    {
                        auto is_key_up = !(!!(::GetKeyState(VK_RMENU) & keystate_state_mask));
                        auto my_rightkey = KeyCode::RAlt;
                        auto alt_key = ConvertKeyCodeToWinVK(KeyCode::Alt);
                        my_key = is_key_up ? my_rightkey : KeyCode::Unknown;
                        if(my_key != KeyCode::Unknown) {
                            RegisterKeyUp(alt_key);
                        }
                        break;
                    }
                }
            }
            switch(my_key) {
                case KeyCode::Alt: //Left Alt
                {
                    auto is_key_up = !(!!(::GetKeyState(VK_LMENU) & keystate_state_mask));
                    auto my_leftkey = KeyCode::LAlt;
                    auto alt_key = ConvertKeyCodeToWinVK(KeyCode::Alt);
                    my_key = is_key_up ? my_leftkey : KeyCode::Unknown;
                    if(my_key != KeyCode::Unknown) {
                        RegisterKeyUp(alt_key);
                    }
                    break;
                }
                case KeyCode::F10:
                {
                    auto is_key_up = !(!!(::GetKeyState(VK_F10) & keystate_state_mask));
                    my_key = is_key_up ? KeyCode::F10 : KeyCode::Unknown;
                }
            }
            key = ConvertKeyCodeToWinVK(my_key);
            RegisterKeyUp(key);
            return true;
        }
        case WindowsSystemMessage::Mouse_LButtonDown:
        {
            constexpr uint16_t lbutton_mask  = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask  = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask    = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask     = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask  = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(wp & lbutton_mask) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::LButton);
                RegisterKeyDown(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_LButtonUp:
        {
            constexpr uint16_t lbutton_mask  = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask  = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask    = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask     = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask  = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(!(wp & lbutton_mask)) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::LButton);
                RegisterKeyUp(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_RButtonDown:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(wp & rbutton_mask) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::RButton);
                RegisterKeyDown(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_RButtonUp:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(!(wp & rbutton_mask)) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::RButton);
                RegisterKeyUp(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_MButtonDown:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(wp & mbutton_mask) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::MButton);
                RegisterKeyDown(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_MButtonUp:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_mask = 0b0000'0000'0100'0000; //0x0040
            if(!(wp & mbutton_mask)) {
                unsigned char key = ConvertKeyCodeToWinVK(KeyCode::MButton);
                RegisterKeyUp(key);
                POINTS p = MAKEPOINTS(lp);
                _mouseDelta = _mouseCoords;
                _mouseCoords = Vector2(p.x, p.y);
                _mouseDelta = _mouseCoords - _mouseDelta;
                return true;
            }
        }
        case WindowsSystemMessage::Mouse_XButtonDown:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            constexpr uint16_t xbutton1_mask = 0b0000'0001; //0x0001
            constexpr uint16_t xbutton2_mask = 0b0000'0010; //0x0002
            auto buttons = GET_XBUTTON_WPARAM(wp);
            unsigned char key = ConvertKeyCodeToWinVK(KeyCode::XButton1);
            if(buttons & xbutton1_mask) {
                key = ConvertKeyCodeToWinVK(KeyCode::XButton1);
            }
            if(buttons & xbutton2_mask) {
                key = ConvertKeyCodeToWinVK(KeyCode::XButton2);
            }
            RegisterKeyDown(key);
            POINTS p = MAKEPOINTS(lp);
            _mouseDelta = _mouseCoords;
            _mouseCoords = Vector2(p.x, p.y);
            _mouseDelta = _mouseCoords - _mouseDelta;
            return true;
        }
        case WindowsSystemMessage::Mouse_XButtonUp:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            constexpr uint16_t xbutton1_mask = 0b0000'0001; //0x0001
            constexpr uint16_t xbutton2_mask = 0b0000'0010; //0x0002
            auto buttons = GET_XBUTTON_WPARAM(wp);
            unsigned char key = 0;
            if(buttons & xbutton1_mask) {
                key = ConvertKeyCodeToWinVK(KeyCode::XButton1);
            }
            if(buttons & xbutton2_mask) {
                key = ConvertKeyCodeToWinVK(KeyCode::XButton2);
            }
            RegisterKeyUp(key);
            POINTS p = MAKEPOINTS(lp);
            _mouseDelta = _mouseCoords;
            _mouseCoords = Vector2(p.x, p.y);
            _mouseDelta = _mouseCoords - _mouseDelta;
            return true;
        }
        case WindowsSystemMessage::Mouse_MouseMove:
        {
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            POINTS p = MAKEPOINTS(lp);
            _mouseDelta = _mouseCoords;
            _mouseCoords = Vector2(p.x, p.y);
            _mouseDelta = _mouseCoords - _mouseDelta;
            return true;
        }
        case WindowsSystemMessage::Mouse_MouseWheel:
        {
            constexpr uint16_t wheeldelta_mask = 0b1111'1111'0000'0000; //FF00
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            POINTS p = MAKEPOINTS(lp);
            _mouseDelta = _mouseCoords;
            _mouseCoords = Vector2(p.x, p.y);
            _mouseDelta = _mouseCoords - _mouseDelta;
            _mouseWheelPosition = GET_WHEEL_DELTA_WPARAM(wp);
            return true;
        }
        case WindowsSystemMessage::Mouse_MouseHWheel:
        {
            constexpr uint16_t wheeldelta_mask = 0b1111'1111'0000'0000; //FF00
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001; //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010; //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100; //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000; //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000; //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            POINTS p = MAKEPOINTS(lp);
            _mouseDelta = _mouseCoords;
            _mouseCoords = Vector2(p.x, p.y);
            _mouseDelta = _mouseCoords - _mouseDelta;
            _mouseWheelHPosition = GET_WHEEL_DELTA_WPARAM(wp);
            return true;
        }
    }
    return false;
}

void InputSystem::Initialize() {
    UpdateXboxConnectedState();
    std::ostringstream ss;
    ss << _connected_controller_count << " Xbox controllers detected!\n";
    DebuggerPrintf(ss.str().c_str());
}

void InputSystem::BeginFrame() {
    if(_connection_poll.CheckAndReset()) {
        UpdateXboxConnectedState();
    }
    for(int i = 0; i < _connected_controller_count; ++i) {
        _xboxControllers[i].Update(i);
    }
}

void InputSystem::Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) {
    /* DO NOTHING */
}

void InputSystem::Render() const {
    /* DO NOTHING */
}

void InputSystem::EndFrame() {
    _previousKeys = _currentKeys;
    _mouseWheelPosition = 0;
    _mouseWheelHPosition = 0;
}

bool InputSystem::WasAnyKeyPressed() const {
    for(KeyCode k = KeyCode::First_; k < KeyCode::Last_; ++k) {
        if(WasKeyJustPressed(k)) {
            return true;
        }
    }
    return false;
}

bool InputSystem::IsKeyUp(const KeyCode& key) const {
    return !_previousKeys[(std::size_t)key] && !_currentKeys[(std::size_t)key];
}

bool InputSystem::WasKeyJustPressed(const KeyCode& key) const {
    return !_previousKeys[(std::size_t)key] && _currentKeys[(std::size_t)key];
}

bool InputSystem::IsKeyDown(const KeyCode& key) const {
    return _previousKeys[(std::size_t)key] && _currentKeys[(std::size_t)key];
}

bool InputSystem::IsAnyKeyDown() const {
    for(KeyCode k = KeyCode::First_; k < KeyCode::Last_; ++k) {
        if(IsKeyDown(k)) {
            return true;
        }
    }
    return false;
}

bool InputSystem::WasKeyJustReleased(const KeyCode& key) const {
    return _previousKeys[(std::size_t)key] && !_currentKeys[(std::size_t)key];
}

bool InputSystem::WasMouseWheelJustScrolledUp() const {
    return GetMouseWheelPositionNormalized() > 0;
}

bool InputSystem::WasMouseWheelJustScrolledDown() const {
    return GetMouseWheelPositionNormalized() < 0;
}

bool InputSystem::WasMouseWheelJustScrolledLeft() const {
    return GetMouseWheelHorizontalPositionNormalized() < 0;
}

bool InputSystem::WasMouseWheelJustScrolledRight() const {
    return GetMouseWheelHorizontalPositionNormalized() > 0;
}

std::size_t InputSystem::GetConnectedControllerCount() const {
    int connected_count = 0;
    for(const auto& controller : _xboxControllers) {
        if(controller.IsConnected()) {
            ++connected_count;
        }
    }
    return connected_count;
}

bool InputSystem::IsAnyControllerConnected() const {
    bool result = false;
    for(const auto& controller : _xboxControllers) {
        result |= controller.IsConnected();
    }
    return result;
}

const XboxController& InputSystem::GetXboxController(const std::size_t& controllerIndex) const {
    return _xboxControllers[controllerIndex];
}

XboxController& InputSystem::GetXboxController(const std::size_t& controllerIndex) {
    return const_cast<XboxController&>(static_cast<const InputSystem&>(*this).GetXboxController(controllerIndex));
}
