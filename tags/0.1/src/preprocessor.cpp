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

	// Set the color table
	grayColorTable.resize(256);
	for (int i=0; i < 256; i++)
	{
		grayColorTable[i] = qRgb(i, i, i);
	}
}

// Remove dark fields from image borders
QImage Preprocessor::removeDarkFields(const QImage& image)
{
	if (image.isNull())
	{
		qDebug() << "Null image sent to Preprocessor::removeDarkFields";
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
			temp += (image.scanLine(i))[j];
		if (qRound(double(temp) / image.height()) > light_threshold)
			break;

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Loop from right
	for (int j = image.width() - 1;
	j >= image.width() - qRound(image.width() * field_size); j--)
	{
		p2.setX(j);
		temp = 0;
		// Check column mean value
		for (int i = 0; i < image.height(); i++)
			temp += (image.scanLine(i))[j];
		if (qRound(double(temp) / image.height()) > light_threshold)
			break;

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Loop from top
	for (int i = 0; i <= qRound(image.height() * field_size); i++)
	{
		p1.setY(i);
		temp = 0;
		// Check row mean value
		for (int j = 0; j < image.width(); j++)
			temp += (image.scanLine(i))[j];
		if (qRound(double(temp) / image.width()) > light_threshold)
			break;

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Loop from bottom
	for (int i = image.height() - 1;
	i >= image.height() - qRound(image.height() * field_size); i--)
	{
		p2.setY(i);
		temp = 0;
		// Check row mean value
		for (int j = 0; j < image.width(); j++)
			temp += (image.scanLine(i))[j];
		if (qRound(double(temp) / image.width()) > light_threshold)
			break;

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Copy resulting image
	return image.copy(p1.x(), p1.y(), p2.x()-p1.x()+1, p2.y()-p1.y()+1);
}

// Remove trash from image borders
QImage Preprocessor::removeBorderTrash(const QImage& image)
{
	if (image.isNull())
	{
		qDebug() << "Null image sent to Preprocessor::removeBorderTrash";
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
			if ((image.scanLine(i))[j] <= (uint)empty_threshold)
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
			if ((image.scanLine(i))[j] <= (uint)empty_threshold)
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
			if ((image.scanLine(i))[j] <= (uint)empty_threshold)
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
			if ((image.scanLine(i))[j] <= (uint)empty_threshold)
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
		if ((image.scanLine(i))[x] <= (uint)empty_threshold)
			return false;

	return true;
}

// Check if image row is empty
bool Preprocessor::emptyRow(const QImage& image, int y)
{
	for (int j = 0; j < image.width(); j++)
		if ((image.scanLine(y))[j] <= (uint)empty_threshold)
			return false;

	return true;
}

// Binarization
QImage Preprocessor::binarize(const QImage& image)
{
	QImage result;
	if (image.isNull())
	{
		qDebug() << "Null image sent to Preprocessor::binarize";
		return result;
	}

	// Create 8-bit QImage and set appropriate color table
	result = QImage(image.size(), QImage::Format_Indexed8);
	result.setColor(0, qRgb(0, 0, 0));
	result.setColor(1, qRgb(255, 255, 255));

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		// Check pixel value
		if ((image.scanLine(i))[j] <= (uint)empty_threshold)
			(result.scanLine(i))[j] = 0;
		else
			(result.scanLine(i))[j] = 1;
	}

	return result;
}

// Transform to 8-bit image with 255 colors
QImage Preprocessor::grayscale(const QImage& image)
{
	QImage result;
	if (image.isNull())
	{
		qDebug() << "Null image sent to Preprocessor::grayscale";
		return result;
	}

	// Create 8-bit QImage and set appropriate color table
	result = QImage(image.size(), QImage::Format_Indexed8);
	result.setColorTable(grayColorTable);

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		(result.scanLine(i))[j] = qGray(image.pixel(j, i));
	}

	return result;
}

// Connected components analysis on binary images
int* Preprocessor::markCC(const QImage& image, int* number)
{
	if (image.isNull())
		return NULL;

	// Marking matrix
	int * matrix = new int[image.width() * image.height()];
	if (!matrix)
		return NULL;
	for (int i = 0; i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
		matrix[image.width() * i + j] = -1;

	// Components number
	int n = 0;
	// Current label
	int current_label = 1;
	// Marking queue
	QQueue<QPoint> queue;

	int x = 0;
	int y = 0;

	// Search for unmarked pixels
	for (int i = 0; i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		// Check pixel occupation
		if (matrix[image.width() * i + j] != -1)
			continue;

		// Check pixel value
		if ((image.scanLine(i))[j] == 0)
		{
			// Append pixel
			queue.enqueue(QPoint(j, i));

			// Process component
			while (!queue.isEmpty())
			{
				QPoint point = queue.dequeue();
				x = point.x();
				y = point.y();
				if (matrix[image.width() * y + x] != -1
					|| (image.scanLine(y))[x] != 0)
					continue;

				// Mark pixel
				matrix[image.width() * y + x] = current_label;

				// Check neighbours
				if (x > 0 && matrix[image.width() * y + x-1] == -1
					&& (image.scanLine(y))[x-1] == 0)
					queue.enqueue(QPoint(x-1, y));
				if (x > 0 && y > 0 && matrix[image.width() * (y-1) + x-1] == -1
					&& (image.scanLine(y-1))[x-1] == 0)
					queue.enqueue(QPoint(x-1, y-1));
				if (y > 0 && matrix[image.width() * (y-1) + x] == -1
					&& (image.scanLine(y-1))[x] == 0)
					queue.enqueue(QPoint(x, y-1));
				if (x < image.width() - 1 && y > 0
					&& matrix[image.width() * (y-1) + x+1] == -1
					&& (image.scanLine(y-1))[x+1] == 0)
					queue.enqueue(QPoint(x+1, y-1));
				if (x < image.width() - 1 && matrix[image.width() * y + x+1] == -1
					&& (image.scanLine(y))[x+1] == 0)
					queue.enqueue(QPoint(x+1, y));
				if (x < image.width() - 1 && y < image.height() - 1
					&& matrix[image.width() * (y+1) + x+1] == -1
					&& (image.scanLine(y+1))[x+1] == 0)
					queue.enqueue(QPoint(x+1, y+1));
				if (y < image.height() - 1
					&& matrix[image.width() * (y+1) + x] == -1
					&& (image.scanLine(y+1))[x] == 0)
					queue.enqueue(QPoint(x, y+1));
				if (x > 0 && y < image.height() - 1
					&& matrix[image.width() * (y+1) + x-1] == -1
					&& (image.scanLine(y+1))[x-1] == 0)
					queue.enqueue(QPoint(x-1, y+1));
			}

			// Update label and components number
			n++;
			current_label++;
		}
		else
		{
			matrix[image.width() * i + j] = 0;
		}
	}


	// Set number
	if (number)
		*number = n;
	return matrix;
}

// Morphological dilation
QImage Preprocessor::dilate(const QImage& image)
{
	QImage result = QImage(image.size(), QImage::Format_Indexed8);
	result.setColor(0, qRgb(0, 0, 0));
	result.setColor(1, qRgb(255, 255, 255));
	result.fill(1);

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		if ((image.scanLine(i))[j] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j > 0 && (image.scanLine(i))[j-1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j > 0 && i > 0 && (image.scanLine(i-1))[j-1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (i > 0 && (image.scanLine(i-1))[j] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j < image.width() - 1 && i > 0
			&& (image.scanLine(i-1))[j+1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j < image.width() - 1 && (image.scanLine(i))[j+1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j < image.width() - 1 && i < image.height() - 1
			&& (image.scanLine(i+1))[j+1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (i < image.height() - 1 && (image.scanLine(i+1))[j] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
		if (j > 0 && i < image.height() - 1
			&& (image.scanLine(i+1))[j-1] == 0)
		{
			(result.scanLine(i))[j] = 0;
			continue;
		}
	}

	return result;
}

// Morphological erosion
QImage Preprocessor::erode(const QImage& image)
{
	QImage result = QImage(image.size(), QImage::Format_Indexed8);
	result.setColor(0, qRgb(0, 0, 0));
	result.setColor(1, qRgb(255, 255, 255));
	result.fill(1);

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		if ((image.scanLine(i))[j] == 1)
			continue;
		if (j > 0 && (image.scanLine(i))[j-1] == 1)
			continue;
		if (j > 0 && i > 0 && (image.scanLine(i-1))[j-1] == 1)
			continue;
		if (i > 0 && (image.scanLine(i-1))[j] == 1)
			continue;
		if (j < image.width() - 1 && i > 0 && (image.scanLine(i-1))[j+1] == 1)
			continue;
		if (j < image.width() - 1 && (image.scanLine(i))[j+1] == 1)
			continue;
		if (j < image.width() - 1 && i < image.height() - 1
			&& (image.scanLine(i+1))[j+1] == 1)
			continue;
		if (i < image.height() - 1 && (image.scanLine(i+1))[j] == 1)
			continue;
		if (j > 0 && i < image.height() - 1
			&& (image.scanLine(i+1))[j-1] == 1)
			continue;

		(result.scanLine(i))[j] = 0;
	}

	return result;
}

// Analyse connected components
QVector< QVector<int> > Preprocessor::analyseComponents(const QImage& image,
														int *marked, int number)
{
	// Values order in inline vector: x_min, y_min, x_max, y_max, points number
	QVector< QVector<int> > result(number+1, QVector<int>(5, 0));

	// Initialize maximum border coordinates
	for (int i = 0; i <= number; i++)
	{
		result[i][0] = image.width() - 1;
		result[i][1] = image.height() - 1;
	}

	int label = 0;

	// Loop on whole image
	for (int y = 0; y < image.height(); y++)
	for (int x = 0; x < image.width(); x++)
	{
		// Get pixel number
		label = marked[image.width() * y + x];
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
