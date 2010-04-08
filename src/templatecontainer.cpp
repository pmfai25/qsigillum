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

#include "templatecontainer.h"

TemplateContainer::TemplateContainer()
{
	setX(0);
	setY(0);
	setWidth(0);
	setHeight(0);
	setInterval(-1);
	groupBox = NULL;
}

TemplateContainer::TemplateContainer(TemplateContainer * container)
{
	setX(container->getX());
	setY(container->getY());
	setWidth(container->getWidth());
	setHeight(container->getHeight());
	setInterval(container->getInterval());
	//if (container->getGroupBox())
		//createGroupBox(container->getGroupBox()->parentWidget());

	foreach (TemplateField * field, container->getFields())
	{
		fields.append(new TemplateField(field));
	}
}

TemplateContainer::~TemplateContainer()
{
	while (!fields.isEmpty())
		delete fields.takeFirst();
}

int TemplateContainer::getX()
{
	return x;
}

void TemplateContainer::setX(int x)
{
	this->x = x;
}

int TemplateContainer::getY()
{
	return y;
}

void TemplateContainer::setY(int y)
{
	this->y = y;
}

int TemplateContainer::getWidth()
{
	return width;
}

void TemplateContainer::setWidth(int width)
{
	this->width = width;
}

int TemplateContainer::getHeight()
{
	return height;
}

void TemplateContainer::setHeight(int height)
{
	this->height = height;
}

int TemplateContainer::getInterval()
{
	return interval;
}

void TemplateContainer::setInterval(int interval)
{
	this->interval = interval;
}

QGroupBox * TemplateContainer::getGroupBox()
{
	return groupBox;
}

void TemplateContainer::createGroupBox(QWidget *parent)
{
	groupBox = new QGroupBox(parent);
}

QList<TemplateField*>& TemplateContainer::getFields()
{
	QList<TemplateField*>& value = fields;
	return value;
}
