#include "SettingsManager.h"


/**
* Class that manages writing and reading to settings files.
**/
SettingsManager::SettingsManager() : QObject() {

	this->checkFolders();

}

/**
* Makes sure all necessary folders exist.
**/
void SettingsManager::checkFolders() {
	// If settings folder is not set, set it to default location
	if (m_SettingsFolderPath.isEmpty()) {
		/*
		* Standard data folders:
		* Windows: %appdata%/H2Analyst/
		* Mac: ? (tbd)
		* Linux: ? (tbd)
		*/
		m_SettingsFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
		m_SettingsFolder = QDir(m_SettingsFolderPath);

		// Check if settings folder exists and if not, create it
		if (!m_SettingsFolder.exists()) {
			std::cout << "Application folder does not exist. Creating it: " << m_SettingsFolderPath.toStdString() << std::endl;
			m_SettingsFolder.mkpath(m_SettingsFolderPath);
		}

		// Check if subfolder 'Settings' exists
		if (!m_SettingsFolder.exists("Settings")) {
			std::cout << "Settings folder does not exist. Making it in " << m_SettingsFolderPath.toStdString() << std::endl;
			m_SettingsFolder.mkdir("Settings");
		}
	}
}


/**
* Overrides all currently existing files to the ones that were packaged with the application.
**/
void SettingsManager::resetAllToDefault() {
	this->checkFolders();
}