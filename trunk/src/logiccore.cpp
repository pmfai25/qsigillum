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
	qDebug() << "getImage()";
}

void LogicCore::preprocess()
{
}

void LogicCore::segmentate()
{
}

void LogicCore::classify()
{
}

void LogicCore::saveResults()
{
}

void LogicCore::processAutomatedMode()
{
	qDebug() << "processAutomatedMode()";
}
