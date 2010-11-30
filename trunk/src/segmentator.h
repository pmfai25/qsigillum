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

#ifndef SEGMENTATOR_H
#define SEGMENTATOR_H

#include <QtGui>
#include <QString>
#include <QImage>
#include <QtDebug>

#include "segmentationtemplate.h"
#include "templatecontainer.h"
#include "preprocessor.h"

class Segmentator : public QObject
{

public:
    Segmentator();
	~Segmentator();
	void loadTemplate(const QString & fileName);
	void setImage(QImage * image);
	void setPreprocessor(Preprocessor * preprocessor);
	void segmentate();
	// Get real anchor coordinates
	QPoint getRealAnchor();
	// Get real container anchor coordinates
	QPoint getRealContainerAnchor(int raw_x, int raw_y);

	const QList<TemplateContainer *> & getBody();
	// Remove container from body list
	void removeBodyElement(int element);
	// Duplicate container using element number
	void duplicateBodyElement(int element);

private:
	// Preprocessor instance
	Preprocessor * preprocessor;
	// Segmentation template instance
	SegmentationTemplate * segTemplate;
	// Source document image
	QImage * image;
	// RGB version of document image for difference calculation
	QImage rgbImage;
	// Segmentation results
	QList<TemplateContainer *> body;

	// Image width size used for scaling
	const int scale_width;
	// Maximum deviation
	const unsigned int max_deviation;
	// Region size for second pass analysis
	const int region_size;

	// Real-to-suspected image width ratio
	double width_ratio;
	// Real-to-suspected image height ratio
	double height_ratio;

	// Real anchor coordinates
	QPoint anchor;
	// Anchor change flag
	bool anchorFound;

	// Check if source image line is empty
	bool emptyLine(int y);
	// Dump body for debug purposes
	void dumpData();
	// Search for real anchor coordinates
	void searchAnchor();

	// New version of segmentation routine
	void segmentateNew();

};

#endif // SEGMENTATOR_H
