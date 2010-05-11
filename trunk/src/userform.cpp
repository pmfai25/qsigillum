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

#include "userform.h"
#include "ui_userform.h"

UserForm::UserForm(QWidget *parent) :
    QMainWindow(parent),
	m_ui(new Ui::UserForm)
{
    m_ui->setupUi(this);
	init();
}

UserForm::~UserForm()
{
	logicCore->quit();
	delete logicCore;

	delete m_ui;
}

// Init components
void UserForm::init()
{
	connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	logicCore = new LogicCore(this);
	connect(m_ui->actionPreprocessing,SIGNAL(triggered()), logicCore,
			SLOT(preprocess()));
	connect(m_ui->actionSegmentation,SIGNAL(triggered()), logicCore,
			SLOT(segmentate()));
	connect(m_ui->actionRecognition,SIGNAL(triggered()), logicCore,
			SLOT(classify()));
	logicCore->start();
	//logicCore->moveToThread(logicCore);
}

void UserForm::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QAction * UserForm::getFileMenuHead()
{
	return m_ui->menuFile->actions().at(0);
}

QVBoxLayout * UserForm::getToolbarLayout()
{
	return qobject_cast<QVBoxLayout *>(m_ui->groupBox->layout());
}

QMenu * UserForm::getMenu()
{
	return m_ui->menuFile;
}

QLabel * UserForm::getPreviewLabel()
{
	return m_ui->lbPreview;
}

QScrollArea * UserForm::getScrollArea()
{
	return m_ui->scrollArea;
}

QStatusBar * UserForm::getStatusBar()
{
	return m_ui->statusbar;
}

void UserForm::on_actionHelp_triggered()
{
	// Get help file URL
	QDir dir(qApp->applicationDirPath());
	QDesktopServices::openUrl(
			QUrl(QString("file:///").
				 append(dir.absoluteFilePath("../doc/help.htm"))));
}

void UserForm::on_actionAboutQt_triggered()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void UserForm::on_actionAboutApplication_triggered()
{
	QMessageBox::about(this,tr("About application"),
					   tr("<b>QSigillum</b> - Handwritten digits recognition for structured documents<br>Copyright 2009 Konstantin \"konst\" Kucher (konst.hex@gmail.com)"));
}
