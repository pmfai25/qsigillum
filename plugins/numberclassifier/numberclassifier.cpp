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

#include "numberclassifier.h"

QString NumberClassifier::classify(QImage image)
{
	init();
	this->image = image;
	getBoundingBox();
	qDebug() << "xmin:" << this->x_min;
	return QString();
}

GlyphParameters NumberClassifier::newParam()
{
	GlyphParameters param;
	param.count = 0;
	param.last = 0;
	param.dark.clear();
	param.light.clear();
	return param;
}

void NumberClassifier::init()
{
	this->color = 175;
	this->x_min = 0;
	this->x_max = 0;
	this->y_min = 0;
	this->y_max = 0;
	this->h = 0;
	this->verge1 = 0;
	this->verge2 = 0;
	for (int i = 0; i<10; i++)
		this->result[i] = 0;
	this->feature_double = 0;
	this->feature_horizontal = 0;
	this->feature_single = 0;
	this->feature_strongly_double = 0;
	this->feature_strongly_horizontal = 0;
	this->feature_vertical = 0;
	this->feature_line_distance = 0;
	this->lasts.clear();
	this->flag = 0;
	this->control_flow = QString("");
}

void NumberClassifier::getBoundingBox()
{
	this->x_min = this->image.width();
	this->y_min = this->image.height();
	for (int y = 0; y < this->image.height(); y++)
	for (int x = 0; x < this->image.width(); x++)
	if (qGray(this->image.pixel(x,y)) <= this->color)
	{
		if (x < this->x_min)
			this->x_min = x;
		if (x > this->x_max)
			this->x_max = x;
		if (y > this->y_max)
			this->y_max = y;
		if (y < this->y_min)
			this->y_min = y;
	}
	this->h = this->y_max - this->y_min;
	this->verge1 = this->y_min + qRound(this->h / 5);
	this->verge2 = this->y_max + qRound(this->h / 5);
}

GlyphParameters NumberClassifier::getParameters(int y)
{
	GlyphParameters temp = this->newParam();
	// Initial horizontal coordinate
	int x = this->x_min - 1;
	while (x < this->image.width()) {
		// Light field size
		int i = 0;
		// Light field analyzing
		for (; qGray(this->image.pixel(x,y)) > color
			 && x < this->x_max; i++, x++) {}
		// If we reach a dark field, we should increment
		// dark fields counter
		if (x < this->x_max)
			temp.count++;
		// Otherwise, we should record last light field size
		else
		{
			temp.last = i;
			break;
		}
		// Recording light field size
		if (temp.count >= 1)
			temp.light[temp.count - 1] = i;

		// Dark field size
		int j = 0;
		// Dark field analyzing
		for (; qGray(this->image.pixel(x,y)) <= color
			 && x < this->x_max; j++, x++) {}
		// If we reach a light field, we should record
		// dark field size and reset last light field size
		if (x <= this->x_max)
		{
			temp.dark[temp.count - 1] = j;
			temp.last = 0;
		}
	}

	return temp;
}

void NumberClassifier::processUpperPart()
{
	// Glyph line parameters
	GlyphParameters p = this->newParam();
	// Current horizontal value
	int x = -1;
	// Delayed value
	int x_delayed;
	for (int y = this->y_min; y <= this->verge1; y++)
	{
		// Save delayed value
		x_delayed = x;
		// Use last light field size
		x = p.last;
		// Update parameters
		p = this->getParameters(y);
		// Save current last light field size
		this->lasts.append(p.last);

		// Mark '5' result
		if (x_delayed != -1 && x_delayed - x < -5)
			this->result[5+1] = 1;

		// If there are 2 dark fields, we should mark
		// double feature
		if (p.count == 2)
		{
			this->feature_strongly_double++;
			if (p.dark[1] <= 5 && p.light[1] > 3)
			{
				this->feature_double++;
				// Demarking digit "4" feature
				if (this->result[1+4] < 0)
					this->result[1+4]--;
			}
		}

		// In case of one dark field of small size
		if (p.count == 1 && p.dark[0] <= qRound(this->x_min / 2) && p.dark[0] < 8)
		{
			// Search for doubling
			if (p.light[0] < qRound(this->x_min/4)*3)
			{
				this->feature_double++;
				this->result[1+4]--;
			}
			else
				// Increasing vertical feature
				this->feature_vertical++;
		}
		// Increasing horizontal feature
		if (p.count == 1 && p.dark[0] >= 8)
			this->feature_horizontal++;
		// Increasing strongly horizontal feature
		if (p.count >= 1 && p.dark[0] > 10)
			this->feature_strongly_horizontal++;
		// Increasing single feature
		if (p.count == 1 && p.light[0] >= qRound(this->x_min / 3))
			this->feature_single++;
	}
}

// Check for digit "1"
// Also effects "4" and "6" features
void NumberClassifier::check1()
{
	// Glyph line parameters
	GlyphParameters p;

	// If length of a vertical line is bigger than 75% of
	// the first part of digit
	// and doubling / horizontal features are absent
	if (qAbs(this->feature_vertical - (this->verge1 - this->y_min))
		<= qRound((this->verge1 - this->y_min) / 4)
		&& this->feature_double <= 1
		&& this->feature_horizontal == 0)
	{
		// Analyzing image part from the first verge to digit's half
		for (int y = this->verge1; y <= (this->y_min + qRound(this->h / 2)); y++)
		{
			p = this->getParameters(y);
			// Additional doubling check
			if (p.count == 2 && p.dark[1] <= 8 && p.light[1] > qRound(this->x_min / 3))
				this->feature_double++;
			// Line distance check for digit "1"
			if (p.count == 2 && this->feature_line_distance == 0)
			{
				if (p.light[1] > 5)
					this->feature_line_distance = 1;
				else
					this->feature_line_distance = -1;
			}
		}

		// Set digit "1" feature
		this->result[1+1] = 1;

		// Line distance check
		if (this->feature_line_distance == 1)
		{
			this->result[1+1] = 0;
			this->result[1+4] = 1;
			this->control_flow = QString("end");
		}
	}
}

Q_EXPORT_PLUGIN2(numberclassifier, NumberClassifier)
