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
	field_size = 0.20;

	// Empty column checking
	empty_threshold = 200;

	// Trash removal
	empty_field_size = 0.1;

	// Set the color table
	grayColorTable.resize(256);
	for (int i = 0; i < 256; i++)
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
		// Check row values
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
	// Column-wise order is necessary to maintain left-to-right digits order
	for (int j = 0; j < image.width(); j++)
	for (int i = 0; i < image.height(); i++)
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
	// Values order in inner vector: label, x_min, y_min, x_max, y_max, points number
	QVector< QVector<int> > result(number, QVector<int>(6, 0));

	for (int i = 0; i < number; i++)
	{
		// Initialize border coordinate
		result[i][0] = i + 1;
		// Initialize minimum border coordinates
		result[i][1] = image.width() - 1;
		result[i][2] = image.height() - 1;
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
			result[label-1][5]++;

			// Check for border
			if (x < result[label-1][1])
				result[label-1][1] = x;
			if (y < result[label-1][2])
				result[label-1][2] = y;
			if (x > result[label-1][3])
				result[label-1][3] = x;
			if (y > result[label-1][4])
				result[label-1][4] = y;
		}
	}

	return result;
}

// Analyse specified component using given info vector
QVector<int> Preprocessor::analyseComponent(const QImage& image, int* marked, QVector<int> field)
{
	// Values order label, x_min, y_min, x_max, y_max, points number
	QVector<int> result(6, 0);
	result[0] = field[0];
	// Initialize minimum border coordinates
	result[1] = field[3];
	result[2] = field[4];

	int label = 0;

	// Loop on image part
	for (int y = field[2]; y <= field[4]; y++)
	for (int x = field[1]; x <= field[3]; x++)
	{
		// Get pixel number
		label = marked[image.width() * y + x];
		if (label == result[0])
		{
			// Increase corresponding pixel number
			result[5]++;

			// Check for border
			if (x < result[1])
				result[1] = x;
			if (y < result[2])
				result[2] = y;
			if (x > result[3])
				result[3] = x;
			if (y > result[4])
				result[4] = y;
		}
	}

	return result;
}

// Calculate coordinates of center of mass of binary image
QPoint Preprocessor::getMassCenter(const QImage& image)
{
	QPoint result = QPoint(image.width()/2, image.height()/2);

	// Arrays for histogram analysis
	double * vertical_histogram = new double[image.height()];
	double * horizontal_histogram = new double[image.width()];
	if (!vertical_histogram || !horizontal_histogram)
		return result;

	// Initialize histograms
	for (int y = 0; y < image.height(); y++)
	{
		vertical_histogram[y] = 0.0;
	}
	for (int x = 0; x < image.width(); x++)
	{
		horizontal_histogram[x] = 0.0;
	}

	// Loop on whole image
	for (int y = 0; y < image.height(); y++)
	for (int x = 0; x < image.width(); x++)
	{
		if ((image.scanLine(y))[x] == 0)
		{
			// Update histograms
			vertical_histogram[y]++;
			horizontal_histogram[x]++;
		}
	}

	// Histogram sums
	double vertical_sum = 0.0;
	double horizontal_sum = 0.0;

	// Weighted histogram sums
	double vertical_weighted_sum = 0.0;
	double horizontal_weighted_sum = 0.0;

	// Loop on vertical histogram
	for (int y = 0; y < image.height(); y++)
	{
		// Update histogram sum
		vertical_sum += vertical_histogram[y];

		// Update weighted sum
		vertical_weighted_sum += double(y) * vertical_histogram[y];
	}

	// Loop on horizontal histogram
	for (int x = 0; x < image.width(); x++)
	{
		// Update histogram sum
		horizontal_sum += horizontal_histogram[x];

		// Update weighted sum
		horizontal_weighted_sum += double(x) * horizontal_histogram[x];
	}

	delete[] vertical_histogram;
	delete[] horizontal_histogram;

	// Check resulting coordinates
	if (horizontal_weighted_sum/horizontal_sum > 0.000001
		&& horizontal_weighted_sum/horizontal_sum < double(image.width()+1))
	{
		result.setX(horizontal_weighted_sum/horizontal_sum);
	}
	if (vertical_weighted_sum/vertical_sum > 0.000001
		&& vertical_weighted_sum/vertical_sum < double(image.height()+1))
	{
		result.setY(vertical_weighted_sum/vertical_sum);
	}

	return result;
}

// Compensate rotation for grayscale digit image
QImage Preprocessor::autoRotate(const QImage& image)
{
	if (image.height() <= 1 || image.width() <= 1)
		return image;

	// POI counter
	int totalNumber = 0;

	// Mean coordinates of upper part
	double upperX = 0.0;
	double upperY = 0.0;

	// Analyze upper 25% of image
	for (int i = 0; i <= int(image.height() / 4); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < (uint)empty_threshold)
		{
			totalNumber++;
			upperX += (double) j;
			upperY += (double) i;
		}
	}

	// Calculate mean coordinates
	if (totalNumber <= 0)
		return image;

	upperX /= totalNumber;
	upperY /= totalNumber;

	totalNumber = 0;

	// Mean coordinates of lower part
	double lowerX = 0.0;
	double lowerY = 0.0;

	// Analyze lower 25% of image
	for (int i = int(3 * image.height() / 4); i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < (uint)empty_threshold)
		{
			totalNumber++;
			lowerX += (double) j;
			lowerY += (double) i;
		}
	}

	// Calculate mean coordinates
	if (totalNumber <= 0)
		return image;

	lowerX /= totalNumber;
	lowerY /= totalNumber;

	// Check horizontal difference
	if (abs(upperX - lowerX) < 1.0)
		return image;

	// Calculate rotation angle using norm value
	double angle = 0.2 * atan((upperY - lowerY) / (upperX - lowerX));

	// Create temporary image for pixel rotation
	QImage temp = QImage(qRound(image.width() * 1.5),
						 qRound(image.height() * 1.5), QImage::Format_Indexed8);
	temp.setColorTable(grayColorTable);
	temp.fill(255);

	// Rotation centre
	int x0 = image.width() / 2;
	int y0 = image.height() / 2;

	// Resulting image pixel coordinates
	int x = 0;
	int y = 0;

	// Rotation
	for (int i = 0; i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < (uint)empty_threshold)
		{
			x = x0 + qRound(double(j - x0) * cos(angle)
							- double(i - y0) * sin(angle));
			y = y0 + qRound(double(j - x0) * sin(angle)
							+ double(i - y0) * cos(angle));

			if (x >= 0 && x < temp.width()
				&& y >= 0 && y < temp.height())
			{
				(temp.scanLine(y))[x] = (image.scanLine(i))[j];
			}
		}
	}

	// Additional erosion to compensate sampling errors
	for (int i = 1; i < temp.height() - 1; i++)
	for (int j = 1; j < temp.width() - 1; j++)
	{
		if (((temp.scanLine(i))[j-1] < (uint)empty_threshold)
			&& ((temp.scanLine(i-1))[j] < (uint)empty_threshold)
			&& ((temp.scanLine(i+1))[j] < (uint)empty_threshold)
			&& ((temp.scanLine(i))[j+1] < (uint)empty_threshold))
		{
			(temp.scanLine(i))[j] = 0;
		}

	}

	return temp;
}

