#include <windows.h>

#include <QDebug>

#include "HotkeyHandler.h"

Hotkey hotkeyFromWinApi(const WORD key);
WORD hotkeyToWinApi(const Hotkey hotkey);
UINT hotkeyModifiersToWinApi(const Hotkey hotkey);
LRESULT CALLBACK mouseHookWinApi(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK keyboardHookWinApi(int nCode, WPARAM wParam, LPARAM lParam);

HotkeyHandler &HotkeyHandler::instance()
{
	static HotkeyHandler instance;
	return instance;
}

HotkeyHandler::HotkeyHandler()
{
	m_moduleHandler = GetModuleHandle(NULL);
}

HotkeyHandler::~HotkeyHandler()
{
	if (m_mouseHotkeysCount) {
		UnhookWindowsHookEx(m_mouseHook);
	}

	if (!m_keyboardHotkeys.isEmpty()) {
		UnhookWindowsHookEx(m_keyboardHook);
	}
}
//TODO: implement modifiers
bool HotkeyHandler::addHotkey(const Hotkey hotkey)
{
	if (!hotkey.isValid()) {
		return false;
	}

	if (hotkey.isMouse()) {
		switch (hotkey.mouseButton()) {
			case Qt::MiddleButton:
				if (!m_middleButton.isValid()) {
					++m_mouseHotkeysCount;
				}

				m_middleButton = hotkey;
			break;

			case Qt::BackButton:
				if (!m_backButton.isValid()) {
					++m_mouseHotkeysCount;
				}

				m_backButton = hotkey;
			break;

			case Qt::ForwardButton:
				if (!m_forwardButton.isValid()) {
					++m_mouseHotkeysCount;
				}

				m_forwardButton = hotkey;
			break;

			default:
			return false;
		}

		if (m_mouseHotkeysCount == 1) {
			m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookWinApi, m_moduleHandler, 0);
		}
	} else if (!m_keyboardHotkeys.contains(hotkey)) {
		if (m_keyboardHotkeys.isEmpty()) {
			m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookWinApi, m_moduleHandler, 0);
		}

		//TODO: remove this commented code if keyboard hook above will be suitable approach
		//(need to check ensure keyboard modifiers support)
//		if (!RegisterHotKey(HWND(winId()), 0, hotkeyModifiersToWinApi(hotkey), hotkeyToWinApi(hotkey))) {
//			return false;
//		}

		m_keyboardHotkeys.append(hotkey);
	}

	return true;
}

bool HotkeyHandler::removeHotkey(const Hotkey hotkey)
{
	if (hotkey.isMouse()) {
		if (!m_mouseHotkeysCount) {
			return false;
		}

		switch (hotkey.mouseButton()) {
			case Qt::MiddleButton:
				if (m_middleButton.isValid()) {
					--m_mouseHotkeysCount;
				}

				m_middleButton.reset();
			break;

			case Qt::BackButton:
				if (m_backButton.isValid()) {
					--m_mouseHotkeysCount;
				}

				m_backButton.reset();
			break;

			case Qt::ForwardButton:
				if (m_forwardButton.isValid()) {
					--m_mouseHotkeysCount;
				}

				m_forwardButton.reset();
			break;

			default:
			return false;
		}

		if (!m_mouseHotkeysCount) {
			UnhookWindowsHookEx(m_mouseHook);
		}

		return true;
	} else if (m_keyboardHotkeys.removeOne(hotkey)) {
		if (m_keyboardHotkeys.isEmpty()) {
			UnhookWindowsHookEx(m_keyboardHook);
		}

		return true;
	}

	return false;
}

//TODO: remove this commented code if keyboard hook above will be suitable approach
//(need to check ensure keyboard modifiers support)
//bool HotkeyHandler::nativeEvent(const QByteArray &/*eventType*/, void *message, long */*result*/)
//{

//	MSG *msg = static_cast<MSG *>(message);
//	if (msg->message == WM_HOTKEY) {
//		WORD key =  HIWORD(msg->lParam);

//		switch (key) {
//			default:
//				emit hotkeyPressed(hotkeyFromWinApi(key));
//			break;
//		}

//		return true;
//	}

//	return false;
//}

