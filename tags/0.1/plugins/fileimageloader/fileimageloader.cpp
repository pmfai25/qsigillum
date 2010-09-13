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

#include "fileimageloader.h"

// Load image data
QImage FileImageLoader::loadImage()
{
	// Get file name via a dialog
	QString fileName = QFileDialog::getOpenFileName(0,
		tr("Open image file"),"../data/");
	if (fileName.length() <= 0)
		return QImage();

	if (statusBar)
		statusBar->showMessage(tr("Loading image: ") + fileName);

	return QImage(fileName);
}

// Get icon for menu
QIcon FileImageLoader::getMenuIcon()
{
	return QIcon(":/document-open.png");
}

// Get image loading action description for menu
QString FileImageLoader::getLoadingDescription()
{
	return tr("Open image file");
}

// Get image loading & processing action description for menu
QString FileImageLoader::getLoadingProcessingDescription()
{
	return tr("Open and process image file");
}

// Get translation file base name
QString FileImageLoader::getTranslationFileBaseName()
{
	return QString("fileimageloader");
}

// Set status bar for updating GUI info
void FileImageLoader::setStatusBar(QStatusBar * statusBar)
{
	this->statusBar = statusBar;
}

Q_EXPORT_PLUGIN2(fileimageloader, FileImageLoader)
