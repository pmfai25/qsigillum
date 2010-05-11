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

#include "logiccore.h"

LogicCore::LogicCore(UserForm *parent)
		: parent(parent)
{
	init();
}

LogicCore::~LogicCore()
{

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

	if (!newFileActions.isEmpty())
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
		// Update status message
		parent->getStatusBar()->showMessage(tr("Loading image..."));

		srcImage = sender->loadImage();

		if (!srcImage.isNull())
		{
			// Show preview
			QPixmap pixmap = QPixmap::fromImage(srcImage, Qt::MonoOnly)
							 .scaled(parent->getPreviewLabel()->size(),
							 Qt::KeepAspectRatio, Qt::FastTransformation);

			parent->getPreviewLabel()->setPixmap(pixmap);

			// Update status message
			parent->getStatusBar()->showMessage(tr("Image successfully loaded"), 2000);
		}
		else
			// Update status message
			parent->getStatusBar()->showMessage(tr("Image loading failed"), 2000);
	}

}

void LogicCore::preprocess()
{
	if (srcImage.isNull())
		return;

	// Update status message
	parent->getStatusBar()->showMessage(tr("Preprocessing image..."));

	// Remove dark fields
	srcImage = preprocessor.removeDarkFields(srcImage);

	if (!srcImage.isNull())
	{
		// Show preview
		QPixmap pixmap = QPixmap::fromImage(srcImage, Qt::MonoOnly)
						 .scaled(parent->getPreviewLabel()->size(),
						 Qt::KeepAspectRatio, Qt::FastTransformation);

		parent->getPreviewLabel()->setPixmap(pixmap);
	}

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image successfully preprocessed"), 2000);

}

void LogicCore::segmentate()
{
	if (srcImage.isNull())
		return;

	// Update status message
	parent->getStatusBar()->showMessage(tr("Segmentating image..."));

	QDir dir(qApp->applicationDirPath());
	segmentator.loadTemplate(dir.absoluteFilePath("../data/marksheet.xml"));
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

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image successfully segmentated"), 2000);
}

void LogicCore::classify()
{
	if (srcImage.isNull() || classifiers.isEmpty() || segmentator.getBody().isEmpty())
		return;

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image classification..."));

	int min_width = qRound(srcImage.width() * 0.004);
	int z = 0;

	// Traversing segments
	foreach (TemplateContainer * container, segmentator.getBody())
	{
		foreach (TemplateField * field, container->getFields())
		{
			// Extracting image part
			QImage part = preprocessor.removeDarkFields(
					srcImage.copy(container->getX() + field->getX(),
					container->getY() + field->getY(),
					field->getWidth(), field->getHeight()));

			// Subdividing fields onto digits and classifying them
			QString result;

			// Horizontal borders of digit
			int b1 = -1, b2 = 0;

			// Horizontal scanning
			for (int j = 1; j < part.width(); j++)
			{
				// Light-to-dark region transition
				if (!preprocessor.emptyColumn(part, j)
					&& preprocessor.emptyColumn(part, j-1))
				{
					b1 = j - 1;
				}
				// Dark-to-light region transition
				if (preprocessor.emptyColumn(part, j)
					&& !preprocessor.emptyColumn(part, j-1))
				{
					b2 = j;
				}

				// Digit extraction
				if (b1 > -1 && b2 > 0 && (b2 - b1) >= min_width)
				{
					//qDebug() << b1 << " 0 " << b2 << " " << part.height() << " z=" << z;

					//QImage q = part.copy(b1, 0, b2-b1+1, part.height());
					//q.save(QString("../data/trash/").append(QString::number(z)).append(QString(".jpg")));

					result.append(classifiers.at(0)
								  ->classify(part.copy
											 (b1, 0, b2-b1+1, part.height())));

					// Reset
					b1 = -1, b2 = 0;

					z++;
				}
			}

			// Updating label
			field->getLineEdit()->setText(result);

		}
	}

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image successfully classified"), 2000);
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
