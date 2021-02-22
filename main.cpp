#include <QApplication>
#include <QStyle>

#include "SettingsDialog.h"
#include "MicSwitcher.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);

	MicSwitcher switcher;
	switcher.show();

	return a.exec();
}
