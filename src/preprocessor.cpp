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

#include "preprocessor.h"

Preprocessor::Preprocessor()
{
	// Some useful variables

	// Dark fields removal
	light_threshold = 240;
	field_size = 0.25;

	// Empty column checking
	empty_threshold = 200;

	// Trash removal
	empty_field_size = 0.1;
}

// Remove dark fields from image borders
QImage Preprocessor::removeDarkFields(const QImage& image)
{
	if (image.isNull())
	{
		qDebug() << "Preprocessor got empty image";
		return QImage();
	}

	QPoint p1, p2;
	long int temp;

	// Loop from left
	for (int j = 0; j <= qRound(image.width() * field_size); j++)
	{
		p1.setX(j);
		temp = 0;
		// Check column mean value
		for (int i = 0; i < image.height(); i++)
			temp += qGray(image.pixel(j, i));
		if (qRound(double(temp) / image.height()) > light_threshold)
			break;
	}

	// Loop from right
	for (int j = image.width() - 1;
	j >= image.width() - qRound(image.width() * field_size); j--)
	{
		p2.setX(j);
		temp = 0;
		// Check column mean value
		for (int i = 0; i < image.height(); i++)
			temp += qGray(image.pixel(j, i));
		if (qRound(double(temp) / image.height()) > light_threshold)
			break;
	}

	// Loop from top
	for (int i = 0; i <= qRound(image.height() * field_size); i++)
	{
		p1.setY(i);
		temp = 0;
		// Check row mean value
		for (int j = 0; j < image.width(); j++)
			temp += qGray(image.pixel(j, i));
		if (qRound(double(temp) / image.width()) > light_threshold)
			break;
	}

	// Loop from bottom
	for (int i = image.height() - 1;
	i >= image.height() - qRound(image.height() * field_size); i--)
	{
		p2.setY(i);
		temp = 0;
		// Check row mean value
		for (int j = 0; j < image.width(); j++)
			temp += qGray(image.pixel(j, i));
		if (qRound(double(temp) / image.width()) > light_threshold)
			break;
	}

	// Copy resulting image
	return image.copy(p1.x(), p1.y(), p2.x()-p1.x()+1, p2.y()-p1.y()+1);
}

// Remove trash from image borders
QImage Preprocessor::removeBorderTrash(const QImage& image)
{
	if (image.isNull())
	{
		qDebug() << "Preprocessor got empty image";
		return QImage();
	}

	QPoint p1, p2;
	long int temp;
	bool gotBlack, gotWhite;

	// Loop from left
	gotBlack = false; gotWhite = false;
	for (int j = 0; j <= qRound(image.width() * field_size); j++)
	{
		p1.setX(j);
		temp = 0;
		// Check column values
		for (int i = 0; i < image.height(); i++)
			if (qGray(image.pixel(j, i)) <= empty_threshold)
			temp++;

		// Got black column
		if (temp >= qRound(image.height() * field_size) && !gotBlack)
		{
			gotBlack = true;
			continue;
		}

		// Got empty column
		if (temp < qRound(image.height() * empty_field_size) && gotBlack)
		{
			gotWhite = true;
			break;
		}
	}
	if (!gotWhite)
		p1.setX(0);

	// Loop from right
	gotBlack = false; gotWhite = false;
	for (int j = image.width() - 1;
	j >= image.width() - qRound(image.width() * field_size); j--)
	{
		p2.setX(j);
		temp = 0;
		// Check column values
		for (int i = 0; i < image.height(); i++)
			if (qGray(image.pixel(j, i)) <= empty_threshold)
			temp++;

		// Got black column
		if (temp >= qRound(image.height() * field_size) && !gotBlack)
		{
			gotBlack = true;
			continue;
		}

		// Got empty column
		if (temp < qRound(image.height() * empty_field_size) && gotBlack)
		{
			gotWhite = true;
			break;
		}
	}
	if (!gotWhite)
		p2.setX(image.width() - 1);

	// Loop from top
	gotBlack = false; gotWhite = false;
	for (int i = 0; i <= qRound(image.height() * field_size); i++)
	{
		p1.setY(i);
		temp = 0;
		// Check row values
		for (int j = 0; j < image.width(); j++)
			if (qGray(image.pixel(j, i)) <= empty_threshold)
			temp++;

		// Got black column
		if (temp >= qRound(image.width() * field_size) && !gotBlack)
		{
			gotBlack = true;
			continue;
		}

		// Got empty column
		if (temp < qRound(image.width() * empty_field_size) && gotBlack)
		{
			gotWhite = true;
			break;
		}
	}
	if (!gotWhite)
		p1.setY(0);

	// Loop from bottom
	gotBlack = false; gotWhite = false;
	for (int i = image.height() - 1;
	i >= image.height() - qRound(image.height() * field_size); i--)
	{
		p2.setY(i);
		temp = 0;
		// Check row values
		for (int j = 0; j < image.width(); j++)
			if (qGray(image.pixel(j, i)) <= empty_threshold)
			temp++;

		// Got black column
		if (temp >= qRound(image.width() * field_size) && !gotBlack)
		{
			gotBlack = true;
			continue;
		}

		// Got empty column
		if (temp < qRound(image.width() * empty_field_size) && gotBlack)
		{
			gotWhite = true;
			break;
		}
	}
	if (!gotWhite)
		p2.setY(image.height() - 1);

	// Copy resulting image
	return image.copy(p1.x(), p1.y(), p2.x()-p1.x()+1, p2.y()-p1.y()+1);
}

