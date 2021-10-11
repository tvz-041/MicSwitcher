#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "Settings.h"

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(Settings &settings, QWidget *parent = nullptr);
	~SettingsDialog();

	void accept() override;
	void reject() override;

signals:
    void switchModeChanged(Settings::SwitchMode newMode);
	void trayIconStyleChanged(Settings::IconStyle newStyle);
	void pushDelayChanged(int newDelay);
	void releaseDelayChanged(int newDelay);

private slots:
	void setPushDelay(const int delay);
	void setReleaseDelay(const int delay);

private:
	Ui::SettingsDialog *ui;

	Settings &m_settings;
	const Settings m_initialSettings;
};

#endif // SETTINGSDIALOG_H
