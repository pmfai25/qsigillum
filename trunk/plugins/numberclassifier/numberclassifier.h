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

#ifndef NUMBERCLASSIFIER_H
#define NUMBERCLASSIFIER_H

#include <QtPlugin>
#include <QtGui>
#include <QImage>
#include <QString>

#include "../../src/classifier.h"

class NumberClassifier : public QObject, public Classifier
{
	Q_OBJECT
	Q_INTERFACES(Classifier)

public:
		// Classify image data
		QString classify(QImage image);

};

#endif // NUMBERCLASSIFIER_H
