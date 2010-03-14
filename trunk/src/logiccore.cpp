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

#include "logiccore.h"

LogicCore::LogicCore(UserForm *parent)
		: parent(parent)
{
	init();
}

void LogicCore::run()
{
	exec();
}

// Initialize all components
void LogicCore::init()
{
	// List of actions to be inserted to File menu
	QList<QAction *> newFileActions;

	// Position in toolbar
	int position = 0;

	// Load plugins
	QDir pluginsDir = QDir(qApp->applicationDirPath());
	pluginsDir.cd("../plugins/");

	foreach (QString fileName, pluginsDir.entryList(QDir::Files))
	{
		 QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		 QObject *plugin = loader.instance();

		 if (plugin)
		 {
			 qDebug() << "Loaded plugin: " << fileName;

			 ImageLoader *iImageLoader = qobject_cast<ImageLoader *>(plugin);
			 if (iImageLoader)
			 {
				 qDebug() << fileName << " is ImageLoader plugin";

				 imageLoaders.append(iImageLoader);

				 // Update translation
				 QTranslator *translator = new QTranslator();
				 // TODO: remove hardcoded russian translation value
				 translator->load(pluginsDir.absolutePath()+"/"+
								  iImageLoader->getTranslationFileBaseName()
								  + "_ru.qm");
				 translator->moveToThread(qApp->thread());
				 qApp->installTranslator(translator);

				 // Update menu
				 QAction * action = new QAction(iImageLoader->getMenuIcon(),
												iImageLoader->
												getLoadingDescription(),
												parent);
				 action->moveToThread(qApp->thread());
				 connect(action, SIGNAL(triggered()), plugin,
						 SIGNAL(activatedLoading()));
				 connect(plugin, SIGNAL(activatedLoading()),
						 this, SLOT(getImage()));
				 newFileActions.append(action);

				 // Update toolbar
				 QToolButton* button = new QToolButton();
				 button->setToolTip(iImageLoader->
									getLoadingProcessingDescription());
				 button->setIcon(iImageLoader->getMenuIcon());
				 button->setIconSize(QSize(48,48));
				 button->moveToThread(qApp->thread());
				 connect(button, SIGNAL(clicked()), plugin,
						 SIGNAL(activatedAutomatedProcessing()));
				 connect(plugin, SIGNAL(activatedAutomatedProcessing()),
						 this, SLOT(processAutomatedMode()));
				 parent->getToolbarLayout()->insertWidget(position++, button);
			 }

			 Classifier *iClassifier
					 = qobject_cast<Classifier *>(plugin);
			 if (iClassifier)
			 {
				 qDebug() << fileName << " is Classifier plugin";

				 classifiers.append(iClassifier);
			 }

		 }
	 }

	if (! newFileActions.isEmpty())
	{
		QAction * insertPoint = parent->getFileMenuHead();

		parent->getMenu()->insertActions(insertPoint, newFileActions);
		parent->getMenu()->insertSeparator(insertPoint);
	}

}

void LogicCore::getImage()
{
	ImageLoader *sender = qobject_cast<ImageLoader *>(this->sender());

	if (sender)
	{
		srcImage = sender->loadImage();

		if (!srcImage.isNull())
		{
			// Show preview
			QPixmap pixmap = QPixmap::fromImage(srcImage, Qt::MonoOnly)
							 .scaled(parent->getPreviewLabel()->size(),
							 Qt::KeepAspectRatio, Qt::FastTransformation);

			parent->getPreviewLabel()->setPixmap(pixmap);
		}
	}

}

void LogicCore::preprocess()
{
}

void LogicCore::segmentate()
{
	QDir dir(qApp->applicationDirPath());
	segmentator.loadTemplate(dir.absoluteFilePath("../data/marksheet.xml"));
	qDebug() << "Loaded template";
	segmentator.setImage(&srcImage);
	segmentator.segmentate();

	// Create LineEdit widgets
	QWidget * saParent = new QWidget();
	QVBoxLayout * saLayout = new QVBoxLayout();
	saParent->setLayout(saLayout);

	foreach (TemplateContainer * container, segmentator.getBody())
	{
		QHBoxLayout * hLayout = new QHBoxLayout();
		container->createGroupBox(saParent);
		container->getGroupBox()->setLayout(hLayout);

		foreach (TemplateField * field, container->getFields())
		{
			field->createLineEdit(container->getGroupBox());
			hLayout->addWidget(field->getLineEdit());
		}

		saLayout->addWidget(container->getGroupBox());
	}

	parent->getScrollArea()->setWidget(saParent);
}

void LogicCore::classify()
{
	if (classifiers.isEmpty())
		return;

	qDebug() << "1.tif: " << classifiers.at(0)->
			classify(QImage("../data/1.tif"));

	qDebug() << "2.tif: " << classifiers.at(0)->
			classify(QImage("../data/2.tif"));
}

void LogicCore::saveResults()
{
}

void LogicCore::processAutomatedMode()
{
//  getImage();
//	preprocess();
//	segmentate();
//	classify();
}
