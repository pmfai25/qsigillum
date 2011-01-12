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

#ifndef PNNCLASSIFIER_H
#define PNNCLASSIFIER_H

#include <QtPlugin>
#include <QtGui>
#include <QImage>
#include <QString>
#include <QList>
#include <QtDebug>

#include <cmath>

#include "../../src/classifier.h"

// Probabilistic neural network digits classifier
class PNNClassifier : public QObject, public Classifier
{
Q_OBJECT
Q_INTERFACES(Classifier)

public:
	// Classify image data
	QString classify(QImage image);

	// Get base plugin name
	QString getBaseName();

private:
	// Deviation value
	double deviation;

	// Result threshold value
	double result_threshold;

	// Binarization threshold value
	int bin_threshold;

	// Grayscale color table
	QVector<QRgb> grayColorTable;

	// Image
	QImage image;

	// Weights vector
	QVector< QVector< QVector<double> > > weights;

	// Class labels
	QVector<QString> labels;

	// Initialize members
	void init();

	// Initialize color table
	void initColorTable();

	// Crop image
	QImage crop(QImage image);

	// Get points occupancy in north-east image quarter
	double getOccupancyQ1();

	// Get points occupancy in north-west image quarter
	double getOccupancyQ2();

	// Get points occupancy in south-west image quarter
	double getOccupancyQ3();

	// Get points occupancy in south-east image quarter
	double getOccupancyQ4();

	// Get points occupancy in top third part
	double getOccupancyVerticalT1();

	// Get points occupancy in middle third part
	double getOccupancyVerticalT2();

	// Get points occupancy in bottom third part
	double getOccupancyVerticalT3();

	// Get points occupancy in top fifth part
	double getOccupancyVerticalQv1();

	// Get points occupancy in second fifth part
	double getOccupancyVerticalQv2();

	// Get points occupancy in middle fifth part
	double getOccupancyVerticalQv3();

	// Get points occupancy in fourth fifth part
	double getOccupancyVerticalQv4();

	// Get points occupancy in bottom fifth part
	double getOccupancyVerticalQv5();

	// Get relative distance from left border to a black point
	// at height = 0%
	double getDistanceLeftBorder0p();

	// Get relative distance from left border to a black point
	// at height = 25%
	double getDistanceLeftBorder25p();

	// Get relative distance from left border to a black point
	// at height = 50%
	double getDistanceLeftBorder50p();

	// Get relative distance from left border to a black point
	// at height = 75%
	double getDistanceLeftBorder75p();

	// Get relative distance from left border to a black point
	// at height = 100%
	double getDistanceLeftBorder100p();

	// Get relative distance from right border to a black point
	// at height = 0%
	double getDistanceRightBorder0p();

	// Get relative distance from right border to a black point
	// at height = 25%
	double getDistanceRightBorder25p();

	// Get relative distance from right border to a black point
	// at height = 50%
	double getDistanceRightBorder50p();

	// Get relative distance from right border to a black point
	// at height = 75%
	double getDistanceRightBorder75p();

	// Get relative distance from right border to a black point
	// at height = 100%
	double getDistanceRightBorder100p();

	// Get relative distance from left border to a black point
	// at top fifth
	// OBSOLETE!!!
	double getDistanceLeftBorderQv1();

	// Get relative distance from left border to a black point
	// at second fifth
	// OBSOLETE!!!
	double getDistanceLeftBorderQv2();

	// Get relative distance from left border to a black point
	// at third fifth
	// OBSOLETE!!!
	double getDistanceLeftBorderQv3();

	// Get relative distance from left border to a black point
	// at fourth fifth
	// OBSOLETE!!!
	double getDistanceLeftBorderQv4();

	// Get relative distance from left border to a black point
	// at bottom fifth
	// OBSOLETE!!!
	double getDistanceLeftBorderQv5();

	// Get relative distance from right border to a black point
	// at top fifth
	// OBSOLETE!!!
	double getDistanceRightBorderQv1();

	// Get relative distance from right border to a black point
	// at second fifth
	// OBSOLETE!!!
	double getDistanceRightBorderQv2();

	// Get relative distance from right border to a black point
	// at third fifth
	// OBSOLETE!!!
	double getDistanceRightBorderQv3();

	// Get relative distance from right border to a black point
	// at fourth fifth
	// OBSOLETE!!!
	double getDistanceRightBorderQv4();

	// Get relative distance from right border to a black point
	// at bottom fifth
	// OBSOLETE!!!
	double getDistanceRightBorderQv5();

	// Get relative distance from top border to a black point
	// at leftmost fifth
	// OBSOLETE!!!
	double getDistanceTopBorderQv1();

	// Get relative distance from top border to a black point
	// at second fifth
	// OBSOLETE!!!
	double getDistanceTopBorderQv2();

	// Get relative distance from top border to a black point
	// at third fifth
	// OBSOLETE!!!
	double getDistanceTopBorderQv3();

	// Get relative distance from top border to a black point
	// at fourth fifth
	// OBSOLETE!!!
	double getDistanceTopBorderQv4();

	// Get relative distance from top border to a black point
	// at rightmost fifth
	// OBSOLETE!!!
	double getDistanceTopBorderQv5();

	// Get relative distance from bottom border to a black point
	// at leftmost fifth
	// OBSOLETE!!!
	double getDistanceBottomBorderQv1();

	// Get relative distance from bottom border to a black point
	// at second fifth
	// OBSOLETE!!!
	double getDistanceBottomBorderQv2();

	// Get relative distance from bottom border to a black point
	// at third fifth
	// OBSOLETE!!!
	double getDistanceBottomBorderQv3();

	// Get relative distance from bottom border to a black point
	// at fourth fifth
	// OBSOLETE!!!
	double getDistanceBottomBorderQv4();

	// Get relative distance from bottom border to a black point
	// at rightmost fifth
	// OBSOLETE!!!
	double getDistanceBottomBorderQv5();

	// Get occupancy in north-west image ninth part
	double getOccupancyNW();

	// Get occupancy in north-centre image ninth part
	double getOccupancyNC();

	// Get occupancy in north-east image ninth part
	double getOccupancyNE();

	// Get occupancy in centre-west image ninth part
	double getOccupancyCW();

	// Get occupancy in centre-centre image ninth part
	double getOccupancyCC();

	// Get occupancy in centre-east image ninth part
	double getOccupancyCE();

	// Get occupancy in south-west image ninth part
	double getOccupancySW();

	// Get occupancy in south-centre image ninth part
	double getOccupancySC();

	// Get occupancy in south-east image ninth part
	double getOccupancySE();

	// Convert image to grayscale format
	QImage grayscale(QImage src);

	// Train classifier
	void trainClassifier();

	// Check classifier for training set
	void checkClassifier();

	// Compensate rotation for digit image
	// Method is duplicated from Preprocessor and is used for training / testing
	QImage autoRotate(const QImage& image);

};

#endif // PNNCLASSIFIER_H
