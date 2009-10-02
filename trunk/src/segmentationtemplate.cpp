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

#include "segmentationtemplate.h"

SegmentationTemplate::SegmentationTemplate()
{
	imageHeight = 0;
	imageWidth = 0;
}

SegmentationTemplate::~SegmentationTemplate()
{
	while (!body.isEmpty())
		delete body.takeFirst();
}

const QString& SegmentationTemplate::getTitle()
{
	const QString& value = title;
	return value;
}

void SegmentationTemplate::setTitle(const QString& title)
{
	this->title = title;
}

const QString& SegmentationTemplate::getAuthor()
{
	const QString& value = author;
	return value;
}

void SegmentationTemplate::setAuthor(const QString& author)
{
	this->author = author;
}

const QString& SegmentationTemplate::getDate()
{
	const QString& value = date;
	return value;
}

void SegmentationTemplate::setDate(const QString& date)
{
	this->date = date;
}

QList<TemplateContainer*>& SegmentationTemplate::getBody()
{
	QList<TemplateContainer*>& value = body;
	return value;
}

int SegmentationTemplate::getImageWidth()
{
	return imageWidth;
}

void SegmentationTemplate::setImageWidth(int width)
{
	imageWidth = width;
}

int SegmentationTemplate::getImageHeight()
{
	return imageHeight;
}

void SegmentationTemplate::setImageHeight(int height)
{
	imageHeight = height;
}

bool SegmentationTemplate::loadFromFile(const QString& fileName)
{
	 // First of all, we should check the input file
	 QFile file(fileName);
	 if (!file.open(QIODevice::ReadOnly))
		return false;

	// Read XML data; read Qt documentation for examples
	QXmlStreamReader reader(&file);
	while (!reader.atEnd())
	{
		reader.readNext();

		if (reader.error())
		{
			return false;
		}

		if (reader.isStartElement())
		{
			if (reader.name() == "document" ||
				reader.name() == "properties" ||
				reader.name() == "body")
				continue;

			if (reader.name() == "title")
			{
				reader.readNext();
				if (reader.isCharacters())
				{
					this->setTitle(reader.text().toString());
				}
				continue;
			}

			if (reader.name() == "author")
			{
				reader.readNext();
				if (reader.isCharacters())
				{
					this->setAuthor(reader.text().toString());
				}
				continue;
			}

			if (reader.name() == "date")
			{
				reader.readNext();
				if (reader.isCharacters())
				{
					this->setDate(reader.text().toString());
				}
				continue;
			}

			if (reader.name() == "image_width")
			{
				reader.readNext();
				if (reader.isCharacters())
				{
					this->setImageWidth(reader.text().toString().toInt());
				}
				continue;
			}

			if (reader.name() == "image_height")
			{
				reader.readNext();
				if (reader.isCharacters())
				{
					this->setImageHeight(reader.text().toString().toInt());
				}
				continue;
			}

			if (reader.name() == "container")
			{
				TemplateContainer * container = loadContainer(reader);
				if (container != NULL)
					this->body.append(container);

				continue;
			}
		}

	 }

	reader.clear();
	file.close();
	return true;
}

TemplateContainer * SegmentationTemplate::loadContainer
		(QXmlStreamReader& reader)
{
	TemplateContainer *container = new TemplateContainer();

	do
	{
		reader.readNext();

		if (!reader.isStartElement())
			continue;

		if (reader.name() == "x")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				container->setX(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "y")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				container->setY(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "width")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				container->setWidth(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "height")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				container->setHeight(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "interval")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				container->setInterval(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "field")
		{
			TemplateField * field = loadField(reader);
			if (field != NULL)
				container->getFields().append(field);

			continue;
		}

	} while (!(reader.isEndElement() &&
			 reader.name() == "container"));


	if (container->getWidth() == 0 &&
		container->getHeight() == 0)
	{
		delete container;
		return NULL;
	}
	else
		return container;
}

TemplateField * SegmentationTemplate::loadField(QXmlStreamReader& reader)
{
	TemplateField *field = new TemplateField();

	do
	{
		reader.readNext();

		if (!reader.isStartElement())
			continue;

		if (reader.name() == "x")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				field->setX(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "y")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				field->setY(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "width")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				field->setWidth(reader.text().toString().toInt());
			}
			continue;
		}

		if (reader.name() == "height")
		{
			reader.readNext();
			if (reader.isCharacters())
			{
				field->setHeight(reader.text().toString().toInt());
			}
			continue;
		}

	} while (!(reader.isEndElement() &&
			 reader.name() == "field"));


	if (field->getWidth() == 0 &&
		field->getHeight() == 0)
	{
		delete field;
		return NULL;
	}
	else
		return field;
}

void SegmentationTemplate::dumpData()
{
	qDebug() << "Author: " << getAuthor() << "\n" <<
			"Title: " << getTitle() << "\n" <<
			"Date: " << getDate() << "\n" <<
			"Dimensions: " << getImageWidth() << "x" <<
			getImageHeight() << "\n";

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
}
