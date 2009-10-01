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
#include <QLibraryInfo>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>

#include "segmentationtemplate.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// We should allow only one application instance to work
	QTcpServer server;
	if (!server.listen(QHostAddress::LocalHost,16981))
		return 1;
	server.close();

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(),
		QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&qtTranslator);

	QTranslator appTranslator;
	// Automatic locale loading is possible
	// If you need it, use Qt internationalization guide
	appTranslator.load("../res/qsigillum_ru");
	a.installTranslator(&appTranslator);

	SegmentationTemplate temp;
	temp.loadFromFile("../data/marksheet.xml");
	temp.dumpData();

    //TestSystem w;
	//w.show();
	//return a.exec();
	return 0;
}
