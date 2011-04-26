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
	// Testing mode
	//this->checkClassifier();
	//return QString("");

	// Checking input image
	if (image.isNull())
	{
		qDebug() << "Got empty image!";
		return QString("");
	}

	// Initialization and processing
	init();
	this->image = image;
	getBoundingBox();
	processUpperPart();
	check1();
	check6();
	check4();
	check0();
	check3();
	check8();
	check2();
	fullCheck6();
	check9();
	check5();
	check7();

	/*for (int i = 0; i < 10; i++)
		qDebug() << i << ": " << this->result[i];*/

	// Results processing
	if (this->result[0] == 1)
		return QString("0");
	if (this->result[6] == 2)
		return QString("6");
	if (this->result[2] == 1)
		return QString("2");
	if (this->result[4] == 1)
		return QString("4");
	if (this->result[8] == 1)
		return QString("8");
	if (this->result[5] == 1)
		return QString("5");
	if (this->result[9] == 1)
		return QString("9");
	if (this->result[1] == 1)
		return QString("1");
	if (this->result[3] == 1)
		return QString("3");
	if (this->result[6] == 1)
		return QString("6");
	if (this->result[7] == 1)
		return QString("7");
	return QString("");
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
	this->x = 0;
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
	this->verge2 = this->y_max - qRound(this->h / 5);
}

GlyphParameters NumberClassifier::getParameters(int y)
{
	GlyphParameters temp = this->newParam();
	// Initial horizontal coordinate
	int x = this->x_min - 1;
	do
	{
		// Light field size
		int i = 0;
		// Light field analyzing
		do
		{
			i++;
			x++;
		} while (x <= this->x_max &&
				 qGray(this->image.pixel(x,y)) > color);
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
		{
			temp.light.resize(temp.light.size() + 1);
			temp.light[temp.light.size() - 1] = i;
		}

		// Dark field size
		int j = 0;
		// Dark field analyzing
		do
		{
			j++;
			x++;
		} while (x < this->x_max
			 && qGray(this->image.pixel(x,y)) <= color);
		// If we reach a light field, we should record
		// dark field size and reset last light field size
		if (x <= this->x_max)
		{
			temp.dark.resize(temp.dark.size() + 1);
			temp.dark[temp.dark.size() - 1] = j;
		}
	} while (x <= this->x_max);

	return temp;
}

void NumberClassifier::processUpperPart()
{
	// Glyph line parameters
	GlyphParameters p = this->newParam();
	// Current horizontal value
	int x = 0;
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

		//qDebug() << x_delayed << " " << p.last << " " << (x_delayed - p.last < -3);

		// Mark '5' result
		if (x_delayed != 0 && x_delayed - p.last < -3)
			this->result[5] = 1;

		// If there are 2 dark fields, we should increase
		// double feature
		if (p.count == 2)
			this->feature_strongly_double++;


		//qDebug() << "here" << " " << y << " " << p.count << " " << p.dark.size();
		// In case of one dark field of small size
		if (p.count == 1 && p.dark.size() > 0
			&& p.dark[0] < 10)
		{
			// Search for doubling
			if (p.light.size() > 0 && p.light[0] < qRound(this->x_min/4))
			{
				this->feature_double++;
				this->result[4]--;
			}
			else
				// Increasing vertical feature
				this->feature_vertical++;
		}

		if (p.count == 2 && p.dark.size() > 1 && p.dark[1] <= 5
			&& p.light.size() > 1 && p.light[1] > 3)
		{
			this->feature_double++;
			// Suppressing digit "4" feature
			if (this->result[4] < 0)
				this->result[4]--;
		}

		// Increasing horizontal feature
		if (p.count == 1 && p.dark.size() > 0 && p.dark[0] >= 9)
			this->feature_horizontal++;
		// Increasing strongly horizontal feature
		if (p.count >= 1 && p.dark.size() > 0 && p.dark[0] > 10)
			this->feature_strongly_horizontal++;
		// Increasing single feature
		if (p.count == 1 && p.light.size() > 0
			&& p.light[0] >= qRound(this->x_min / 3))
			this->feature_single++;
	}
}

