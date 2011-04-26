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

#ifndef NUMBERCLASSIFIER_H
#define NUMBERCLASSIFIER_H

#include <QtPlugin>
#include <QtGui>
#include <QImage>
#include <QString>
#include <QList>
#include <QtDebug>

#include "../../src/classifier.h"

// Digit glyph parameters
struct GlyphParameters
{
	// Number of dark fields
	int count;
	// Size of last light field
	int last;
	// Sizes of dark fields
	QVector<int> dark;
	// Sizes of light fields
	QVector<int> light;
};

class NumberClassifier : public QObject, public Classifier
{
Q_OBJECT
Q_INTERFACES(Classifier)

public:
	// Classify image data
	QString classify(QImage image);

	// Get base plugin name
	QString getBaseName();

private:
	// Initialize structure
	GlyphParameters newParam();

	// Initialize members
	void init();

	// Find bounding box
	void getBoundingBox();

	// Get line parameters
	GlyphParameters getParameters(int y);

	// Upper digit part processing
	void processUpperPart();

	// Check for digit "1"
	void check1();

	// Basic check for digit "6"
	void check6();

	// Check for digit "4"
	void check4();

	// Check for digit "0"
	void check0();

	// Check for digit "3"
	void check3();

	// Check for digit "8"
	void check8();

	// Check for digit "2"
	void check2();

	// Full check for digit "6"
	void fullCheck6();

	// Check for digit "9"
	void check9();

	// Check for digit "5"
	void check5();

	// Check for digit "7"
	void check7();

	// Check classifier for training set
	void checkClassifier();

	// Compensate rotation for digit image
	// Method is duplicated from Preprocessor and is used for training / testing
	QImage autoRotate(const QImage& image);

	// Initialize color table
	void initColorTable();

	// Image
	QImage image;

	// Control flow label
	QString control_flow;

	// Dark color threshold
	int color;

	// Current horizontal coordinate
	int x;

	// Dark fields bounding box coordinates
	int x_min, y_min, x_max, y_max;

	// Bounding box height
	int h;

	// Vertical verges
	int verge1, verge2;

	// Output results criteria
	int result[10];

	// Feature variables
	int feature_strongly_double, feature_double, feature_single;
	int feature_vertical, feature_horizontal, feature_strongly_horizontal;
	int feature_line_distance;

	// Additional flag
	int flag;
	// Max size of last light field
	int max_last;

	// Last light fields sizes
	QVector<int> lasts;

	// Grayscale image color table
	QVector<QRgb> grayColorTable;

};

#endif // NUMBERCLASSIFIER_H
