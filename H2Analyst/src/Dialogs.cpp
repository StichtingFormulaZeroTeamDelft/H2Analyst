#include "Dialogs.h"


/**
* Creates a dialog pop-up with the given message.
* Can be used to provide information to the user.
**/
void H2A::Dialog::message(QString message) {
	QMessageBox msgBox;
	msgBox.setWindowFlags(Qt::CustomizeWindowHint);
	msgBox.setText(message);
	msgBox.exec();
}


/**
* Creates a pop-up dialog with a yes/no question.
* Return true if answered yes. Otherwise it returns false.
* 
* @param question Question to display.
**/
bool H2A::Dialog::question(QString question) {
	QMessageBox msgBox;
	msgBox.setModal(true);
	msgBox.setText(question);
	msgBox.setWindowFlags(Qt::CustomizeWindowHint);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	int result = msgBox.exec();
	return result == QMessageBox::Yes;
}

