#include "configuration.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Configuration w;
	w.show();
	return a.exec();
}