// Check for digit "1"
// Also effects "4" feature
void NumberClassifier::check1()
{
	// Glyph line parameters
	GlyphParameters p;

	/*qDebug() << this->feature_vertical << " " <<  this->verge1 << " "
					 <<  this->y_min << " " << qRound((this->verge1 - this->y_min) / 4 + 1);*/


	// If length of a vertical line is bigger than 75% of
	// the first part of digit
	// and doubling / horizontal features are absent
	if (qAbs(this->feature_vertical - (this->verge1 - this->y_min))
		<= qRound((this->verge1 - this->y_min) / 4 + 1)
		&& this->feature_double <= 1
		&& this->feature_horizontal <= 3)
	{
		// Analyzing image part from the first verge to digit's half
		for (int y = this->verge1; y <= (this->y_min + qRound(this->h / 2)); y++)
		{
			p = this->getParameters(y);
			// Additional doubling check
			if (p.count == 2 && p.dark.size() > 1 && p.dark[1] <= 8
				&& p.light.size() > 1 && p.light[1] > qRound(this->x_min / 3))
				this->feature_double++;
			// Line distance check for digit "1"
			if (p.count == 2 && this->feature_line_distance == 0)
			{
				if (p.light.size() > 1 && p.light[1] > 5)
					this->feature_line_distance = 1;
				else
					this->feature_line_distance = -1;
			}
		}

		// Set digit "1" feature
		this->result[1] = 1;

		// Line distance check
		if (this->feature_line_distance == 1)
		{
			this->result[1] = 0;
			this->result[4] = 1;
			this->control_flow = QString("end");
		}
	}
}

// Basic check for digit "6"
void NumberClassifier::check6()
{
	if (this->control_flow != QString(""))
		return;

	// Current vertical coordinate
	int y = this->y_max;
	// Glyph line parameters
	GlyphParameters p = this->getParameters(y);

	// Scanning upwards to check lower part
	do
	{
		y--;
		p = this->getParameters(y);
	} while (y > this->verge2 - 1 && p.dark.size() > 0
		   && p.dark[0] <= 5 );

	if (y > (this->verge2 - 1))
	{
		// Line distance check
		this->feature_line_distance = 0;
		do
		{
			y--;
			p = this->getParameters(y);
			// If a number of dark fields exist,
			// line distance feature should be increased
			if (p.count > 1)
				this->feature_line_distance++;
		} while (y >= this->verge2 - 5);


		//qDebug() << "here " << this->feature_line_distance;
		// old value: 4
		if (this->feature_line_distance > 7)
		{
			// Suppressing digit "1" feature
			this->result[1] = 0;
			// There is a need for full digit "6" analysis
			this->control_flow = QString("six");
		}
	}
}

// Check for digit "4"
void NumberClassifier::check4()
{
	if (this->control_flow != QString(""))
		return;

	// Single feature analysing and suppressing
	if (qAbs(this->feature_double - qAbs(this->result[4]))
		<= qRound((this->verge1 - this->y_min)/4)
		&& this->feature_strongly_horizontal == 0)
		this->feature_single = 0;

	// If double feature is strong, it testifies about digit "4"
	if (this->feature_double > qRound((this->verge1 - this->y_min)/3)
		&& this->feature_horizontal == 0 && this->feature_single == 1)
	{
		// Current vertical coordinate
		int y = this->y_max;
		// Glyph line parameters
		GlyphParameters p = this->getParameters(y);

		// Scanning upwards to check lower part
		for (; (y > this->verge2 && p.dark.size() > 0
			&& p.count > 1 && p.dark[0] <= 5 ) ; y-- )
		{
		}

		// Current part narrowness check
		if (y > (this->verge2 - 1))
		{
			this->result[4] = 1;
			this->control_flow = QString("end");
		}
	}
}

