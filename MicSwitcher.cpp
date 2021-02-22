#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QTimer>

#include "HotkeyHandler.h"
#include "MicHandler.h"
#include "SettingsDialog.h"

#include "MicSwitcher.h"

MicSwitcher::MicSwitcher()
{
	QApplication::setWindowIcon(QIcon(":/Resources/Icons/MicMutedDark"));

	m_trayIcon = new QSystemTrayIcon(this);
	initTrayContextMenu();

	connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MicSwitcher::onTrayIconActivated);

	connect(&MicHandler::instance(), &MicHandler::micSwitched, this, &MicSwitcher::onMicSwitched);

	connect(&HotkeyHandler::instance(), &HotkeyHandler::hotkeyPressed, this, &MicSwitcher::onHotkeyPressed);
	connect(&HotkeyHandler::instance(), &HotkeyHandler::hotkeyReleased, this, &MicSwitcher::onHotkeyReleased);

	//TODO: add setting "disable/enable micro at startup
	setMicEnabled(false);

	//FIXME: remove code below
	m_timerInterval = 300;
	HotkeyHandler::instance().addHotkey(Hotkey(Qt::BackButton));
	HotkeyHandler::instance().addHotkey(Hotkey(Qt::ForwardButton));
}

//public slots:

void MicSwitcher::showSettingsDialog()
{
	//TODO: implement settings dialog
//	SettingsDialog dialog;
//	dialog.exec();
}

void MicSwitcher::setMicEnabled(const bool enabled)
{
	MicHandler::instance().setMicEnabled(enabled);
	onMicSwitched(enabled);
}

void MicSwitcher::setIconStyle(const IconStyle style)
{
	if (m_iconStyle != style) {
		m_iconStyle = style;
		updateTrayIcon();
	}
}

//protected:

void MicSwitcher::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timerId) {
		m_timerId = 0;
		setMicEnabled(false);
	}

	QObject::killTimer(event->timerId());
	QObject::timerEvent(event);
}

//private slots:

void MicSwitcher::onHotkeyPressed()
{
	killTimer();
	++m_hotkeyCounter;

	if (m_hotkeyCounter == 1) {
		if (m_showNotifications) {
			m_trayIcon->showMessage(tr("Microphone Enabled"), "", iconWithCurrentStyle(":/Resources/Icons/MicUnmuted"));
		}

		setMicEnabled(true);
	}
}

void MicSwitcher::onHotkeyReleased()
{
	--m_hotkeyCounter;

	if (m_hotkeyCounter == 0) {
		if (m_showNotifications) {
			m_trayIcon->showMessage(tr("Microphone Disabled"), "", iconWithCurrentStyle(":/Resources/Icons/MicMuted"));
		}

		if (m_timerInterval) {
			killTimer();
			m_timerId = QObject::startTimer(m_timerInterval);
		} else {
			setMicEnabled(false);
		}
	}
}

void MicSwitcher::onMicSwitched(const bool isMicEnabled)
{
	m_isMicEnabled = isMicEnabled;
	updateTrayIcon();
	m_enableMicAcrion->setText(m_isMicEnabled ? tr("Disable microphone") : tr("Enable microphone"));
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

void MicSwitcher::initTrayContextMenu()
{
	QMenu *trayContextMenu = new QMenu();
	QMenu *menu;
	QAction *action;

	m_enableMicAcrion = trayContextMenu->addAction("", this,  &MicSwitcher::switchMic);

	action = trayContextMenu->addAction(tr("Show notifications"), this,  &MicSwitcher::setShowNotifications);
	action->setCheckable(true);

	menu = trayContextMenu->addMenu(tr("Icon Style"));
	menu->addAction(tr("Light"), [this](){setIconStyle(Light);});
	menu->addAction(tr("Dark"), [this](){setIconStyle(Dark);});

	//TODO: implement settings dialog
	//trayContextMenu->addAction(tr("Settings"), this,  &MicSwitcher::showSettingsDialog);

	trayContextMenu->addAction(tr("Quit"), &QApplication::quit);

	m_trayIcon->setContextMenu(trayContextMenu);
}

QIcon MicSwitcher::iconWithCurrentStyle(const QString &iconPath)
{
	switch (m_iconStyle) {
		case Light:
		return QIcon(iconPath + "Light");

		case Dark:
		return QIcon(iconPath + "Dark");
	}

	return QIcon();
}

void MicSwitcher::updateTrayIcon()
{
	if (m_isMicEnabled) {
		m_trayIcon->setIcon(iconWithCurrentStyle(":/Resources/Icons/MicUnmuted"));
	} else {
		m_trayIcon->setIcon(iconWithCurrentStyle(":/Resources/Icons/MicMuted"));
	}
}
