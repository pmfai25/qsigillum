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

#ifndef SEGMENTATIONTEMPLATE_H
#define SEGMENTATIONTEMPLATE_H

#include <QObject>
#include <QList>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QDebug>

#include "templatecontainer.h"

class SegmentationTemplate : public QObject
{
public:
    SegmentationTemplate();
	~SegmentationTemplate();

	const QString& getTitle();
	void setTitle(const QString& title);

	const QString& getAuthor();
	void setAuthor(const QString& author);

	const QString& getDate();
	void setDate(const QString& date);

	QList<TemplateContainer*>& getBody();

	int getImageWidth();
	void setImageWidth(int width);

	int getImageHeight();
	void setImageHeight(int height);

	// Try to load template from file
	bool loadFromFile(const QString& fileName);

	// Output for debug purposes
	void dumpData();

private:
	QString title;
	QString author;
	QString date;
	// Image normalization size
	int imageWidth;
	int imageHeight;

	// Template body
	QList<TemplateContainer*> body;

	// Parse a container definition part of xml file
	TemplateContainer * loadContainer(QXmlStreamReader& reader);

	// Parse a field definition part of xml file
	TemplateField * loadField(QXmlStreamReader& reader);
};

#endif // SEGMENTATIONTEMPLATE_H
