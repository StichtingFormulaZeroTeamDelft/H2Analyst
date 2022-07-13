#pragma once

#include <iostream>

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStandardPaths>
#include <QDir>

class SettingsManager : public QObject
{

	Q_OBJECT

	QString m_SettingsFolderPath;
	QDir m_SettingsFolder;

	void checkFolders();

public:
	SettingsManager();
	
	void resetAllToDefault();

};

