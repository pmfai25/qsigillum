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

#ifndef EDITORFORM_H
#define EDITORFORM_H

#include <QtGui/QMainWindow>

namespace Ui
{
    class EditorForm;
}

class EditorForm : public QMainWindow
{
    Q_OBJECT
public:
    EditorForm(QWidget *parent = 0);
    ~EditorForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditorForm *m_ui;
};

#endif // EDITORFORM_H