// Check for digit "0"
void NumberClassifier::check0()
{
	if (this->control_flow != QString("")
		&& this->control_flow != QString("six"))
		return;

	this->feature_double = 0;
	this->feature_line_distance = 0;

	// Glyph line parameters
	GlyphParameters p;

	// Check for two vertical distant lines
	for (int y = this->verge1; y <= this->verge2; y++)
	{
		p = this->getParameters(y);
		// Dark fields number check
		if (p.count == 1)
			this->feature_line_distance++;

		// "Cap" check
		if (p.count > 2 && p.dark.size() > 1 && p.dark[0] <= 8 && p.dark[1] <= 7
			&& p.light.size() > 2 && p.light[1] < 3
			&& p.light[2] > qRound(this->x_min / 4))
		{
			// Doubling feature increasing
			this->feature_double++;
			this->x = p.light[2];
		}

		if (p.count == 2 && p.dark[0] <= 9 && p.dark.size() > 1
			&& p.dark[1] <= 7 &&  p.light.size() > 1
			&& p.light[1] > qRound(this->x_min / 4))
		{
			// Doubling feature increasing
			this->feature_double++;
		}
	}

	// If doubling feature is strong, we suppose we got digit "0",
	// but additional check for digit "6" is still needed
	if (this->feature_double >= qRound(0.25 * (this->verge2 - this->verge1))
		&& this->feature_line_distance <= 2)
	{
		this->result[0] = 1;
		this->control_flow = QString("six");
	}
}

// Check for digit "3"
void NumberClassifier::check3()
{
	if (this->control_flow != QString("")
		&& this->control_flow != QString("six"))
		return;

	this->feature_line_distance = 0;
	int x_delayed = 0;

	// Digit back part drop and lower hook analysis
	for (int y = this->y_min; y < this->verge1; y++)
	{
		// Check difference between current and previous
		// distances from pixel to right border
		if (qAbs(this->lasts[y - this->y_min] -
				 this->lasts[y - this->y_min+1]) < 4)
			this->feature_line_distance++;
	}

	// If number of drops is small, no analysis is necessary
	if ((this->verge1 - this->y_min) - this->feature_line_distance >= 2)
		return;

	this->feature_line_distance = 0;
	int y = this->verge1;
	// Glyph line parameters
	GlyphParameters p = this->getParameters(y);
	this->x = p.last;

	// Drop search
	while (y < this->verge1 + qRound((this->verge2 - this->verge1) / 4)
		&& p.last - this->x <= 5)
	{
		this->x = p.last;
		y++;
		p = this->getParameters(y);

		if (p.last >= this->x && qAbs(p.last - this->x) < 4)
			this->feature_line_distance++;
	}

	// Check for hook direction
	if (p.last - this->x > 5 || (p.count > 1
								 && p.dark.size() > 0 && p.dark[0] > 1))
		this->result[3] = 0;
	else
	{
		y = this->y_max;
		this->feature_line_distance = 0;
		// Max last distance
		int max_last = this->x_min;
		this->feature_vertical = 0;
		this->feature_single = 0;
		this->feature_double = 0;
		this->feature_strongly_double = 0;
		this->flag = 0;

		// Loop from bottom verge to digit's quarter or hook's end
		do
		{
			// One iteration delay
			this->x = p.last;
			p = this->getParameters(y);

			// Check for decreasing gap between digit "3" back and right
			// border
			if (p.last <= this->x)
			{
				this->feature_line_distance++;
				if (p.last < this->x)
					this->feature_vertical = 0;
			}

			// Check for increasing gap
			if (p.last >= this->x)
				this->feature_vertical++;

			// Hook check
			if (p.count == 2 && p.light.size() > 1 && p.light[1] >= 5)
			{
				x_delayed = p.light[1];
				this->feature_double++;
			}

			// If just one line remains after two have been detected,
			// and distance is bigger than 5 px, we should increase
			// single feature
			if (p.count == 1 && x_delayed > 5 && this->feature_single == 0)
				this->feature_single = 1;
			if (y < this->verge2 && this->feature_double < 2
				&& p.count == 1 && this->feature_single == 0)
				this->feature_single = 1;

			// Smooth drop check
			if (qAbs(p.last - this->x) > 3 && y < this->verge2)
				this->feature_single = 2;
			if (p.light.size() > 0 && p.light[0] < qRound(this->x_min / 3))
				this->feature_strongly_double++;

			// Transition to next line
			y--;
		} while (y >= this->verge1 + qRound(this->h / 4)
			&& this->feature_vertical <= 2);

		// Received data analysis
		if (this->feature_single == 2)
			this->feature_single = 1;
		else
		{
			max_last = p.last;
			this->feature_vertical = 0;
			this->feature_line_distance = 0;
			x_delayed = 0;

			// Continue loop from hook's end to drop
			do
			{
				x = p.last;
				p = this->getParameters(y);
				if (p.last - x > 5 && y < this->verge2 - 3)
					this->feature_single = 2;
				if (p.last > x)
					this->feature_line_distance++;
				if (p.last > x_delayed)
					x_delayed = p.last;
				if (p.count == 1 && this->feature_single == 0)
					this->feature_single = 1;
				if (p.light.size() > 0 && p.light[0] < qRound(this->x_min / 5))
					this->feature_strongly_double++;

				// Transition to next line
				y--;
			} while (y > this->verge1);

			// Single feature suppressing
			if (x_delayed < max_last || this->feature_single == 2)
				this->feature_single = 0;

			// Digit "3" feature setting
			if (this->feature_single != 0 && this->feature_strongly_double < y)
				this->result[3] = 1;

		}
	}

}

