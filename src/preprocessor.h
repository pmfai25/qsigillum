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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <cmath>

#include <QObject>
#include <QApplication>
#include <QImage>
#include <QPoint>
#include <QQueue>
#include <QDebug>
#include <QVector>

class Preprocessor : public QObject
{
public:
    Preprocessor();

	// Remove dark fields from image borders
	QImage removeDarkFields(const QImage& image);
	// Remove trash from image borders
	QImage removeBorderTrash(const QImage& image);

	// Check if image column is empty
	bool emptyColumn(const QImage& image, int x);
	// Check if image row is empty
	bool emptyRow(const QImage& image, int y);

	// Calculate coordinates of center of mass of binary image
	QPoint getMassCenter(const QImage& image);

	// Binarization (8-bit image with 2 colors is used)
	QImage binarize(const QImage& image);
	// Transform to 8-bit image with 255 colors
	QImage grayscale(const QImage& image);

	// Morphological dilation
	QImage dilate(const QImage& image);
	// Morphological erosion
	QImage erode(const QImage& image);

	// Connected components analysis on binary images
	int* markCC(const QImage& image, int* number);
	// Analyse connected components
	QVector< QVector<int> > analyseComponents(const QImage& image, int* marked, int number);
	// Analyse specified component using given info vector
	QVector<int> analyseComponent(const QImage& image, int* marked, QVector<int> field);

	// Compensate rotation for digit image
	QImage autoRotate(const QImage& image);

	// Hit-and-miss transform using given 3x3 structuring element
	QImage hitAndMiss(const QImage& image, const int element[3][3]);

	// Thinning operation for a greyscale image
	QImage thin(const QImage& image);

	// Pruning operation for a greyscale image
	QImage prune(const QImage& image);

	// Thinning structuring element: two elements with 4 rotation variants
	static const int thinningElement[8][3][3];

	// Pruning structuring element: two elements with 4 rotation variants
	static const int pruningElement[8][3][3];

private:
	// Minimum mean value of non-dark field
	int light_threshold;
	// Maximum size of dark field
	double field_size;
	// Threshold size of empty field
	double empty_field_size;
	// Empty column critical value
	int empty_threshold;

	// Grayscale image color table
	QVector<QRgb> grayColorTable;

};

#endif // PREPROCESSOR_H
