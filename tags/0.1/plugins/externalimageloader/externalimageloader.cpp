/*
 *	QSigillum - handwritten digits recognition for structured documents
 *  Copyright 2009 Konstantin "konst" Kucher <konst.hex@gmail.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtPlugin>

#include "externalimageloader.h"

// Load image data
QImage ExternalImageLoader::loadImage()
{
	QImage result;

	// Configuration file loading
	QSettings settings("../res/externalimageloader.ini", QSettings::IniFormat);

	// Get external program, redirection and result file names
	QString program = settings.value("program").toString();
	QString redirection = settings.value("redirection").toString();
	QString filename = settings.value("filename").toString();

	if (program.isEmpty() || filename.isEmpty())
		return result;

	// Ensure program has started
	QProcess process;
	if (!redirection.isEmpty())
		process.setStandardOutputFile(redirection);
	process.start(program);
	if (!process.waitForStarted())
		return result;

	// Get timeout value
	int timeout = settings.value("timeout").toInt();
	QTime timer;
	timer.start();

	// Check for program exit or timeout event
	while (process.state() != QProcess::NotRunning)
	{
		if (timeout > 0 && timer.elapsed() > timeout)
		{
			// Stop process
			process.kill();
			// Try to delete file
			QDir dir;
			dir.remove(filename);
			// Return null image
			return result;
		}

		qApp->processEvents();
	}

	// Ensure program has finished correctly
	if (process.exitStatus() != QProcess::NormalExit)
		return result;

	// Try to read and remove the output file
	result = QImage(filename);
	QDir dir;
	dir.remove(filename);
	return result;
}

// Get icon for menu
QIcon ExternalImageLoader::getMenuIcon()
{
	return QIcon(":/scanner.png");
}

// Get image loading action description for menu
QString ExternalImageLoader::getLoadingDescription()
{
	return tr("Retrieve image from scanner");
}

// Get image loading & processing action description for menu
QString ExternalImageLoader::getLoadingProcessingDescription()
{
	return tr("Retrieve and process image from scanner");
}

// Get translation file base name
QString ExternalImageLoader::getTranslationFileBaseName()
{
	return QString("externalimageloader");
}

// Set status bar for updating GUI info
void ExternalImageLoader::setStatusBar(QStatusBar * statusBar)
{
	this->statusBar = statusBar;
}

Q_EXPORT_PLUGIN2(externalimageloader, ExternalImageLoader)