// Check for digit "8"
void NumberClassifier::check8()
{
	if (this->control_flow != QString("")
		&& this->control_flow != QString("six"))
		return;

	// Central cross search, lower loop analysis
	int y = this->verge1;
	int max_last = 0;
	int x_delayed = 0;
	this->feature_single = 0;
	this->feature_line_distance = 0;
	// Glyph line parameters
	GlyphParameters p;

	// Descent from upper verge to the bottom or to cross
	do
	{
		p = this->getParameters(y);
		y++;
		if (p.count > 1 && p.light.size() > 1)
			this->feature_line_distance = p.light[1];
	} while (y < this->verge2 - 2 &&
			 !(p.count == 1 && this->feature_line_distance < 5));

	// If cross belongs to allowed space:
	if (y < this->y_min + 2 * qRound(this->h / 3)
		&& y - this->verge1 > 2)
	{
		// Suppressing digit "8" feature
		this->result[8]--;
		max_last = p.last;

		// Search for doubling after narrowing
		do
		{
			if (p.dark.size() > 0)
				this->x = p.dark[0];
			p = this->getParameters(y);
			if (p.last > max_last)
				max_last = p.last;
			y++;
		} while (y < this->verge2 - 2 &&
				 !(p.count == 1 &&
				   ( (p.light.size() > 1 && p.light[1] < 5) || this->x < 8)));

		if (y <= this->verge2 - 2)
		{
			// Suppressing digit "8" feature
			this->result[8]--;
			this->x = 0;
			this->feature_line_distance = 0;
			this->feature_horizontal = 0;
			this->feature_vertical = 0;
			x_delayed = p.last;

			// Lower hook analysis
			do
			{
				p = this->getParameters(y);

				if (p.count > 1)
				{
					// Two lines have been detected
					this->feature_line_distance++;
					this->feature_vertical = 0;
				}

				// If excess lines have been detected:
				if (p.count > 2 && p.light.size() > 1 && p.light[1] > 6)
					this->feature_horizontal++;

				// If one line has been detected:
				if (p.count == 1)
					this->feature_vertical++;

				if (p.last < x_delayed)
					x_delayed = p.last;

				this->x++;
				y++;
			} while (y < this->y_max && this->feature_vertical <= 5);

			// Digit "8" feature setting
			if ((this->x - 3) - this->feature_line_distance <=
				qRound(this->x / 4 + 1)
				&& this->feature_vertical <= 5
				&& x_delayed < max_last
				&& this->feature_horizontal < 2)
			{
				this->result[8] = 1;
				this->control_flow = QString("end");
			}
		}
	}

}

