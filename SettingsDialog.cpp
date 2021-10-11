#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog),
	m_settings(settings),
	m_initialSettings(settings)
{
	ui->setupUi(this);
	setWindowTitle("MicSwitch");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui->comboBox_setMicOnStartup->setCurrentIndex(m_initialSettings.m_micStateChangeOnStartup);
    ui->comboBox_switchMode->setCurrentIndex(m_initialSettings.m_switchMode);
	ui->comboBox_trayIconStyle->setCurrentIndex(m_initialSettings.m_trayIconStyle);
	ui->horizontalSlider_pushDelay->setValue(m_initialSettings.m_pushDelay);
	ui->label_pushDelayValue->setText(QString::number(m_initialSettings.m_pushDelay));
	ui->horizontalSlider_releaseDelay->setValue(m_initialSettings.m_releaseDelay);
	ui->label_releaseDelayValue->setText(QString::number(m_initialSettings.m_releaseDelay));
	ui->groupBox_notifications->setChecked(m_initialSettings.showNotifications());
	ui->checkBox_selfSwitchesShowNotification->setChecked(m_initialSettings.showNotificationsOnSelfSwitches());
	ui->checkBox_selfSwitchesPlaySound->setChecked(m_initialSettings.playSoundOnSelfSwitches());
	ui->checkBox_otherSwitchesShowNotification->setChecked(m_initialSettings.showNotificationsOnOtherSwitches());
	ui->checkBox_otherSwitchesPlaySound->setChecked(m_initialSettings.playSoundOnOtherSwitches());

	connect(ui->comboBox_setMicOnStartup, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](const int index) {
		m_settings.m_micStateChangeOnStartup = Settings::MicStateChange(index);
	});

    connect(ui->comboBox_switchMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](const int index) {
		emit switchModeChanged(Settings::SwitchMode(index));
	});

	connect(ui->comboBox_trayIconStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](const int index) {
		emit trayIconStyleChanged(Settings::IconStyle(index));
	});

	connect(ui->horizontalSlider_pushDelay, &QSlider::valueChanged, [this](const int delay) {
		ui->label_pushDelayValue->setText(QString::number(delay));
		emit pushDelayChanged(delay);
	});

	connect(ui->horizontalSlider_releaseDelay, &QSlider::valueChanged, [this](const int delay) {
		ui->label_releaseDelayValue->setText(QString::number(delay));
		emit releaseDelayChanged(delay);
	});

	connect(ui->groupBox_notifications, &QGroupBox::clicked, [this](const bool enabled) {
		m_settings.setShowNotifications(enabled);
	});

	connect(ui->checkBox_selfSwitchesShowNotification, &QCheckBox::clicked, [this](const bool enabled) {
		m_settings.setShowNotificationsOnSelfSwitches(enabled);
	});

	connect(ui->checkBox_selfSwitchesPlaySound, &QCheckBox::clicked, [this](const bool enabled) {
		m_settings.setPlaySoundOnSelfSwitches(enabled);
	});

	connect(ui->checkBox_otherSwitchesShowNotification, &QCheckBox::clicked, [this](const bool enabled) {
		m_settings.setShowNotificationsOnOtherSwitches(enabled);
	});

	connect(ui->checkBox_otherSwitchesPlaySound, &QCheckBox::clicked, [this](const bool enabled) {
		m_settings.setPlaySoundOnOtherSwitches(enabled);
	});

	connect(ui->pushButton_ok, &QPushButton::clicked, this, &SettingsDialog::accept);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, &SettingsDialog::reject);
	connect(ui->pushButton_quit, &QPushButton::clicked, &QApplication::quit);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::accept()
{
	QDialog::accept();
}

void SettingsDialog::reject()
{
	if (m_settings != m_initialSettings) {
		//Restoring old settings
		m_settings.m_micStateChangeOnStartup = m_initialSettings.m_micStateChangeOnStartup;
		emit switchModeChanged(m_initialSettings.m_switchMode);
		emit trayIconStyleChanged(m_initialSettings.m_trayIconStyle);
		emit pushDelayChanged(m_initialSettings.m_pushDelay);
		emit releaseDelayChanged(m_initialSettings.m_releaseDelay);
		m_settings.m_notificationsFlags = m_initialSettings.m_notificationsFlags;
	}

	QDialog::reject();
}

//private slots:

void SettingsDialog::setPushDelay(const int delay)
{
	ui->label_pushDelayValue->setText(QString::number(delay));
}

void SettingsDialog::setReleaseDelay(const int delay)
{
	ui->label_releaseDelayValue->setText(QString::number(delay));
}
