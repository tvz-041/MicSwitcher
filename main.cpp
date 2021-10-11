#include <QApplication>
#include <QStyle>

#include "SettingsDialog.h"
#include "MicSwitcher.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);

	MicSwitcher switcher;
	switcher.show();

	return app.exec();
}
