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

LogicCore::LogicCore(UserForm * parent)
		: parent(parent)
{
	init();
}

// Initialize all components
void LogicCore::init()
{
	segmentator.setPreprocessor(&preprocessor);
	operationRunning = false;

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
		 QObject * plugin = loader.instance();

		 if (plugin)
		 {
			 qDebug() << "Loaded plugin: " << fileName;

			 ImageLoader * iImageLoader = qobject_cast<ImageLoader *>(plugin);
			 if (iImageLoader)
			 {
				 qDebug() << fileName << " is ImageLoader plugin";

				 imageLoaders.append(iImageLoader);

				 // Update translation
				 QTranslator * translator = new QTranslator();
				 // TODO: remove hardcoded russian translation value
				 translator->load(pluginsDir.absolutePath()+"/../res/"+
								  iImageLoader->getTranslationFileBaseName()
								  + "_ru.qm");
				 qApp->installTranslator(translator);

				 // Update menu
				 QAction * action = new QAction(iImageLoader->getMenuIcon(),
												iImageLoader->
												getLoadingDescription(),
												parent);
				 connect(action, SIGNAL(triggered()), plugin,
						 SIGNAL(activatedLoading()));
				 connect(plugin, SIGNAL(activatedLoading()),
						 this, SLOT(getImage()));
				 newFileActions.append(action);

				 // Update toolbar
				 QToolButton * button = new QToolButton();
				 button->setToolTip(iImageLoader->
									getLoadingProcessingDescription());
				 button->setIcon(iImageLoader->getMenuIcon());
				 button->setIconSize(QSize(48,48));
				 connect(button, SIGNAL(clicked()), plugin,
						 SIGNAL(activatedAutomatedProcessing()));
				 connect(plugin, SIGNAL(activatedAutomatedProcessing()),
						 this, SLOT(processAutomatedMode()));
				 parent->getToolbarLayout()->insertWidget(position++, button);

				 // Set status bar
				 iImageLoader->setStatusBar(parent->getStatusBar());
			 }

			 Classifier * iClassifier
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

	// Load application settings
	settings = new QSettings(qApp->applicationDirPath()
							 .append(QString("/../res/qsigillum.ini")), QSettings::IniFormat, this);

}

void LogicCore::getImage()
{
	if (operationRunning)
		return;
	operationRunning = true;

	ImageLoader * sender = qobject_cast<ImageLoader *>(this->sender());

	if (sender)
	{
		// Update status message
		parent->getStatusBar()->showMessage(tr("Loading image..."));

		srcImage = sender->loadImage();

		if (!srcImage.isNull())
		{
			// Convert image to 8-bit
			srcImage = preprocessor.grayscale(srcImage);

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

	operationRunning = false;
}

void LogicCore::preprocess()
{
	if (srcImage.isNull())
		return;

	if (operationRunning)
		return;
	operationRunning = true;

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

	operationRunning = false;
}

void LogicCore::segmentate()
{
	if (srcImage.isNull())
		return;

	if (operationRunning)
		return;
	operationRunning = true;

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

	// Container (i.e. row) counter
	int row = 1;

	foreach (TemplateContainer * container, segmentator.getBody())
	{
		QHBoxLayout * hLayout = new QHBoxLayout();
		container->createGroupBox(saParent);
		container->getGroupBox()->setLayout(hLayout);
		hLayout->addWidget(new QLabel
						   (QString::number(row++).leftJustified(2)));

		foreach (TemplateField * field, container->getFields())
		{
			field->createLineEdit(container->getGroupBox());
			hLayout->addWidget(field->getLineEdit());
		}

		// Add container removal button
		QPushButton * button = new QPushButton();
		button->setIcon(QIcon(":/list-remove.png"));
		button->setToolTip(tr("Remove row"));
		hLayout->addWidget(button);
		connect(button, SIGNAL(clicked()), this, SLOT(deleteContainer()));

		// Add container insertion button
		button = new QPushButton();
		button->setIcon(QIcon(":/insert-table.png"));
		button->setToolTip(tr("Insert row"));
		hLayout->addWidget(button);
		connect(button, SIGNAL(clicked()), this, SLOT(insertContainer()));

		saLayout->addWidget(container->getGroupBox());
	}

	parent->getScrollArea()->setWidget(saParent);

	// Update preview
	if (parent->getPreviewLabel()->pixmap())
	{
		QPixmap preview = parent->getPreviewLabel()->pixmap()->copy();
		QPainter painter(&preview);

		// Calculate preview to source image size ratio
		double preview_ratio = double(preview.width()) / srcImage.width();

		// Paint anchor point
		painter.setBrush(Qt::green);
		painter.drawEllipse(segmentator.getRealAnchor() * preview_ratio, 3, 3);

		// Paint template containers
		foreach (TemplateContainer * container, segmentator.getBody())
		{
			painter.drawRoundedRect(
					QRectF(container->getX() * preview_ratio,
						   container->getY() * preview_ratio,
						   container->getWidth() * preview_ratio,
						   container->getHeight() * preview_ratio), 3, 3);
		}

		parent->getPreviewLabel()->setPixmap(preview);
	}


	// Update status message
	parent->getStatusBar()->showMessage(tr("Image successfully segmentated"), 2000);

	operationRunning = false;
}

void LogicCore::classify()
{
	/*QImage test = preprocessor.binarize(QImage("../data/trash/a.bmp"));
	qDebug() << "Result: " << classifiers.at(0)->classify(test);
	return;*/

	/*// Testing classifier
	for (int i = 0; i < 100; i++)
	{
		int number = 0;
		QImage marked = preprocessor.markCC(preprocessor.erode(
				preprocessor.dilate(preprocessor.binarize(
				preprocessor.removeBorderTrash(
						(QImage(QString("../data/test/")
						.append(QString::number(i))
						.append(QString(".jpg")))))))), &number);

		// Subdividing fields onto digits and classifying them
		QString result;

		// If zero or one component is found, analysis is vain
		if (number <= 1)
		{
			result.append(classifiers.at(0)
						  ->classify(marked));
		}
		else
		{
			// Get fields parameters
			QVector< QVector<int> > fields = preprocessor.analyseComponents(marked, number);

			// Check field and add it to the list
			for (int n = 1; n <= number; n++)
			{
				QVector<int> field = fields[n];

				int h = field[3] - field[1];
				int w = field[2] - field[0];

				if (h <= 0 || w <= 0 || h * w < 50)
					continue;

				// Get image
				QImage temp(w + 10, h + 10, QImage::Format_RGB32);
				temp.fill(qRgb(255, 255, 255));
				for (int y = field[1]; y <= field[3]; y++)
				for (int x = field[0]; x <= field[2]; x++)
				{
					int label = qRed(marked.pixel(x, y));
					if (label == n)
						temp.setPixel(x - field[0] + 6, y - field[1] + 6, qRgb(0, 0, 0));
				}

				result.append(classifiers.at(0)
							  ->classify(temp));
		}
		qDebug() << i << ": " << result;
	}
	}
	return; */

	if (srcImage.isNull() || classifiers.isEmpty() || segmentator.getBody().isEmpty())
		return;

	if (operationRunning)
		return;
	operationRunning = true;

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image classification..."));
	// Status message progress bar
	QProgressBar * progressBar = new QProgressBar();
	progressBar->setRange(0, segmentator.getBody().length());
	progressBar->reset();
	parent->getStatusBar()->addWidget(progressBar);

	// Counters for debug output purposes
	int containerCounter = 0;
	int fieldCounter = 0;

	// Number of marked connected components
	int num = 0;
	// Size of field
	int h = 0;
	int w = 0;
	// Current label
	int label = 0;

	// Traversing segments
	foreach (TemplateContainer * container, segmentator.getBody())
	{
		containerCounter++;
		fieldCounter = 0;

		// Update progress bar
		progressBar->setValue(progressBar->value() + 1);

		foreach (TemplateField * field, container->getFields())
		{
			fieldCounter++;

			// Extracting image part
			QImage part = preprocessor.erode(
					preprocessor.dilate(preprocessor.binarize(
					preprocessor.removeBorderTrash(
					srcImage.copy(container->getX() + field->getX(),
					container->getY() + field->getY(),
					field->getWidth(), field->getHeight())))));

			/*int cor1 = container->getX() + field->getX();
			int cor2 = container->getY() + field->getY();
			int cor3 = field->getWidth();
			int cor4 = field->getHeight();
			QImage part0 = srcImage.copy(cor1, cor2, cor3, cor4);
			part0.save(QString("../data/trash/part-").
				   append(QString::number(progressBar->value())).append(QString("-zz")).
				   append(QString::number(zz)).
				   append(QString("-x")).append(QString::number(cor1)).
				   append(QString("-y")).append(QString::number(cor2)).
				   append(QString("-w")).append(QString::number(cor3)).
				   append(QString("-h")).append(QString::number(cor4)).
				   append(QString(".bmp")));*/

			num = 0;
			int* marked = preprocessor.markCC(part, &num);

			/*if (containerCounter == 2 && fieldCounter == 5)
			{
				qDebug() << container->getX() + field->getX()
						<< container->getY() + field->getY();

				for (int mi = 0; mi < part.height(); mi++)
				{
					for (int mj = 0; mj < part.width(); mj++)
						printf("%d ", marked[part.width() * mi + mj]);

					printf("\n");
				}

				part.save(QString("../data/trash/cntnr_").append(QString::number(containerCounter)).
				   append(QString("-fld_")).append(QString::number(fieldCounter)).
				   append(QString("-lbls_")).append(QString::number(num)).
				   append(QString(".bmp")));
			}*/
			/*if (zz == 0)
			{
				part.save(QString("../data/trash/part-").
								   append(QString::number(progressBar->value())).append(QString("-zz")).
								   append(QString::number(zz)).append(QString("-")).
								   append(QString::number(num)).append(QString(".bmp")));
				qDebug() << num;
			}*/

			// Subdividing fields onto digits and classifying them
			QString result;

			// If zero or one component is found, analysis is vain
			if (num <= 1)
			{
				result.append(classifiers.at(0)
							  ->classify(part));
			}
			else
			{
				// Get fields parameters
				QVector< QVector<int> > pfields = preprocessor.analyseComponents(part, marked, num);

				// Check field and add it to the list
				for (int n = 1; n <= num; n++)
				{
					QVector<int> pfield = pfields[n];

					h = pfield[3] - pfield[1];
					w = pfield[2] - pfield[0];

					// Validating field
					if (h <= 0 || w <= 0 || h * w < 800 || pfield[4] < 100
						|| pfield[4] > 1500)
						continue;

					// Get image
					QImage temp(w + 10, h + 10, QImage::Format_RGB32);
					temp.fill(qRgb(255, 255, 255));
					for (int y = pfield[1]; y <= pfield[3]; y++)
					for (int x = pfield[0]; x <= pfield[2]; x++)
					{
						label = marked[part.width() * y + x];
						if (label == n)
							temp.setPixel(x - pfield[0] + 6, y - pfield[1] + 6, qRgb(0, 0, 0));
					}

					/*temp.save(QString("../data/trash/temp-").
						   append(QString::number(progressBar->value())).append(QString("-zz")).
						   append(QString::number(zz)).append(QString("-")).
						   append(QString::number(z)).append(QString("-h")).
						   append(QString::number(h)).append(QString("-w")).
						   append(QString::number(w)).append(QString("-po")).
						   append(QString::number(pfield[4])).append(QString(".bmp")));
					QImage ppart = part.copy(pfield[0], pfield[1], w, h);
					ppart.save(QString("../data/trash/ppart-").
						   append(QString::number(progressBar->value())).append(QString("-zz")).
						   append(QString::number(zz)).append(QString("-")).
						   append(QString::number(z)).append(QString("-h")).
						   append(QString::number(h)).append(QString("-w")).
						   append(QString::number(w)).append(QString("-po")).
						   append(QString::number(pfield[4])).append(QString(".bmp")));
					*/

					result.append(classifiers.at(0)
								  ->classify(temp));
				}


			}

			if (marked)
				delete[] marked;

/*
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

					// QImage q = preprocessor.removeBorderTrash(
					//		part.copy(b1, 0, b2-b1+1, part.height()));
					QImage q = part.copy(b1, 0, b2-b1+1, part.height());
					q.save(QString("../data/trash/").
						   append(QString::number(z)).append(QString(".bmp")));

					result.append(classifiers.at(0)
								  ->classify(q));

					// Reset
					b1 = -1, b2 = 0;

					z++;
				}
			}
*/
			// Updating label
			field->getLineEdit()->setText(result);
			// Validating result
			if (result.length() <= 0 || result.length() > 3
				|| (result.length() == 3 && result.toInt() != 100))
			{
				QPalette p = field->getLineEdit()->palette();
				p.setColor(QPalette::Base, Qt::red);
				field->getLineEdit()->setPalette(p);
				connect(field->getLineEdit(), SIGNAL(editingFinished()),
						this, SLOT(changeFieldColor()));
			}

		}
	}

	// Update status message
	parent->getStatusBar()->showMessage(tr("Image successfully classified"), 2000);
	parent->getStatusBar()->removeWidget(progressBar);
	delete progressBar;

	operationRunning = false;
}

void LogicCore::saveResults()
{
	// Saving results to text file
	if (segmentator.getBody().isEmpty())
		return;

	if (operationRunning)
		return;
	operationRunning = true;

	QFile data(qApp->applicationDirPath().append(QString("/../data/output.txt")));
	if (data.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream out(&data);

		// Traversing segments
		foreach (TemplateContainer * container, segmentator.getBody())
		{
			foreach (TemplateField * field, container->getFields())
			{
				out << field->getLineEdit()->text();
			}
		}
	}

	operationRunning = false;
}

void LogicCore::processAutomatedMode()
{
	if (operationRunning)
		return;

	getImage();
	preprocess();
	segmentate();
	classify();
}

void LogicCore::deleteContainer()
{
	QPushButton * sender = qobject_cast<QPushButton *>(this->sender());
	if (sender)
	{
		// Check existing containers
		for (int i = 0; i < segmentator.getBody().length(); i++)
		{
			if (sender->parent() == segmentator.getBody().at(i)->getGroupBox())
			{
				sender->disconnect(SIGNAL(clicked()));
				delete segmentator.getBody().at(i)->getGroupBox();
				segmentator.removeBodyElement(i);
				return;
			}
		}
	}
}

void LogicCore::insertContainer()
{
	QPushButton * sender = qobject_cast<QPushButton *>(this->sender());
	if (sender)
	{
		// Check existing containers
		for (int i = 0; i < segmentator.getBody().length(); i++)
		{
			if (sender->parent() == segmentator.getBody().at(i)->getGroupBox())
			{
				// Duplicate TemplateContainer in body list
				segmentator.duplicateBodyElement(i);
				TemplateContainer * container = segmentator.getBody().value(i+1);

				// Widgets creating routine
				QHBoxLayout * hLayout = new QHBoxLayout();
				container->createGroupBox(parent->getScrollArea()->widget());
				container->getGroupBox()->setLayout(hLayout);

				foreach (TemplateField * field, container->getFields())
				{
					field->createLineEdit(container->getGroupBox());
					hLayout->addWidget(field->getLineEdit());
				}

				// Add container removal button
				QPushButton * button = new QPushButton();
				button->setIcon(QIcon(":/list-remove.png"));
				button->setToolTip(tr("Remove row"));
				hLayout->addWidget(button);
				connect(button, SIGNAL(clicked()), this, SLOT(deleteContainer()));

				// Add container insertion button
				button = new QPushButton();
				button->setIcon(QIcon(":/insert-table.png"));
				button->setToolTip(tr("Insert row"));
				hLayout->addWidget(button);
				connect(button, SIGNAL(clicked()), this, SLOT(insertContainer()));

				QVBoxLayout * saLayout = qobject_cast<QVBoxLayout *>
										 (parent->getScrollArea()->widget()->layout());
				saLayout->insertWidget(i+1, container->getGroupBox());

				return;
			}
		}
	}
}

void LogicCore::changeFieldColor()
{
	QLineEdit * sender = qobject_cast<QLineEdit *>(this->sender());
	if (sender)
	{
		sender->setPalette(QPalette());
	}
}
