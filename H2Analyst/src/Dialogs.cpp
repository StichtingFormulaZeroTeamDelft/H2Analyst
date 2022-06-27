#include "Dialogs.h"


/**
* Creates a dialog pop-up with the given message.
* Can be used to provide information to the user.
**/
void H2A::Dialog::message(QString message) {
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.exec();
}


/**
*
**/
bool H2A::Dialog::question(QString message) {
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	int result = msgBox.exec();
	return result == QMessageBox::Yes;
}
