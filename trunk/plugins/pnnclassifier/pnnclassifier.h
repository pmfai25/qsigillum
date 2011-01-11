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

	// Image
	QImage image;

	// Weights vector
	QVector< QVector< QVector<double> > > weights;

	// Class labels
	QVector<QString> labels;

	// Initialize members
	void init();

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

	// Convert image to grayscale format
	QImage grayscale(QImage src);

	// Train classifier
	void trainClassifier();

	// Check classifier for training set
	void checkClassifier();

};

#endif // PNNCLASSIFIER_H
