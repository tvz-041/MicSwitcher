#include <functional>

#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QTimer>

#include "HotkeyHandler.h"
#include "MicHandler.h"
#include "Settings.h"
#include "SettingsDialog.h"

#include "MicSwitcher.h"

//TODO: implement
//class RestartableTimer : public QTimer
//{
//public:
//    void start(void (*timeoutFunction)());
//    void start(const int msecs, void (*timeoutFunction)());
//    void stop();
//private:
//    QMetaObject::Connection m_connection;
//};

MicSwitcher::MicSwitcher() :
    m_timer(new QTimer(this)),
    m_settings(new QSettings(QString("settings.ini"), QSettings::IniFormat, this))
{
    m_timer->setSingleShot(true);
	QApplication::setWindowIcon(QIcon(":/Resources/Icons/MicMutedDark"));

	initTray();

	connect(&MicHandler::instance(), &MicHandler::micSwitched, this, &MicSwitcher::onMicSwitched);
    connect(&MicHandler::instance(), &MicHandler::micVolumeChanged, this, &MicSwitcher::onMicVolumeChanged);

	connect(&HotkeyHandler::instance(), &HotkeyHandler::hotkeyPressed, this, &MicSwitcher::onHotkeyPressed);
	connect(&HotkeyHandler::instance(), &HotkeyHandler::hotkeyReleased, this, &MicSwitcher::onHotkeyReleased);

    //FIXME: remove code block below
    {
        HotkeyHandler::instance().addHotkey(Hotkey(Qt::BackButton));
        HotkeyHandler::instance().addHotkey(Hotkey(Qt::ForwardButton));
    }

    loadSettings();

    switch(m_settings.micStateChangeOnStartup()) {
        case Settings::MicStateChange::SetEnable:
            enableMic();
        break;

        case Settings::MicStateChange::SetDisable:
            disableMic();
        break;

        default:
        break;
    }
}

void MicSwitcher::loadSettings()
{
    setMicEnabled(m_settings.isMicEnabled());
    setOverrideVolume(m_settings.overrideVolume());
    setShowNotifications(m_settings.showNotifications());
    setTrayIconStyle(m_settings.trayIconStyle());
}

void MicSwitcher::loadSettings(const Settings &settings)
{
    if (m_settings != settings) {
        m_settings = settings;
        loadSettings();
    }
}

MicSwitcher::~MicSwitcher()
{
    stopTimer();
    m_settings.saveToDisk();
}

void MicSwitcher::startTimer(const int msecs, std::function<void()> timeoutFunction)
{
    stopTimer();

    if (msecs < 0 || timeoutFunction == nullptr) {
        return;
    }

    if (msecs == 0) {
        timeoutFunction();
    } else {
        m_timerConnection = connect(m_timer, &QTimer::timeout, timeoutFunction);
        m_timer->start(msecs);
    }
}

void MicSwitcher::stopTimer()
{
    m_timer->stop();

    if (m_timerConnection) {
        disconnect(m_timerConnection);
    }
}

//public slots:

void MicSwitcher::showSettingsDialog()
{
	//TODO: implement settings dialog
    SettingsDialog dialog(m_settings);

	connect(&dialog, &SettingsDialog::trayIconStyleChanged, this, &MicSwitcher::setTrayIconStyle);

	dialog.exec();
}

inline void MicSwitcher::switchMic()
{
    setMicEnabled(!m_settings.isMicEnabled());
}

void MicSwitcher::setMicEnabled(const bool enabled)
{
    MicHandler::instance().setMicEnabled(enabled);
    onMicSwitched(enabled);
}

void MicSwitcher::setOverrideVolume(const bool override)
{
    m_settings.setOverrideVolume(override);
    m_overrideVolumeAction->setChecked(m_settings.overrideVolume());
    onMicVolumeChanged(MicHandler::instance().micVolume());
}

void MicSwitcher::setShowNotifications(const bool show)
{
    m_settings.setShowNotifications(show);
    m_showNotificationsAction->setChecked(show);
}