// Check for digit "2"
void NumberClassifier::check2()
{
	//qDebug() << "here(2) - 1" << this->control_flow;

	if (this->control_flow != QString(""))
		return;

	int y = this->verge2;
	int x_delayed = 0;
	this->flag = 0;
	this->feature_line_distance = this->x_min;
	this->feature_strongly_horizontal = 0;
	// Glyph line parameters
	GlyphParameters p;
	int max_last = 0;

	// Lower tail analysis
	while (y < this->y_max)
	{
		p = this->getParameters(y);

		// Tail end search
		if (p.last < this->feature_line_distance)
			this->feature_line_distance = p.last;

		if (p.count == 0)
			break;

		if (p.count > 1 || (p.dark.size() > 0 && p.dark[0] >= 5))
			x_delayed++;

		// If tail is straight:
		if (p.count == 1 && p.dark.size() > 0 && p.dark[0] >= 8)
			this->feature_strongly_horizontal++;

		y++;
	}

	if (qAbs(x_delayed - (this->y_max - this->verge2))
		<= qRound((this->y_max - this->verge2) / 2)
		|| this->feature_strongly_horizontal > 0)
	{
		// Increasing digit "2" feature
		this->result[2] = 1;
		y = this->verge2;
		max_last = 0;

		// Middle part analysis
		do
		{
			y--;
			p = this->getParameters(y);

			// Check for lower tail's transition to hook
			if (p.last > max_last)
			{
				max_last = p.last;
				this->feature_vertical = y;
			}
			if (p.count > 1 && p.light.size() > 1)
				this->x = p.light[1];

		} while (y > this->verge1
				 && !(p.count == 1 && ((p.dark.size() > 0 && p.dark[0] < 10)
									   || this->x < 4)));


		/*qDebug() << p.count << " " << p.light[0] << " "
				<< " " << (3 * qRound(this->x_min / 4)) << " y:" << y << " " << this->y_max;*/
		// If just one line is detected in right quarter of the digit
		// and vertical coordinate belongs to allowed space:
		if (p.count == 1 && p.light.size() > 0 && p.light[0] > 10
			&& p.light[0] > 3 * qRound(this->x_min / 4)	&& y < this->y_max - 2)
			this->flag = 1;

		// Digit "2" feature setting
		if (p.count == 1 && p.last > this->feature_line_distance
			&& this->x < qRound(this->x_min / 2))
			this->result[2] = 1;
		else
			this->result[2] = 0;

		// Leg form analysis
		if (p.count == 1 && p.light.size() > 0
			&& p.light[0] < 2 * qRound((this->verge2 - y) / 3)
			&& this->verge2 - y > 5)
			this->result[2] = 0;

		// Additional check for upper hook
		if (this->result[2] == 1)
		{
			this->feature_vertical = 0;

			for (y = this->y_min; y <= this->verge1; y++)
			{
				p = this->getParameters(y);
				if (p.count == 1 && p.dark.size() > 0 && p.dark[0] <= 6)
					this->feature_vertical++;
			}

			if ((this->verge1 - this->y_min) - this->feature_vertical < 1)
				this->result[2] = 0;

			if (this->flag != 0)
				this->result[2] = 0;

		}
	}
}

