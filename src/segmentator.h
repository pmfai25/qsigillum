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

#ifndef SEGMENTATOR_H
#define SEGMENTATOR_H

#include <QtGui>
#include <QString>
#include <QImage>
#include <QtDebug>

#include "segmentationtemplate.h"
#include "templatecontainer.h"

class Segmentator : public QObject
{

public:
    Segmentator();
	~Segmentator();
	void loadTemplate(const QString & fileName);
	void setImage(QImage * image);
	void segmentate();
	const QList<TemplateContainer *> & getBody();

private:
	SegmentationTemplate *segTemplate;
	// Source document image
	QImage * image;
	// Segmentation results
	QList<TemplateContainer *> body;

	// Calculate threshold value
	bool containerNotEmpty(TemplateContainer * container);
	// Dump body for debug purposes
	void dumpData();

};

#endif // SEGMENTATOR_H