void MicSwitcher::setTrayIconStyle(const Settings::IconStyle style)
{
    m_settings.setTrayIconStyle(style);
    updateTrayIcon();
}

//private slots:

void MicSwitcher::onHotkeyPressed()
{
    stopTimer();
	++m_hotkeyCounter;

	if (m_hotkeyCounter == 1) {
        if (m_settings.showNotifications()) {
			m_trayIcon->showMessage(tr("Microphone Enabled"), "", iconWithCurrentStyle(":/Resources/Icons/MicUnmuted"));
		}

        startTimer(m_settings.pushDelay(), [this](){
            setMicEnabled(true);
        });
	}
}

void MicSwitcher::onHotkeyReleased()
{
	--m_hotkeyCounter;

	if (m_hotkeyCounter == 0) {
        if (m_settings.showNotifications()) {
			m_trayIcon->showMessage(tr("Microphone Disabled"), "", iconWithCurrentStyle(":/Resources/Icons/MicMuted"));
		}

        startTimer(m_settings.releaseDelay(), [this](){
            setMicEnabled(false);
        });
	}
}

void MicSwitcher::onMicSwitched(const bool isMicEnabled)
{
    m_settings.setMicEnabled(isMicEnabled);
	updateTrayIcon();
    m_enableMicAction->setText(isMicEnabled ? tr("Disable microphone") : tr("Enable microphone"));
}

void MicSwitcher::onMicVolumeChanged(const float micVolume)
{
    if (m_settings.isNeedToOverrideVolume(micVolume)) {
        MicHandler::instance().setMicVolume(m_settings.micVolume());
    }
}

void MicSwitcher::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::ActivationReason::DoubleClick:
			showSettingsDialog();
		break;

		default:
		break;
	}
}

//private:

void MicSwitcher::initTray()
{
	m_trayIcon = new QSystemTrayIcon(this);
	updateTrayIcon();

	QMenu *trayContextMenu = new QMenu();
    QMenu *menu;

    m_enableMicAction = trayContextMenu->addAction("", this,  &MicSwitcher::switchMic);

    m_overrideVolumeAction = trayContextMenu->addAction(tr("Override volume"), this,
                                        &MicSwitcher::setOverrideVolume);
    m_overrideVolumeAction->setCheckable(true);

    m_showNotificationsAction = trayContextMenu->addAction(tr("Show notifications"), this,
                                        &MicSwitcher::setShowNotifications);
    m_showNotificationsAction->setCheckable(true);

	menu = trayContextMenu->addMenu(tr("Icon Style"));
	menu->addAction(tr("Light"), [this](){setTrayIconStyle(Settings::IconStyle::Light);});
	menu->addAction(tr("Dark"), [this](){setTrayIconStyle(Settings::IconStyle::Dark);});

	//TODO: implement settings dialog
	//trayContextMenu->addAction(tr("Settings"), this,  &MicSwitcher::showSettingsDialog);

	trayContextMenu->addAction(tr("Quit"), &QApplication::quit);
	trayContextMenu->addAction(tr("Restart"), []{
		QProcess::startDetached(QApplication::arguments()[0], QApplication::arguments());
		QApplication::quit();
	});

	m_trayIcon->setContextMenu(trayContextMenu);

	connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MicSwitcher::onTrayIconActivated);
}

QIcon MicSwitcher::iconWithCurrentStyle(const QString &iconPath)
{
    switch (m_settings.trayIconStyle()) {
		case Settings::IconStyle::Light:
		return QIcon(iconPath + "Light");

		case Settings::IconStyle::Dark:
		return QIcon(iconPath + "Dark");
	}

	return QIcon();
}

void MicSwitcher::updateTrayIcon()
{
    if (m_settings.isMicEnabled()) {
		m_trayIcon->setIcon(iconWithCurrentStyle(":/Resources/Icons/MicUnmuted"));
	} else {
		m_trayIcon->setIcon(iconWithCurrentStyle(":/Resources/Icons/MicMuted"));
	}
}
