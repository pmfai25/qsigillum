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

#ifndef TEMPLATEFIELD_H
#define TEMPLATEFIELD_H

#include <QLineEdit>

class TemplateField : public QObject
{
private:
	// Horizontal coordinate in container's coordinate system
	int x;
	// Vertical coordinate in container's coordinate system
	int y;
	int width;
	int height;
	// Widget that represents data field on document
	QLineEdit *lineEdit;

public:
    TemplateField();
	TemplateField(TemplateField * field);

	int getX();
	void setX(int x);

	int getY();
	void setY(int y);

	int getWidth();
	void setWidth(int width);

	int getHeight();
	void setHeight(int height);

	QLineEdit * getLineEdit();
	void createLineEdit(QWidget *parent = 0 );
};

#endif // TEMPLATEFIELD_H