LRESULT CALLBACK mouseHookWinApi(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT *mouseStruct = (MSLLHOOKSTRUCT *)(lParam);

	if(mouseStruct != nullptr) {
		Hotkey hotkey;
		bool isPressed = true;

		switch (wParam) {
			case WM_MBUTTONUP:
				isPressed = false;
			Q_FALLTHROUGH();
			case WM_MBUTTONDOWN:
				hotkey = HotkeyHandler::instance().middleButtonHotkey();
			break;

			case WM_XBUTTONUP:
				isPressed = false;
			Q_FALLTHROUGH();
			case WM_XBUTTONDOWN:
				if (HIWORD(mouseStruct->mouseData) == 1) {
					hotkey = HotkeyHandler::instance().forwardButtonHotkey();
				} else {
					hotkey = HotkeyHandler::instance().backButtonHotkey();
				}
			break;

			default:
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		if (hotkey.isValid()) {
			if (isPressed) {
				HotkeyHandler::instance().hotkeyPressed(hotkey);
			} else {
				HotkeyHandler::instance().hotkeyReleased(hotkey);
			}

			return 1;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//TODO: implement
LRESULT CALLBACK keyboardHookWinApi(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *keyboardStruct = (KBDLLHOOKSTRUCT *)(lParam);

	if(keyboardStruct != nullptr) {
		Hotkey hotkey;
		bool isPressed = (wParam == WM_KEYDOWN);

		switch (keyboardStruct->vkCode) {
//			case VK_TAB:
//				qDebug() << "TAB";
//			break;

			default:
//				qDebug() << "default" << wParam << VK_TAB;
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		if (hotkey.isValid()) {
			if (isPressed) {
				HotkeyHandler::instance().hotkeyPressed(hotkey);
			} else {
				HotkeyHandler::instance().hotkeyReleased(hotkey);
			}

			return 1;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
//TODO: implement
Hotkey hotkeyFromWinApi(const WORD key)
{
	switch (key) {
//		case VK_LBUTTON:
//		return QKeySequence;
//		0x01
//		Left mouse button
//		VK_RBUTTON
//		0x02
//		Right mouse button
//		VK_CANCEL
//		0x03
//		Control-break processing
//		VK_MBUTTON
//		0x04
//		Middle mouse button (three-button mouse)
//		VK_XBUTTON1
//		0x05
//		X1 mouse button
//		VK_XBUTTON2
//		0x06
//		X2 mouse button
//		-
//		0x07
//		Undefined
//		VK_BACK
//		0x08
//		BACKSPACE key
//		VK_TAB
//		0x09
//		TAB key
//		-
//		0x0A-0B
//		Reserved
//		VK_CLEAR
//		0x0C
//		CLEAR key
//		VK_RETURN
//		0x0D
//		ENTER key
//		-
//		0x0E-0F
//		Undefined
//		VK_SHIFT
//		0x10
//		SHIFT key
//		VK_CONTROL
//		0x11
//		CTRL key
//		VK_MENU
//		0x12
//		ALT key
//		VK_PAUSE
//		0x13
//		PAUSE key
//		VK_CAPITAL
//		0x14
//		CAPS LOCK key
//		VK_KANA
//		0x15
//		IME Kana mode
//		VK_HANGUEL
//		0x15
//		IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
//		VK_HANGUL
//		0x15
//		IME Hangul mode
//		VK_IME_ON
//		0x16
//		IME On
//		VK_JUNJA
//		0x17
//		IME Junja mode
//		VK_FINAL
//		0x18
//		IME final mode
//		VK_HANJA
//		0x19
//		IME Hanja mode
//		VK_KANJI
//		0x19
//		IME Kanji mode
//		VK_IME_OFF
//		0x1A
//		IME Off
//		VK_ESCAPE
//		0x1B
//		ESC key
//		VK_CONVERT
//		0x1C
//		IME convert
//		VK_NONCONVERT
//		0x1D
//		IME nonconvert
//		VK_ACCEPT
//		0x1E
//		IME accept
//		VK_MODECHANGE
//		0x1F
//		IME mode change request
//		VK_SPACE
//		0x20
//		SPACEBAR
//		VK_PRIOR
//		0x21
//		PAGE UP key
//		VK_NEXT
//		0x22
//		PAGE DOWN key
//		VK_END
//		0x23
//		END key
//		VK_HOME
//		0x24
//		HOME key
//		VK_LEFT
//		0x25
//		LEFT ARROW key
//		VK_UP
//		0x26
//		UP ARROW key
//		VK_RIGHT
//		0x27
//		RIGHT ARROW key
//		VK_DOWN
//		0x28
//		DOWN ARROW key
//		VK_SELECT
//		0x29
//		SELECT key
//		VK_PRINT
//		0x2A
//		PRINT key
//		VK_EXECUTE
//		0x2B
//		EXECUTE key
//		VK_SNAPSHOT
//		0x2C
//		PRINT SCREEN key
//		VK_INSERT
//		0x2D
//		INS key
//		VK_DELETE
//		0x2E
//		DEL key
//		VK_HELP
//		0x2F
//		HELP key
//		0x30
//		0 key
//		0x31
//		1 key
//		0x32
//		2 key
//		0x33
//		3 key
//		0x34
//		4 key
//		0x35
//		5 key
//		0x36
//		6 key
//		0x37
//		7 key
//		0x38
//		8 key
//		0x39
//		9 key
//		-
//		0x3A-40
//		Undefined
//		0x41
//		A key
//		0x42
//		B key
//		0x43
//		C key
//		0x44
//		D key
//		0x45
//		E key
//		0x46
//		F key
//		0x47
//		G key
//		0x48
//		H key
//		0x49
//		I key
//		0x4A
//		J key
//		0x4B
//		K key
//		0x4C
//		L key
//		0x4D
//		M key
//		0x4E
//		N key
//		0x4F
//		O key
//		0x50
//		P key
//		0x51
//		Q key
//		0x52
//		R key
//		0x53
//		S key
//		0x54
//		T key
//		0x55
//		U key
//		0x56
//		V key
//		0x57
//		W key
//		0x58
//		X key
//		0x59
//		Y key
//		0x5A
//		Z key
//		VK_LWIN
//		0x5B
//		Left Windows key (Natural keyboard)
//		VK_RWIN
//		0x5C
//		Right Windows key (Natural keyboard)
//		VK_APPS
//		0x5D
//		Applications key (Natural keyboard)
//		-
//		0x5E
//		Reserved
//		VK_SLEEP
//		0x5F
//		Computer Sleep key
//		VK_NUMPAD0
//		0x60
//		Numeric keypad 0 key
//		VK_NUMPAD1
//		0x61
//		Numeric keypad 1 key
//		VK_NUMPAD2
//		0x62
//		Numeric keypad 2 key
//		VK_NUMPAD3
//		0x63
//		Numeric keypad 3 key
//		VK_NUMPAD4
//		0x64
//		Numeric keypad 4 key
//		VK_NUMPAD5
//		0x65
//		Numeric keypad 5 key
//		VK_NUMPAD6
//		0x66
//		Numeric keypad 6 key
//		VK_NUMPAD7
//		0x67
//		Numeric keypad 7 key
//		VK_NUMPAD8
//		0x68
//		Numeric keypad 8 key
//		VK_NUMPAD9
//		0x69
//		Numeric keypad 9 key
//		VK_MULTIPLY
//		0x6A
//		Multiply key
//		VK_ADD
//		0x6B
//		Add key
//		VK_SEPARATOR
//		0x6C
//		Separator key
//		VK_SUBTRACT
//		0x6D
//		Subtract key
//		VK_DECIMAL
//		0x6E
//		Decimal key
//		VK_DIVIDE
//		0x6F
//		Divide key
//		VK_F1
//		0x70
//		F1 key
//		VK_F2
//		0x71
//		F2 key
//		VK_F3
//		0x72
//		F3 key
//		VK_F4
//		0x73
//		F4 key
//		VK_F5
//		0x74
//		F5 key
//		VK_F6
//		0x75
//		F6 key
//		VK_F7
//		0x76
//		F7 key
//		VK_F8
//		0x77
//		F8 key
//		VK_F9
//		0x78
//		F9 key
//		VK_F10
//		0x79
//		F10 key
//		VK_F11
//		0x7A
//		F11 key
//		VK_F12
//		0x7B
//		F12 key
//		VK_F13
//		0x7C
//		F13 key
//		VK_F14
//		0x7D
//		F14 key
//		VK_F15
//		0x7E
//		F15 key
//		VK_F16
//		0x7F
//		F16 key
//		VK_F17
//		0x80
//		F17 key
//		VK_F18
//		0x81
//		F18 key
//		VK_F19
//		0x82
//		F19 key
//		VK_F20
//		0x83
//		F20 key
//		VK_F21
//		0x84
//		F21 key
//		VK_F22
//		0x85
//		F22 key
//		VK_F23
//		0x86
//		F23 key
//		VK_F24
//		0x87
//		F24 key
//		-
//		0x88-8F
//		Unassigned
//		VK_NUMLOCK
//		0x90
//		NUM LOCK key
//		VK_SCROLL
//		0x91
//		SCROLL LOCK key
//		0x92-96
//		OEM specific
//		-
//		0x97-9F
//		Unassigned
//		VK_LSHIFT
//		0xA0
//		Left SHIFT key
//		VK_RSHIFT
//		0xA1
//		Right SHIFT key
//		VK_LCONTROL
//		0xA2
//		Left CONTROL key
//		VK_RCONTROL
//		0xA3
//		Right CONTROL key
//		VK_LMENU
//		0xA4
//		Left MENU key
//		VK_RMENU
//		0xA5
//		Right MENU key
//		VK_BROWSER_BACK
//		0xA6
//		Browser Back key
//		VK_BROWSER_FORWARD
//		0xA7
//		Browser Forward key
//		VK_BROWSER_REFRESH
//		0xA8
//		Browser Refresh key
//		VK_BROWSER_STOP
//		0xA9
//		Browser Stop key
//		VK_BROWSER_SEARCH
//		0xAA
//		Browser Search key
//		VK_BROWSER_FAVORITES
//		0xAB
//		Browser Favorites key
//		VK_BROWSER_HOME
//		0xAC
//		Browser Start and Home key
//		VK_VOLUME_MUTE
//		0xAD
//		Volume Mute key
//		VK_VOLUME_DOWN
//		0xAE
//		Volume Down key
//		VK_VOLUME_UP
//		0xAF
//		Volume Up key
//		VK_MEDIA_NEXT_TRACK
//		0xB0
//		Next Track key
//		VK_MEDIA_PREV_TRACK
//		0xB1
//		Previous Track key
//		VK_MEDIA_STOP
//		0xB2
//		Stop Media key
//		VK_MEDIA_PLAY_PAUSE
//		0xB3
//		Play/Pause Media key
//		VK_LAUNCH_MAIL
//		0xB4
//		Start Mail key
//		VK_LAUNCH_MEDIA_SELECT
//		0xB5
//		Select Media key
//		VK_LAUNCH_APP1
//		0xB6
//		Start Application 1 key
//		VK_LAUNCH_APP2
//		0xB7
//		Start Application 2 key
//		-
//		0xB8-B9
//		Reserved
//		VK_OEM_1
//		0xBA
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the ';:' key
//		VK_OEM_PLUS
//		0xBB
//		For any country/region, the '+' key
//		VK_OEM_COMMA
//		0xBC
//		For any country/region, the ',' key
//		VK_OEM_MINUS
//		0xBD
//		For any country/region, the '-' key
//		VK_OEM_PERIOD
//		0xBE
//		For any country/region, the '.' key
//		VK_OEM_2
//		0xBF
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the '/?' key
//		VK_OEM_3
//		0xC0
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the '`~' key
//		-
//		0xC1-D7
//		Reserved
//		-
//		0xD8-DA
//		Unassigned
//		VK_OEM_4
//		0xDB
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the '[{' key
//		VK_OEM_5
//		0xDC
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the '\|' key
//		VK_OEM_6
//		0xDD
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the ']}' key
//		VK_OEM_7
//		0xDE
//		Used for miscellaneous characters; it can vary by keyboard.
//		For the US standard keyboard, the 'single-quote/double-quote' key
//		VK_OEM_8
//		0xDF
//		Used for miscellaneous characters; it can vary by keyboard.
//		-
//		0xE0
//		Reserved
//		0xE1
//		OEM specific
//		VK_OEM_102
//		0xE2
//		Either the angle bracket key or the backslash key on the RT 102-key keyboard
//		0xE3-E4
//		OEM specific
//		VK_PROCESSKEY
//		0xE5
//		IME PROCESS key
//		0xE6
//		OEM specific
//		VK_PACKET
//		0xE7
//		Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
//		-
//		0xE8
//		Unassigned
//		0xE9-F5
//		OEM specific
//		VK_ATTN
//		0xF6
//		Attn key
//		VK_CRSEL
//		0xF7
//		CrSel key
//		VK_EXSEL
//		0xF8
//		ExSel key
//		VK_EREOF
//		0xF9
//		Erase EOF key
//		VK_PLAY
//		0xFA
//		Play key
//		VK_ZOOM
//		0xFB
//		Zoom key
//		VK_NONAME
//		0xFC
//		Reserved
//		VK_PA1
//		0xFD
//		PA1 key
//		VK_OEM_CLEAR
//		0xFE
//		Clear key
	}

	return Hotkey();
}
//TODO: implement
WORD hotkeyToWinApi(const Hotkey hotkey)
{
	switch (hotkey.key()) {
		default:
		return 0;
	}
}

UINT hotkeyModifiersToWinApi(const Hotkey hotkey)
{
	return (hotkey.hasModifier(Qt::AltModifier) ? MOD_ALT : 0) |
		   (hotkey.hasModifier(Qt::ShiftModifier) ? MOD_SHIFT : 0) |
		   (hotkey.hasModifier(Qt::ControlModifier) ? MOD_CONTROL : 0);
}