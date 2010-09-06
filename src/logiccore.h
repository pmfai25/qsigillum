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

#ifndef LOGICCORE_H
#define LOGICCORE_H

#include <QtCore>
#include <QtGui>
#include <QPluginLoader>
#include <QtDebug>
#include <QList>
#include <QDir>
#include <QTranslator>
#include <QImage>
#include <QPixmap>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "userform.h"

#include "preprocessor.h"
#include "imageloader.h"
#include "classifier.h"
#include "outputexporter.h"
#include "segmentator.h"

class UserForm;

// Class incapsulating logic layer
class LogicCore : public QObject
{
	Q_OBJECT

public:
	LogicCore(UserForm *parent);

public slots:
	void getImage();
	void preprocess();
	void segmentate();
	void classify();
	void saveResults();
	void processAutomatedMode();

	// Delete container
	void deleteContainer();
	// Insert container
	void insertContainer();

private:
	UserForm *parent;

	QImage srcImage;

	Segmentator segmentator;
	Preprocessor preprocessor;

	QList<ImageLoader *> imageLoaders;
	QList<Classifier *> classifiers;
	QList<OutputExporter *> outputExporters;

	// Initialize all components
	void init();
};

#endif // LOGICCORE_H
