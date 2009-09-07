/*
 *	QSigillum - handwritten digits recognition for structured documents
 *  Copyright 2009 Konstantin "konst" Kucher <konst.hex@gmail.com>,
 *  Miroslav "sid" Sidorov <grans.bwa@gmail.com>
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

#include "templatefield.h"

TemplateField::TemplateField()
{
	setX(0);
	setY(0);
	setWidth(0);
	setHeight(0);
	lineEdit = NULL;
}

TemplateField::~TemplateField()
{
	if (lineEdit != NULL)
		delete lineEdit;
}

int TemplateField::getX()
{
	return x;
}

void TemplateField::setX(int x)
{
	this->x = x;
}

int TemplateField::getY()
{
	return y;
}

void TemplateField::setY(int y)
{
	this->y = y;
}

int TemplateField::getWidth()
{
	return width;
}

void TemplateField::setWidth(int width)
{
	this->width = width;
}

int TemplateField::getHeight()
{
	return height;
}

void TemplateField::setHeight(int height)
{
	this->height = height;
}

QLineEdit * TemplateField::getLineEdit()
{
	return lineEdit;
}

void TemplateField::createLineEdit(QWidget *parent)
{
	lineEdit = new QLineEdit(parent);
}