// Check if image column is empty
bool Preprocessor::emptyColumn(const QImage& image, int x)
{
	for (int i = 0; i < image.height(); i++)
		if (qGray(image.pixel(x, i)) <= empty_threshold)
			return false;

	return true;
}

// Check if image row is empty
bool Preprocessor::emptyRow(const QImage& image, int y)
{
	for (int j = 0; j < image.width(); j++)
		if (qGray(image.pixel(j, y)) <= empty_threshold)
			return false;

	return true;
}

// Binarization
QImage Preprocessor::binarize(const QImage& image)
{
	QImage result = image.copy();

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		// Check pixel value
		if (qGray(result.pixel(j, i)) <= empty_threshold)
			result.setPixel(j, i, qRgb(0, 0, 0));
		else
			result.setPixel(j, i, qRgb(255, 255, 255));
	}

	return result;
}

// Connected components analysis
QImage Preprocessor::markCC(const QImage& image, int* number)
{
	QImage result = image.copy();
	// Accessory image
	QImage map(result.size(), QImage::Format_Mono);
	map.fill(0);

	// Components number
	int n = 0;
	// Current label
	int current_label = 1;
	// Marking queue
	QQueue<QPoint> queue;

	int x = 0;
	int y = 0;

	// Search for unmarked pixels
	for (int j = 0; j < result.width(); j++)
	for (int i = 0; i < result.height(); i++)
	{
		// Check pixel occupation
		if (map.pixelIndex(j,i) == 1)
			continue;

		// Check pixel value
		if (qGray(result.pixel(j, i)) <= empty_threshold)
		{
			// Append pixel
			queue.enqueue(QPoint(j, i));

			// Process component
			while (!queue.isEmpty())
			{
				QPoint point = queue.dequeue();
				x = point.x();
				y = point.y();
				if (map.pixelIndex(point) == 1)
					continue;

				// Mark pixel
				result.setPixel(point, qRgb(current_label, 0, 0));
				map.setPixel(point, 1);

				// Check neighbours
				if (x > 0 && map.pixelIndex(x-1, y) == 0
					&& qGray(result.pixel(x-1, y)) <= empty_threshold)
					queue.enqueue(QPoint(x-1, y));
				if (x > 0 && y > 0 && map.pixelIndex(x-1, y-1) == 0
					&& qGray(result.pixel(x-1, y-1)) <= empty_threshold)
					queue.enqueue(QPoint(x-1, y-1));
				if (y > 0 && map.pixelIndex(x, y-1) == 0
					&& qGray(result.pixel(x, y-1)) <= empty_threshold)
					queue.enqueue(QPoint(x, y-1));
				if (x < result.width() - 1 && y > 0 && map.pixelIndex(x+1, y-1) == 0
					&& qGray(result.pixel(x+1, y-1)) <= empty_threshold)
					queue.enqueue(QPoint(x+1, y-1));
				if (x < result.width() - 1 && map.pixelIndex(x+1, y) == 0
					&& qGray(result.pixel(x+1, y)) <= empty_threshold)
					queue.enqueue(QPoint(x+1, y));
				if (x < result.width() - 1 && y < result.height() - 1
					&& map.pixelIndex(x+1, y+1) == 0
					&& qGray(result.pixel(x+1, y+1)) <= empty_threshold)
					queue.enqueue(QPoint(x+1, y+1));
				if (y < result.height() - 1
					&& map.pixelIndex(x, y+1) == 0
					&& qGray(result.pixel(x, y+1)) <= empty_threshold)
					queue.enqueue(QPoint(x, y+1));
				if (x > 0 && y < result.height() - 1
					&& map.pixelIndex(x-1, y+1) == 0
					&& qGray(result.pixel(x-1, y+1)) <= empty_threshold)
					queue.enqueue(QPoint(x-1, y+1));
			}

			// Update label and components number
			n++;
			current_label++;
		}
		else
		{
			result.setPixel(j, i, qRgb(255, 255, 255));
			map.setPixel(j, i, 1);
		}
	}


	// Set number
	if (number)
		*number = n;
	return result;
}