// Full check for digit "6"
void NumberClassifier::fullCheck6()
{
	if (this->control_flow != QString("")
		&& this->control_flow != QString("six"))
		return;

	this->control_flow = QString("");

	// Lower part and upper hook analysis
	int y = this->y_max;
	this->feature_single = 0;
	// Glyph line parameters
	GlyphParameters p = this->getParameters(y);

	// Doubling check
	// Loop from bottom to second verge
	do
	{
		if (p.dark.size() > 0)
			this->x = p.dark[0];
		p = this->getParameters(y);
		y--;

	} while (y > this->verge2 - 3 && p.count <= 1);

	// If there is doubling in allowed space:
	if (y > this->verge2 - 3)
	{
		this->feature_horizontal = 0;
		this->feature_double = 0;
		if (p.light.size() > 0)
			this->x = p.light[0];
		else
			this->x = 0;

		do
		{
			// Delay values
			this->feature_line_distance = this->x;
			if (p.light.size() > 0)
				this->x = p.light[0];

			this->feature_strongly_double = this->feature_double;
			this->feature_double = p.last;

			p = this->getParameters(y);
			y--;
			// Leave loop, if upper verge is reached or
			// there is narrowing to 1px width line
		} while (y >= this->verge1 &&
				 !(p.count == 1 && p.dark.size() > 0 && p.dark[0] <= 6));


	/*qDebug() << (p.light.size() > 0) << " " << (qAbs(this->feature_line_distance - p.light[0]) < 5)
			<< " " << (p.light[0] <= qRound(this->x_min / 2)) << " " << (y < this->y_max - qRound(this->h / 3))
			<< " " << (p.last - this->feature_strongly_double >= 0);
	qDebug() << p.light[0] << " " << this->x_min << " " << qRound(2* this->x_min / 3);
	qDebug() << y << " " << this->y_max << " " << qRound(this->h / 3);
	qDebug() << p.last << " " << this->feature_strongly_double;*/

		// If drops are allowed and loop belongs to left half
		// and merge is at allowed height and drop is positive
		if (p.light.size() > 0
			&& qAbs(this->feature_line_distance - p.light[0]) < 5
			//&& p.light[0] <= qRound(this->x_min / 2)
			&& y < this->y_max - qRound(this->h / 3)
			&& p.last - this->feature_strongly_double > 0)
		{
			// Upper part analysis
			do
			{
				if (p.light.size() > 0)
					this->x = p.light[0];
				p = this->getParameters(y);
				y--;
			} while (y > this->y_min && p.count <=1
					 && p.light.size() > 0 && p.light[0] - this->x >= 0);

			// Digit "6" feature increasing
			if (p.light.size() > 0 && p.light[0] - this->x >= 0 )
			{
				this->result[6] = 2;
				this->control_flow = QString("end");
			}
		}
	}
}

