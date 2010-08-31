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

#ifndef SANEIMAGELOADER_H
#define SANEIMAGELOADER_H

#include <QtPlugin>
#include <QtGui>
#include <QImage>
#include <QString>
#include <QIcon>
#include <QtDebug>
#include <QStatusBar>

#include "../../src/imageloader.h"
#include <sane/sane.h>

class SaneImageLoader : public QObject, public ImageLoader
{
Q_OBJECT
Q_INTERFACES(ImageLoader)

public:
	// Load image data
	QImage loadImage();

	// Get translation file base name
	QString getTranslationFileBaseName();

	// Get icon for menu
	QIcon getMenuIcon();

	// Get image loading action description for menu
	QString getLoadingDescription();

	// Get image loading & processing action description for menu
	QString getLoadingProcessingDescription();

	// Set status bar for updating GUI info
	void setStatusBar(QStatusBar * statusBar);

private:
	// Main window status bar
	QStatusBar * statusBar;

signals:
	void activatedLoading();
	void activatedAutomatedProcessing();
};

#endif // SANEIMAGELOADER_H
