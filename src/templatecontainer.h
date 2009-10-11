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

#ifndef TEMPLATECONTAINER_H
#define TEMPLATECONTAINER_H

#include <QList>
#include <QGroupBox>

#include "templatefield.h"

class TemplateContainer : public QObject
{
private:
	// Horizontal coordinate in template's coordinate system
	int x;
	// Vertical coordinate in template's coordinate system
	int y;
	int width;
	int height;

	// Row interval. For non-repeatable containers this value should be
	// negative
	int interval;

	// List of fields
	QList<TemplateField*> fields;

	// Widget representation of container
	QGroupBox *groupBox;

public:
    TemplateContainer();
	TemplateContainer(TemplateContainer * container);
	~TemplateContainer();

	int getX();
	void setX(int x);

	int getY();
	void setY(int y);

	int getWidth();
	void setWidth(int width);

	int getHeight();
	void setHeight(int height);

	int getInterval();
	void setInterval(int interval);

	QList<TemplateField*>& getFields();

	QGroupBox * getGroupBox();
	void createGroupBox(QWidget *parent = 0 );

};

#endif // TEMPLATECONTAINER_H
