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

#include <QtGui/QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QLibraryInfo>
#include <QtDebug>

#include "userform.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(),
		QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&qtTranslator);

	QTranslator appTranslator;
	// Automatic locale loading is possible
	// If you need it, use Qt internationalization guide
	QDir dir(qApp->applicationDirPath());
	appTranslator.load(dir.absoluteFilePath("../res/qsigillum_ru.qm"));
	a.installTranslator(&appTranslator);

	UserForm form;
	form.show();
	return a.exec();
}