// Check for digit "9"
void NumberClassifier::check9()
{
	if (this->control_flow != QString(""))
		return;

	// Upper part and lower hook analysis
	int y = this->y_min;
	this->feature_single = 0;
	// Glyph line parameters
	GlyphParameters p = this->getParameters(y);

	// Doubling check
	// Loop from top to first verge
	do
	{
		if (p.dark.size() > 0)
			this->x = p.dark[0];
		p = this->getParameters(y);
		y++;

	} while (y < this->verge1 + 3
			 && !(p.count > 1 && p.light.size() > 1 && p.light[1] > 1));

	// If there is doubling in allowed space:
	if (y < this->verge1 + 3)
	{
		this->feature_horizontal = 0;
		this->feature_double = 0;
		if (p.light.size() > 0)
			this->x = p.light[0];
		this->flag = 0;
		int max_light = 200;
		int x_delayed = 0;

		do
		{
			// Delay values
			this->feature_line_distance = this->x;
			if (p.light.size() > 0)
				this->x = p.light[0];

			this->feature_strongly_double = this->feature_double;
			this->feature_double = p.last;

			p = this->getParameters(y);

			if (p.count > 1)
				max_light = p.last;

			if (p.count == 1 && max_light - p.last < -5)
				x_delayed++;

			y++;
			// Leave loop, if lower verge is reached or
			// there is narrowing to 1px width line
		} while (y <= this->verge2 && x_delayed <= 3 && p.count >= 1
				 && p.light.size() > 0 && this->x - p.light[0] > -3);

		// Suppressing digit "7" feature
		this->result[7] = -1 * y;
		this->feature_horizontal =  (p.light.size() > 0) ? p.light[0] : 0;

		// Digit "9" feature setting
		if (x_delayed <= 3 && y < this->verge2 + 1 && this->result[1] == 0
			&& this->result[3] == 0 && y > this->y_min + qRound(this->h / 3))
			this->result[9] = 1;
	}

	if (this->result[9] == 0)
	{
		y = this->y_max;
		p = this->getParameters(y);
		this->feature_line_distance = -1;
		this->feature_strongly_double = -1;
		while (p.count == 1 && y > this->verge2)
		{
			y--;
			p = this->getParameters(y);
		}

		if (y <= this->verge2)
			return;

		while (p.count == 2 && y > this->verge2 + 3)
		{
			p = this->getParameters(y);
			if (p.count == 2 && p.light.size() > 1) {
				this->feature_strongly_double = p.light[0];
				this->feature_line_distance = p.light[1];
			}
			y--;
		}

		if (y > this->verge2 + 3)
		{
			if (this->feature_line_distance > 3 && p.light.size() > 0
				&& this->feature_strongly_double < p.light[0])
			{
				this->result[9] = 1;
			}
		}

	}
}

// Check for digit "5"
void NumberClassifier::check5()
{
	if (this->control_flow != QString("") || this->result[5] != 0)
		return;

	// Checking for other digits features
	if (this->result[7] == 0 && this->result[3] == 0
		&& this->result[1] == 0 && this->result[6] == 0)
	{
		this->result[5] = 1;
		return;
	}

	this->flag = 0;
	int y = this->verge1;
	this->x = -1;
	int x_delayed = 0;
	// Glyph line parameters
	GlyphParameters p = this->getParameters(y);

	// Descent from upper verge in search of drop
	do
	{
		x_delayed = this->x;
		this->x = p.last;
		p = this->getParameters(y);

		if (this->x != 0 && x_delayed - p.last < -5 && x_delayed != -1)
			this->flag = 1;

//		if (p.last > 5)
//			this->flag = 1;

		y++;

	} while (y <= this->verge2 && this->flag != 1 && p.count != 1);

	// Digit "5" feature setting
	if (this->flag == 1)
	{
		this->result[5] = 1;
		this->control_flow = QString("end");
	}

}

// Check for digit "7"
void NumberClassifier::check7()
{
	if (this->control_flow != QString(""))
		return;

	// Checking for digit "9" feature
	if (this->result[9] == 0)
	{
		this->result[7] = 1;
	}
	else
	{
		int y = this->verge2;
		int x_delayed = 0;
		// Glyph line parameters
		GlyphParameters p = this->getParameters(y);

		// Additional check for horizontal dash in middle part
		do
		{
			x_delayed = this->x;
			x = p.last;
			p = this->getParameters(y);
			y--;
		} while (y > qAbs(this->result[7]) && p.last - this->x >= 3);

		// Digit "7" feature setting
		if (y > qAbs(this->result[7]))
			this->result[7] = 1;

	}

//	int y = this->y_max;
//	int x_delayed = -1;
//	GlyphParameters p = this->getParameters(y);
//	this->feature_line_distance = -1;
//	while (p.count == 1 && y > this->verge2)
//	{
//		y--;
//		if (p.light.size() > 0)
//			x_delayed = p.light[0];
//		p = this->getParameters(y);
//
//
//		if (p.light.size() > 0 && qAbs(x_delayed - p.light[0]) < 3)
//		{
//			this->result[7] = 1;
//			this->result[3] = 0;
//		}
//
//	}

}

