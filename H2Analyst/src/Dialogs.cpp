#include "Dialogs.h"


void H2A::Dialog::message(QString message)
{
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.exec();
}