// Morphological dilation
QImage Preprocessor::dilate(const QImage& image)
{
	QImage result(image.size(), image.format());
	result.fill(qRgb(255, 255, 255));

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		if (qGray(image.pixel(j, i)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j > 0 && qGray(image.pixel(j-1, i)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j > 0 && i > 0 && qGray(image.pixel(j-1, i-1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (i > 0 && qGray(image.pixel(j, i-1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j < image.width() - 1 && i > 0 && qGray(image.pixel(j+1, i-1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j < image.width() - 1 && qGray(image.pixel(j+1, i)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j < image.width() - 1 && i < image.height() - 1 && qGray(image.pixel(j+1, i+1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (i < image.height() - 1 && qGray(image.pixel(j, i+1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
		if (j > 0 && i < image.height() - 1 && qGray(image.pixel(j-1, i+1)) <= empty_threshold)
		{
			result.setPixel(j, i, qRgb(0, 0, 0));
			continue;
		}
	}

	return result;
}

// Morphological erosion
QImage Preprocessor::erode(const QImage& image)
{
	QImage result(image.size(), image.format());
	result.fill(qRgb(255, 255, 255));

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		if (qGray(image.pixel(j, i)) > empty_threshold)
			continue;
		if (j > 0 && qGray(image.pixel(j-1, i)) > empty_threshold)
			continue;
		if (j > 0 && i > 0 && qGray(image.pixel(j-1, i-1)) > empty_threshold)
			continue;
		if (i > 0 && qGray(image.pixel(j, i-1)) > empty_threshold)
			continue;
		if (j < image.width() - 1 && i > 0 && qGray(image.pixel(j+1, i-1)) > empty_threshold)
			continue;
		if (j < image.width() - 1 && qGray(image.pixel(j+1, i)) > empty_threshold)
			continue;
		if (j < image.width() - 1 && i < image.height() - 1
			&& qGray(image.pixel(j+1, i+1)) > empty_threshold)
			continue;
		if (i < image.height() - 1 && qGray(image.pixel(j, i+1)) > empty_threshold)
			continue;
		if (j > 0 && i < image.height() - 1 && qGray(image.pixel(j-1, i+1)) > empty_threshold)
			continue;

		result.setPixel(j, i, qRgb(0, 0, 0));
	}

	return result;
}

// Analyse connected components
QVector< QVector<int> > Preprocessor::analyseComponents(const QImage& marked, int number)
{
	// Values order in inline vector: x_min, y_min, x_max, y_max, points number
	QVector< QVector<int> > result(number+1, QVector<int>(5, 0));

	// Initialize maximum border coordinates
	for (int i = 0; i <= number; i++)
	{
		result[i][0] = marked.width() - 1;
		result[i][1] = marked.height() - 1;
	}

	int label = 0;

	// Loop on whole image
	for (int y = 0; y < marked.height(); y++)
	for (int x = 0; x < marked.width(); x++)
	{
		// Get pixel number
		label = qRed(marked.pixel(x, y));
		if (label > 0 && label <= number)
		{
			// Increase corresponding pixel number
			result[label][4]++;

			// Check for border
			if (x < result[label][0])
				result[label][0] = x;
			if (y < result[label][1])
				result[label][1] = y;
			if (x > result[label][2])
				result[label][2] = x;
			if (y > result[label][3])
				result[label][3] = y;
		}
	}

	return result;
}
