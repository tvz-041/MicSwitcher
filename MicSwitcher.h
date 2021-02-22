#ifndef MICSWITCHER_H
#define MICSWITCHER_H

#include <QSystemTrayIcon>

class QAction;

class MicSwitcher : public QObject
{
	Q_OBJECT

public:
	enum IconStyle
	{
		Light = 0,
		Dark = 1
	};

	MicSwitcher();
	~MicSwitcher() = default;

public slots:
	void killTimer(); //inline
	inline void show();
	void showSettingsDialog();
	void disableMic(); //inline
	void enableMic(); //inline
	void switchMic();
	void setMicEnabled(const bool enabled);
	inline void setShowNotifications(const bool show = true);
	void setIconStyle(const IconStyle style);

protected:
	void timerEvent(QTimerEvent *event) override;

private slots:
	void onHotkeyPressed();
	void onHotkeyReleased();

	void onMicSwitched(const bool isMicEnabled);
	void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void initTrayContextMenu();
	QIcon iconWithCurrentStyle(const QString &iconPath);
	void updateTrayIcon();

	int m_timerId = 0;
	int m_timerInterval = 0;
	QSystemTrayIcon *m_trayIcon = nullptr;
	QAction *m_enableMicAcrion = nullptr;
	int m_hotkeyCounter = 0;
	int m_showNotifications = false;
	IconStyle m_iconStyle = Light;
	bool m_isMicEnabled;
};



//public slots:

inline void MicSwitcher::killTimer()
{
	if (m_timerId) {
		QObject::killTimer(m_timerId);
		m_timerId = 0;
	}
}

inline void MicSwitcher::show()
{
	m_trayIcon->show();
}

inline void MicSwitcher::setShowNotifications(const bool show)
{
	m_showNotifications = show;
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
