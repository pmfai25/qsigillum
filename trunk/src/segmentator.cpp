/*
 *	QSigillum - handwritten digits recognition for structured documents
 *  Copyright 2009 Konstantin "konst" Kucher <konst.hex@gmail.com>,
 *  Miroslav "sid" Sidorov <grans.bwa@gmail.com>
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
}

Segmentator::~Segmentator()
{
	delete segTemplate;
}

void Segmentator::loadTemplate(const QString & fileName)
{
	delete segTemplate;

	segTemplate = new SegmentationTemplate();
	segTemplate->loadFromFile(fileName);
}

void Segmentator::setImage(QImage * image)
{
	this->image = image;
}

void Segmentator::segmentate()
{
	if (!segTemplate || !image)
		return;

	while (!body.isEmpty())
	 delete body.takeFirst();

	// First of all, we should scale image to template defined' size
	(*image) = image->scaled(segTemplate->getImageWidth(),
							 segTemplate->getImageHeight(),
							 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	qDebug() << "Scaled image";

	// Now we should iterate through containers
	foreach (TemplateContainer * srcContainer, segTemplate->getBody())
	{
		// Check for emptiness and add one container if necessary
		//body.append(new TemplateContainer(srcContainer));
		TemplateContainer * temp = new TemplateContainer(srcContainer);

		if (containerNotEmpty(temp))
		{
			body.append(temp);
		}
		else
			delete temp;

		if (srcContainer->getInterval() >= 0)
		{
			bool isNotEmpty = false;
			int i = 0;

			do
			{
				++i;
				// If container is repeatable, we should check it
				TemplateContainer * temp = new TemplateContainer(srcContainer);

				// If we have run out of image, we obviously have to stop
				if (temp->getY() + (temp->getHeight()
						   + temp->getInterval())*i >= image->width())
					break;

				temp->setY(temp->getY() + (temp->getHeight()
						   + temp->getInterval())*i);
				isNotEmpty = containerNotEmpty(temp);
				if (isNotEmpty)
				{
					body.append(temp);
				}
				else
					delete temp;

			} while (isNotEmpty);

		}
	}

	dumpData();
}

const QList<TemplateContainer *> & Segmentator::getBody()
{
	const QList<TemplateContainer *> & value = body;
	return value;
}

bool Segmentator::containerNotEmpty(TemplateContainer * container)
{
	double thresholdValue = 0.1;

	foreach (TemplateField * field, container->getFields())
	{
		// Calculate average value
		double fieldSize = static_cast<double>(
				field->getWidth()*field->getHeight());
		if (fieldSize == 0.0)
			continue;

		int blackPixels = 0;

		for (int x = container->getX()+field->getX(); x < container->getX()+
					 field->getX()+field->getWidth(); x++ )
		{
			for (int y = container->getY()+field->getY(); y < container->getY()+
					 field->getY()+field->getHeight(); y++ )
			{
				if (qGray(image->pixel(x,y)) <= 200)
					blackPixels++;
			}
		}

		if (static_cast<double>(blackPixels)/fieldSize >= thresholdValue)
			return true;
	}

	return false;
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

