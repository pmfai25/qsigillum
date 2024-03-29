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

#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <QtPlugin>

class QImage;

class Classifier
{
public:

	virtual ~Classifier() {}

	// Classify image data
	virtual QString classify(QImage image) = 0;

	// Get base plugin name
	virtual QString getBaseName() = 0;
};

Q_DECLARE_INTERFACE(Classifier,"qSigillum.Classifier/0.2");

#endif // CLASSIFICATOR_H
