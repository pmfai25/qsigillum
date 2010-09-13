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

#include "saneimageloader.h"

// Load image data
QImage SaneImageLoader::loadImage()
{
	SANE_Handle h;
	// Init SANE and check status
	SANE_Status status = sane_init(NULL, NULL);

	qDebug() << "Sane status: " << sane_strstatus(status);

	// Get SANE devices
	SANE_Device **device_list;
	sane_get_devices(const_cast<const SANE_Device ***>(&device_list), 1);

	if (!device_list[0])
	{
		// Finish work with SANE
		sane_exit();
		return QImage();
	}

	qDebug() << device_list[0]->name << ": " << device_list[0]->model;
	status = sane_open(device_list[0]->name, &h);

	qDebug() << "Sane status: " << sane_strstatus(status);

//	// Find mode option
//	int n;
//	const SANE_Option_Descriptor *desc;
//	sane_control_option(h, 0, SANE_ACTION_GET_VALUE, &n, NULL);
//
//	for (int i = 0; i < n; i++)
//	{
//		desc = sane_get_option_descriptor(h, i);
//		if (desc && strncmp(desc->name, "mode", 4) == 0)
//		{
//			char value[] = "Gray";
//			sane_control_option(h, i, SANE_ACTION_SET_VALUE, value, NULL);
//			break;
//		}
//	}

	// Start retrieving image
	status = sane_start(h);

	// Get current parameters
	SANE_Parameters p;
	status = sane_get_parameters(h, &p);
	qDebug() << "Sane status: " << sane_strstatus(status);

	qDebug() << "format: " << p.format;
	qDebug() << "lines: " << p.lines;
	qDebug() << "bpl: " << p.bytes_per_line;
	qDebug() << "depth: " << p.depth;
	qDebug() << "ppl: " << p.pixels_per_line;

	// Construct QImage
	QImage result(p.pixels_per_line, p.lines, QImage::Format_RGB32);

	// Buffer
	long int size = p.bytes_per_line * p.lines;
	unsigned char* memory = (unsigned char *) malloc(size);

	// Real read chunk size
	int l = 0;
	int x = 0;
	int y = 0;

	// Read data chunk by chunk and convert result to QImage
	while ((status = sane_read(h, (unsigned char *)memory, size, &l))
		== SANE_STATUS_GOOD)
	{
		for (int i = 0; i < l; i+=3)
		{
			result.setPixel(x, y, qRgb(memory[i], memory[i+1], memory[i+2]));

			x++;
			if ( x >= p.pixels_per_line)
			{
				y++;
				x = 0;
			}
		}
	}

	free(memory);

	sane_close(h);

	// Finish work with SANE
	sane_exit();

	result.save(QString("trash.jpg"));

	return result;
}

// Get icon for menu
QIcon SaneImageLoader::getMenuIcon()
{
	return QIcon(":/scanner.png");
}

// Get image loading action description for menu
QString SaneImageLoader::getLoadingDescription()
{
	return tr("Retrieve image from scanner");
}

// Get image loading & processing action description for menu
QString SaneImageLoader::getLoadingProcessingDescription()
{
	return tr("Retrieve and process image from scanner");
}

// Get translation file base name
QString SaneImageLoader::getTranslationFileBaseName()
{
	return QString("saneimageloader");
}

// Set status bar for updating GUI info
void SaneImageLoader::setStatusBar(QStatusBar * statusBar)
{
	this->statusBar = statusBar;
}


Q_EXPORT_PLUGIN2(saneimageloader, SaneImageLoader)
