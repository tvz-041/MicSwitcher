#ifndef MICSWITCHER_H
#define MICSWITCHER_H

#include <QSystemTrayIcon>

#include "Settings.h"

namespace std
{
    template<typename _Signature> class function;
}

class QAction;

class MicSwitcher : public QObject
{
	Q_OBJECT

public:
	MicSwitcher();
    ~MicSwitcher();

    void loadSettings();

    void startTimer(const int msecs, std::function<void()> timeoutFunction);
    void stopTimer();

public slots:
	inline void show();
	void showSettingsDialog();
	void disableMic(); //inline
	void enableMic(); //inline
	void switchMic();
	void setMicEnabled(const bool enabled);
    void setOverrideVolumeOnSwitch(const bool override = true);
	inline void setShowNotifications(const bool show = true);
	void setTrayIconStyle(const Settings::IconStyle style);

private slots:
	void onHotkeyPressed();
	void onHotkeyReleased();

	void onMicSwitched(const bool isMicEnabled);
    void onMicVolumeChanged(const float micVolume);
	void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void initTray();
	QIcon iconWithCurrentStyle(const QString &iconPath);
	void updateTrayIcon();

    QTimer *m_timer = nullptr;
    QMetaObject::Connection m_timerConnection;
    int m_timerId = 0;
	QSystemTrayIcon *m_trayIcon = nullptr;
    QAction *m_enableMicAction = nullptr;
    QAction *m_overrideVolumeAction = nullptr;
    QAction *m_showNotificationsAction = nullptr;
    int m_hotkeyCounter = 0;
    bool m_isMicEnabled;

	Settings m_settings;
};



//public slots:

inline void MicSwitcher::show()
{
	m_trayIcon->show();
}

inline void MicSwitcher::setShowNotifications(const bool show)
{
	m_settings.setUseNotifications(show);
}

inline void MicSwitcher::disableMic()
{
	setMicEnabled(false);
}

inline void MicSwitcher::enableMic()
{
	setMicEnabled(true);
}

inline void MicSwitcher::switchMic()
{
	setMicEnabled(!m_isMicEnabled);
}

#endif // MICSWITCHER_H
