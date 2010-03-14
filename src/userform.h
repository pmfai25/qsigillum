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

#ifndef USERFORM_H
#define USERFORM_H

#include <QtGui>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

#include "logiccore.h"

class LogicCore;

namespace Ui
{
    class UserForm;
}

class UserForm : public QMainWindow
{
    Q_OBJECT
public:
    UserForm(QWidget *parent = 0);
    ~UserForm();

	QMenu * getMenu();
	QAction * getFileMenuHead();
	QVBoxLayout * getToolbarLayout();
	QLabel * getPreviewLabel();
	QScrollArea * getScrollArea();

protected:
    void changeEvent(QEvent *e);

private:
	// Init components
	void init();


	Ui::UserForm *m_ui;
	LogicCore *logicCore;
};

#endif // USERFORM_H