// Check classifier for training set
void NumberClassifier::checkClassifier()
{
	// Go to training set directory
	QDir dir = QDir(qApp->applicationDirPath());
	dir.cd("/home/konst/coding/projects/mnist/testing/");
	qDebug() << "NumberClassifier path: " << dir.path();

	// Get file list
	QStringList filelist = dir.entryList(QStringList("*.bmp"));

	// Digit image width and height
	int part_size = 28;

	// Class image sheet containing a number of digit images
	QImage classImage;

	// Class image coordinates
	int class_x = 0;
	int class_y = 0;

	// Finish flag
	bool isFinished = false;

	// Total number of patterns in class
	int total_number = 0;
	// Number of correctly recognized patterns
	int correct_number = 0;

	// Looping over classes
	for (int l = 0; l < 10; l++)
	{
		// Load image
		classImage = QImage(dir.absoluteFilePath(filelist.at(l)));

		isFinished = false;
		class_x = 0;
		class_y = 0;

		total_number = 0;
		correct_number = 0;

		do
		{
			// Get image part
			this->image = autoRotate(classImage.copy(class_x, class_y, part_size, part_size));
			// Check image
			if (this->image.isNull())
			{
				isFinished = true;
				continue;
			}

			// Update class image counter
			total_number++;

			// Classify image
			init();
			getBoundingBox();
			processUpperPart();
			check1();
			check6();
			check4();
			check0();
			check3();
			check8();
			check2();
			fullCheck6();
			check9();
			check5();
			check7();


			// Results processing
			int result = -1;
			if (this->result[0] == 1)
				result = 0;
			else if (this->result[6] == 2)
				result = 6;
			else if (this->result[2] == 1)
				result = 2;
			else if (this->result[4] == 1)
				result = 4;
			else if (this->result[8] == 1)
				result = 8;
			else if (this->result[5] == 1)
				result = 5;
			else if (this->result[9] == 1)
				result = 9;
			else if (this->result[1] == 1)
				result = 1;
			else if (this->result[3] == 1)
				result = 3;
			else if (this->result[6] == 1)
				result = 6;
			else if (this->result[7] == 1)
				result = 7;

			// Checking label
			if (result == l)
				correct_number++;

			// Update class image coordinates
			class_y += part_size;
			if (class_y >= classImage.height())
			{
				class_y = 0;
				class_x += part_size;
			}
			if (class_x >= classImage.width())
			{
				isFinished = true;
				continue;
			}

		} while (!isFinished && total_number < 100);

		// Output total statistics
		qDebug() << "class" << l << ":" << correct_number
				<< "/" << total_number << "("
				<< double(correct_number) / total_number << ")";

	}
}

// Initialize color table
void NumberClassifier::initColorTable()
{
	// Set the color table
	grayColorTable.resize(256);
	for (int i = 0; i < 256; i++)
	{
		grayColorTable[i] = qRgb(i, i, i);
	}
}

// Compensate rotation for digit image
// Method is duplicated from Preprocessor and is used for training / testing
QImage NumberClassifier::autoRotate(const QImage& image)
{
	if (grayColorTable.size() < 256)
		initColorTable();

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
		if ((image.scanLine(i))[j] < color)
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
		if ((image.scanLine(i))[j] < color)
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
		if ((image.scanLine(i))[j] < color)
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

	// Additional dilation to compensate sampling errors
	for (int i = 1; i < temp.height() - 1; i++)
	for (int j = 1; j < temp.width() - 1; j++)
	{
		if (((temp.scanLine(i))[j-1] < color)
			&& ((temp.scanLine(i-1))[j] < color)
			&& ((temp.scanLine(i+1))[j] < color)
			&& ((temp.scanLine(i))[j+1] < color))
		{
			(temp.scanLine(i))[j] = 0;
		}

	}

	return temp;
}

// Get base plugin name
QString NumberClassifier::getBaseName()
{
	return QString("NumberClassifier");
}


Q_EXPORT_PLUGIN2(numberclassifier, NumberClassifier)
