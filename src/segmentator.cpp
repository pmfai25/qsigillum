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

#include "segmentator.h"

Segmentator::Segmentator()
{
	segTemplate = NULL;
	while (!body.isEmpty())
		delete body.takeFirst();
	width_ratio = 1;
	height_ratio = 1;
}

Segmentator::~Segmentator()
{
	delete segTemplate;
	while (!body.isEmpty())
		delete body.takeFirst();
}

void Segmentator::loadTemplate(const QString & fileName)
{
	while (!body.isEmpty())
		delete body.takeFirst();
	delete segTemplate;

	segTemplate = new SegmentationTemplate();
	segTemplate->loadFromFile(fileName);
}

void Segmentator::setImage(QImage * image)
{
	this->image = image;

	// Set ratio
	if (image != NULL
		&& image->width() > 0 && image->height() > 0
		&& segTemplate != NULL
		&& segTemplate->getImageWidth() > 0
		&& segTemplate->getImageHeight() > 0)
	{
		width_ratio = double(image->width()) / segTemplate->getImageWidth();
		height_ratio = double(image->height()) / segTemplate->getImageHeight();
	}
}

void Segmentator::segmentate()
{
	if (!segTemplate || !image)
		return;

	while (!body.isEmpty())
	 delete body.takeFirst();

	// Scanning zone height
	int scan_height = (qRound(image->height() * 0.003) > 0)
					  ? qRound(image->height() * 0.003) : 1;

	// Now we should iterate through containers
	foreach (TemplateContainer * srcContainer, segTemplate->getBody())
	{
		// Check for emptiness and add one container if necessary
		TemplateContainer * temp = new TemplateContainer(srcContainer);

		bool isNotEmpty = false;

		// Scanning borders
		int b1 = (temp->getY() - scan_height >= 0 )
				 ? (temp->getY() - scan_height) : 0 ;
		int b2 = (temp->getY() + scan_height < image->height())
				 ? (temp->getY() + scan_height)
				 : (image->height() - 1);

		// Local scanning
		for (int y = b1; y <= b2 ; y++)
		if (!emptyLine(y))
		{
			isNotEmpty = true;
			break;
		}

		if (isNotEmpty)
		{
			body.append(temp);
		}
		else
			delete temp;

		// Check next container rows
		if (srcContainer->getInterval() >= 0)
		{
			// Data analysis success
			bool success;

			int i = srcContainer->getY() + srcContainer->getHeight()
					+ srcContainer->getInterval();

			do
			{
				success = false;

				// Scanning borders
				int b1 = i;
				int b2 = (i + scan_height < image->height() )
						 ? (i + scan_height) : image->height() - 1;

				// Local scanning
				for (int y = b1; y <= b2 ; y++)
				{
					// Check for non-empty region
					if (!emptyLine(y))
					{
						success = true;
						i = y - 1;
						break;
					}
				}

				if (!success)
					break;

				// Appending template row
				TemplateContainer * temp = new TemplateContainer(srcContainer);
				temp->setY(i);
				body.append(temp);

				// Incrementing vertical coordinate
				i += srcContainer->getHeight() + srcContainer->getInterval();

			} while (success);

		}
	}

}

// Check if source image line is empty
bool Segmentator::emptyLine(int y)
{
	if (!image)
		return true;

	int threshold = 200;

	for (int j = 0; j < image->width(); j++)
		if (qGray(image->pixel(j, y)) <= threshold)
			return false;

	return true;
}

// Read part of image based on interest zones from template
QImage Segmentator::getInterestPart()
{
	if (segTemplate != NULL
		&& segTemplate->getBody().size() > 0)
	{
		int x = segTemplate->getBody().at(0)->getX();
		int y = segTemplate->getBody().at(0)->getY();
		int w = segTemplate->getBody().at(0)->getWidth();
		int h = segTemplate->getBody().at(0)->getHeight();
		return image->copy(x, y, w, h);
	}
	else
		return QImage();
}

const QList<TemplateContainer *> & Segmentator::getBody()
{
	const QList<TemplateContainer *> & value = body;
	return value;
}

void Segmentator::dumpData()
{
	foreach (TemplateContainer * container, body)
	{
		qDebug() << "Container " << container->getWidth() << "x" <<
				container->getHeight() << " at (" <<
				container->getX() << ";" << container->getY() <<
				") / interval" << container->getInterval();

		foreach (TemplateField * field, container->getFields())
		{
			qDebug() << "\t Field " << field->getWidth() << "x" <<
				field->getHeight() << " at (" <<
				field->getX() << ";" << field->getY() << ")";
		}
	}

	qDebug() << "Total: " << body.length() << " containers";
}

