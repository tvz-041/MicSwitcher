#ifndef HOTKEYHANDLER_H
#define HOTKEYHANDLER_H

#include <QWidget>

#include "Hotkey.h"

struct HHOOK__;
struct HINSTANCE__;
typedef HHOOK__* HHOOK;

class Hotkey;

class HotkeyHandler : public QObject
{
	Q_OBJECT

public:
	static HotkeyHandler &instance();
	~HotkeyHandler();

	bool addHotkey(const Hotkey hotkey);
	bool removeHotkey(const Hotkey hotkey);

	inline Hotkey middleButtonHotkey() const;
	inline Hotkey backButtonHotkey() const;
	inline Hotkey forwardButtonHotkey() const;

signals:
	void hotkeyPressed(const Hotkey hotkey);
	void hotkeyReleased(const Hotkey hotkey);

protected:
	//TODO: remove this commented code if keyboard hook above will be suitable approach
	//(need to check ensure keyboard modifiers support)
	//bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
	HotkeyHandler();
	HotkeyHandler(const HotkeyHandler &other) = delete;
	HotkeyHandler &operator=(const HotkeyHandler &other) = delete;

	QVector<Hotkey> m_keyboardHotkeys;
	Hotkey m_middleButton;
	Hotkey m_backButton;
	Hotkey m_forwardButton;
	int m_mouseHotkeysCount = 0;

	HHOOK m_mouseHook;
	HHOOK m_keyboardHook;
	HINSTANCE__ *m_moduleHandler;
};



inline Hotkey HotkeyHandler::middleButtonHotkey() const
{
	return m_middleButton;
}

inline Hotkey HotkeyHandler::backButtonHotkey() const
{
	return m_backButton;
}

inline Hotkey HotkeyHandler::forwardButtonHotkey() const
{
	return m_forwardButton;
}

#endif // HOTKEYHANDLER_H
