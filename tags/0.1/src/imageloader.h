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

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QtPlugin>

class QImage;
class QString;
class QIcon;
class QStatusBar;

class ImageLoader
{
public:

	virtual ~ImageLoader() {}

	// Load image data
	virtual QImage loadImage() = 0;

	// Get translation file base name
	virtual QString getTranslationFileBaseName() = 0;

	// Get icon for menu
	virtual QIcon getMenuIcon() = 0;

	// Get image loading action description for menu
	virtual QString getLoadingDescription() = 0;

	// Get image loading & processing action description for menu
	virtual QString getLoadingProcessingDescription() = 0;

	// Set status bar for updating GUI info
	virtual void setStatusBar(QStatusBar *) = 0;

signals:
	virtual void activatedLoading() = 0;
	virtual void activatedAutomatedProcessing() = 0;
};

Q_DECLARE_INTERFACE(ImageLoader,"qSigillum.ImageLoader/0.2");


#endif // IMAGELOADER_H
