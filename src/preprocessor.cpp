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
