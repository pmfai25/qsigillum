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

#include "segmentator.h"

Segmentator::Segmentator()
	: scale_width(160), max_deviation(30000000), region_size(20)
{
	segTemplate = NULL;
	while (!body.isEmpty())
		delete body.takeFirst();
	width_ratio = 1;
	height_ratio = 1;
	rgbImage = QImage();
}

Segmentator::~Segmentator()
{
	delete segTemplate;
	while (!body.isEmpty())
		delete body.takeFirst();
}

void Segmentator::loadTemplate(const QString & fileName)
{
	while (!body.isEmpty())
		delete body.takeFirst();
	delete segTemplate;

	segTemplate = new SegmentationTemplate();
	segTemplate->loadFromFile(fileName);
	// Reset anchor
	anchor = QPoint();
	anchorFound = false;
}

void Segmentator::setImage(QImage * image)
{
	this->image = image;
	this->rgbImage = image->convertToFormat(QImage::Format_RGB32);

	// Set ratio
	if (image != NULL
		&& image->width() > 0 && image->height() > 0
		&& segTemplate != NULL
		&& segTemplate->getImageWidth() > 0
		&& segTemplate->getImageHeight() > 0)
	{
		width_ratio = double(image->width()) / segTemplate->getImageWidth();
		height_ratio = double(image->height()) / segTemplate->getImageHeight();
	}
}

void Segmentator::setPreprocessor(Preprocessor * preprocessor)
{
	this->preprocessor = preprocessor;
}

void Segmentator::segmentate()
{
	segmentateNew();
	return;

	if (!segTemplate || !image)
		return;

	while (!body.isEmpty())
	 delete body.takeFirst();

	// Scanning zone height
	int scan_height = 15;

	// Get real image anchor point
	searchAnchor();
	if (!anchorFound)
	{
		anchor = QPoint(segTemplate->getAnchorX(), segTemplate->getAnchorY());
	}

	//qDebug() << "anchor point: (" << anchor.x() << "; " << anchor.y()
	//		<< ") scan height:" << scan_height;

	// Now we should iterate through containers
	foreach (TemplateContainer * srcContainer, segTemplate->getBody())
	{
		// Check for emptiness and add one container if necessary
		TemplateContainer * temp = new TemplateContainer(srcContainer);
		// Fix coordinates using field template search
		temp->setX(temp->getX() + anchor.x());
		temp->setY(temp->getY() + anchor.y());
//		qDebug() << "original position" << QPoint(temp->getX(), temp->getY());
		QPoint containerPosition = getRealContainerAnchor(
				temp->getX(), temp->getY());
		if (!containerPosition.isNull())
		{
			temp->setX(containerPosition.x());
			temp->setY(containerPosition.y());
		}
//		qDebug() << "got c.anchor at" << containerPosition<<
//				"container at" << QPoint(temp->getX(), temp->getY());


		bool isNotEmpty = false;

		// Scanning borders
		int b1 = (temp->getY() - scan_height >= 0 )
				 ? (temp->getY() - scan_height) : 0 ;
		int b2 = (temp->getY() + scan_height < image->height())
				 ? (temp->getY() + scan_height)
				 : (image->height() - 1);

		// Local scanning
		for (int y = b1; y <= b2 ; y++)
		if (!emptyLine(y))
		{
			isNotEmpty = true;
			break;
		}

		if (isNotEmpty)
		{
			body.append(temp);
			//qDebug() << "container at (" << temp->getX() << "; "
			//	<< temp->getY() << ")";
		}
		else
			delete temp;

		// Check next container rows
		if (srcContainer->getInterval() >= 0)
		{
			// Data analysis success
			bool success;

			int i = srcContainer->getY() + anchor.y() + srcContainer->getHeight()
					+ srcContainer->getInterval();

			do
			{
				success = false;

				// Scanning borders
				int b1 = i;
				int b2 = (i + scan_height < image->height() )
						 ? (i + scan_height) : image->height() - 1;

				//qDebug() << "scanning from " << b1 << "to "
				//		<< b2 << " (height " << scan_height << ")";

				// Local scanning
				for (int y = b1; y <= b2 ; y++)
				{
					// Check for non-empty region
					if (!emptyLine(y))
					{
						success = true;
						i = y - 1;
						break;
					}
				}

				if (!success)
					break;

				// Appending template row
				TemplateContainer * temp = new TemplateContainer(srcContainer);
				QPoint containerPosition = getRealContainerAnchor(
						temp->getX() + anchor.x(), i);

				if (containerPosition.isNull())
				{
					temp->setY(i);
					temp->setX(temp->getX() + anchor.x());
				}
				else
				{

					temp->setX(containerPosition.x());
					temp->setY(containerPosition.y());
				}

				body.append(temp);
//				qDebug() << "got c.anchor at" << containerPosition
//						<< "container at" << QPoint(temp->getX(), temp->getY());


				// Incrementing vertical coordinate
				if (containerPosition.isNull())
					i += srcContainer->getHeight() + srcContainer->getInterval();
				else
					i = containerPosition.y() + srcContainer->getHeight()
						+ srcContainer->getInterval();

				// Update GUI to avoid freezing
				qApp->processEvents();

			} while (success);

		}
	}

}

// Check if source image line is empty
bool Segmentator::emptyLine(int y)
{
	if (!image)
		return true;

	int threshold = 200;

	for (int j = 0; j < image->width(); j++)
		if ((image->scanLine(y))[j] <= threshold)
			return false;

	return true;
}

// Analyze image and return real anchor coordinates
QPoint Segmentator::getRealAnchor()
{
	return anchor;
}

const QList<TemplateContainer *> & Segmentator::getBody()
{
	const QList<TemplateContainer *> & value = body;
	return value;
}

void Segmentator::dumpData()
{
	foreach (TemplateContainer * container, body)
	{
		qDebug() << "Container " << container->getWidth() << "x" <<
				container->getHeight() << " at (" <<
				container->getX() << ";" << container->getY() <<
				") / interval" << container->getInterval();

		foreach (TemplateField * field, container->getFields())
		{
			qDebug() << "\t Field " << field->getWidth() << "x" <<
				field->getHeight() << " at (" <<
				field->getX() << ";" << field->getY() << ")";
		}
	}

	qDebug() << "Total: " << body.length() << " containers";
}

void Segmentator::searchAnchor()
{
	anchor = QPoint();
	anchorFound = false;

	// Load anchor image
	QImage anchorImage = QImage(qApp->applicationDirPath()
								.append(QString("/../data/"))
								.append(segTemplate->getAnchorFileName()));

	if (anchorImage.isNull()
		|| segTemplate->getAnchorX() < 0
		|| segTemplate->getAnchorY() < 0)
		return;

	// Scale images
	double ratio = double(image->width()) / scale_width;
	int im_width = scale_width;
	int im_height = qRound(double(image->height()) / ratio);

	int a_width = qRound((double(segTemplate->getImageWidth()) / image->width())
						 * anchorImage.width() / ratio);
	int a_height = qRound((double(segTemplate->getImageHeight()) / image->height())
						 * anchorImage.height() / ratio);

	if (a_width > im_width || a_height > im_height)
		return;

	QImage imScaled = rgbImage.scaled(im_width, im_height,
									Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QImage aScaled = anchorImage.scaled(a_width, a_height,
									Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	// Deviation matrix size
	int m_height = imScaled.height() - aScaled.height();
	if (m_height <= 0)
		m_height = 1;
	int m_width = imScaled.width() - aScaled.width();
	if (m_width <= 0)
		m_width = 1;

	// Deviation matrix
	unsigned int * matrix = new unsigned int[m_width * m_height];
	if (!matrix)
		return;
	// Sum of deviations
	unsigned int sum = 0;

	// Deviation sums calculation
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			// Value initialization
			matrix[i * m_width + j] = max_deviation;
			sum = 0;

			// Calculate deviations
			for (int y = 0; y < a_height; y++)
				for (int x = 0; x < a_width; x++)
			{
				//sum += qAbs((imScaled.scanLine(i+y))[j+x]
				//			- (aScaled.scanLine(y))[x]);
				sum += qAbs(qGray(imScaled.pixel(j+x, i+y))
							- qGray(aScaled.pixel(x, y)));
				if (sum > max_deviation)
					sum = max_deviation;
			}

			// Value saving
			matrix[i * m_width + j] = sum;
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Searching for minimum value
	unsigned int min = matrix[0];
	unsigned int max = matrix[0];

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (matrix[i * m_width + j] < min)
			{
				min = matrix[i * m_width + j];
				anchor.setX(j);
				anchor.setY(i);
			}
			if (matrix[i * m_width + j] > max)
				max = matrix[i * m_width + j];
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	delete[] matrix;

	// Intermediate result
	int ix = qAbs(anchor.x() * ratio);
	int iy = qAbs(anchor.y() * ratio);

	/*qDebug() << "first pass result: (" << ix << "," << iy
			<< ") source (" << anchor.x() << anchor.y()
			<< ") min: " << min << " max: " << max
			<< " matrix size: (" << m_height << "," << m_width << ")";
	*/

	// Second pass region bounds
	int bl = (ix - region_size > 0) ? (ix - region_size) : 0;
	int br = (ix + region_size < image->width()) ? (ix + region_size) : image->width() - 1;
	int bt = (iy - region_size > 0) ? (iy - region_size) : 0;
	int bb = (iy + region_size < image->height()) ? (iy + region_size) : image->height() - 1;

	//qDebug() << "second pass bounds: " << bl << br << bt << bb;

	// Anchor image scaling
	a_width = qRound((double(segTemplate->getImageWidth()) / image->width())
						 * anchorImage.width());
	a_height = qRound((double(segTemplate->getImageHeight()) / image->height())
						 * anchorImage.height());
	aScaled = anchorImage.scaled(a_width, a_height,
										Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	// Second pass deviation matrix
	m_height = (bb - bt + 1) - region_size;
	if (m_height <= 0)
		m_height = 1;
	m_width = (br - bl + 1) - region_size;
	if (m_width <= 0)
		m_width = 1;
	matrix = new unsigned int[m_width * m_height];
	if (!matrix)
		return;

	// Deviations sum
	sum = 0;

	// Deviation sums calculation
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			// Value initialization
			matrix[i * m_width + j] = max_deviation;
			sum = 0;

			// Calculate deviations
			for (int y = 0; y < 2 * region_size; y++)
				for (int x = 0; x < 2 * region_size; x++)
			{
				if (bt+i+y < rgbImage.height() && bt+i+y >= 0
					&& bl+j+x < rgbImage.width() && bl+j+x >=0)
					sum += qAbs(qGray(rgbImage.pixel(bl+j+x, bt+i+y))
							- qGray(aScaled.pixel(x, y)));
				if (sum > max_deviation)
					sum = max_deviation;
			}

			// Value saving
			matrix[i * m_width + j] = sum;
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Searching for minimum value
	min = matrix[0];
	max = matrix[0];

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (matrix[i * m_width + j] < min)
			{
				min = matrix[i * m_width + j];
				anchor.setX(j);
				anchor.setY(i);
			}
			if (matrix[i * m_width + j] > max)
				max = matrix[i * m_width + j];
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	delete[] matrix;

	anchor.setX(anchor.x() + bl);
	anchor.setY(anchor.y() + bt);
	anchorFound = true;

	/*qDebug() << "second pass result: (" << anchor.x() << ", " << anchor.y()
			<< ") source: (" << anchor.x() - bl << anchor.y() - bt
			<< ") min: " << min << " max: " << max
			<< " matrix size: (" << m_height << ", " << m_width << ")";
	*/

}

void Segmentator::removeBodyElement(int element)
{
	if (element >= 0 && element < body.length())
	{
		delete body.takeAt(element);
	}
}

void Segmentator::duplicateBodyElement(int element)
{
	if (element >= 0 && element < body.length())
	{
		body.insert(element+1, new TemplateContainer(body.at(element)));
	}
}

QPoint Segmentator::getRealContainerAnchor(int raw_x, int raw_y)
{
	QPoint result;
	// Load field template
	QImage fieldImage = QImage(qApp->applicationDirPath()
								.append(QString("/../data/field.png")));
	// Region size
	int region_size_width = 15;
	int region_size_height = 15;

	// Region bounds
	int bl = (raw_x - region_size_width > 0) ? (raw_x - region_size_width) : 0;
	int br = (raw_x + region_size_width < rgbImage.width()) ?
			 (raw_x + region_size_width) : rgbImage.width() - 1;
	int bt = (raw_y - region_size_height > 0) ? (raw_y - region_size_height) : 0;
	int bb = (raw_y + region_size_height < rgbImage.height()) ?
			 (raw_y + region_size_height) : rgbImage.height() - 1;

	// Deviation matrix size
	int m_height = (bb - bt + 1) - region_size_height;
	if (m_height <= 0)
		m_height = 1;
	int m_width = (br - bl + 1) - region_size_width;
	if (m_width <= 0)
		m_width = 1;

	// Deviation matrix
	unsigned int * matrix = new unsigned int[m_width * m_height];
	if (!matrix)
		return result;

	// Sum of deviations
	unsigned int sum = 0;

	// Deviation sums calculation
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			// Value initialization
			matrix[i * m_width + j] = max_deviation;
			sum = 0;

			// Calculate deviations
			for (int y = 0; y < fieldImage.height(); y++)
				for (int x = 0; x < fieldImage.width(); x++)
			{
				if (bt+i+y < rgbImage.height() && bt+i+y >= 0
					&& bl+j+x < rgbImage.width() && bl+j+x >=0)
					sum += qAbs(qGray(rgbImage.pixel(bl+j+x, bt+i+y))
							- qGray(fieldImage.pixel(x, y)));
				if (sum > max_deviation)
					sum = max_deviation;
			}

			// Value saving
			matrix[i * m_width + j] = sum;
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	// Searching for minimum value
	unsigned int min = matrix[0];
	unsigned int max = matrix[0];

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (matrix[i * m_width + j] < min)
			{
				min = matrix[i * m_width + j];
				result.setX(j+bl+fieldImage.width());
				result.setY(i+bt);
			}
			if (matrix[i * m_width + j] > max)
				max = matrix[i * m_width + j];
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}


	delete[] matrix;

	return result;
}

// New version of segmentation routine
void Segmentator::segmentateNew()
{
	if (!segTemplate || !image)
		return;

	while (!body.isEmpty())
	 delete body.takeFirst();

	// Get real image anchor point
	searchAnchor();
	if (!anchorFound)
	{
		anchor = QPoint(segTemplate->getAnchorX(), segTemplate->getAnchorY());
	}
	//qDebug() << "anchor: " << anchor;

	// Binarization threshold
	int thresholdEmptyValue = 230;

	// Now we should iterate through containers
	foreach (TemplateContainer * srcContainer, segTemplate->getBody())
	{
		// Scanning start and end points; TODO: move these parametres to template
		int scanStopHorizontal = anchor.x() + srcContainer->getX() - 75;
		int scanStartVertical = anchor.y() + srcContainer->getY() - 15;
		int scanBorderLeft = anchor.x() + srcContainer->getX() - 25;
		int scanBorderRight = anchor.x() + srcContainer->getX() + 25;

		// Build vertical histogram
		QVector<int> verticalHistogram;

		// Number of continuous empty rows
		int continuousEmptyRowsNumber = 0;

		// Threshold dark points value
		int thresholdDarkPoints = 20;

		for (int i = scanStartVertical; i < image->height()
			&& continuousEmptyRowsNumber < 0.6 * srcContainer->getHeight(); i++)
		{
			int sum = 0;
			// Calculate number of dark points on row segment
			for (int j = anchor.x(); j < scanStopHorizontal; j++)
				if ((image->scanLine(i))[j] <= thresholdEmptyValue)
					sum++;
			// Save mean value
			verticalHistogram.append(sum);

			// Check for empty row condition
			if (sum >= thresholdDarkPoints)
				continuousEmptyRowsNumber = 0;
			else
				continuousEmptyRowsNumber++;

			// Update GUI to avoid freezing
			qApp->processEvents();
		}

		//qDebug() << "histogram size:" << verticalHistogram.size();

		// Extracting non-empty segments that are situated between empty segments
		QVector< QPair<int, int> > segments;
		QPair<int, int> temp = qMakePair(-1, -1);

		continuousEmptyRowsNumber = 0;
		// Minimum empty rows number threshold (is used to glue up neighbour segments)
		int emptyRowsThreshold = 10;
		// Last non-empty row index
		int lastNonEmptyIndex = 0;

		for (int i = 0; i < verticalHistogram.size(); i++)
		{
			// Check for non-empty row
			if (verticalHistogram.at(i) >= thresholdDarkPoints)
			{
				lastNonEmptyIndex = i;
				continuousEmptyRowsNumber = 0;

				// Create new segment if necessary
				if (temp.first < 0)
				{
					temp.first = i;
				}

			}
			else
			{
				continuousEmptyRowsNumber++;
				// Finish segment if necessary
				if (continuousEmptyRowsNumber >= emptyRowsThreshold
					&& temp.first >= 0)
				{
					temp.second = lastNonEmptyIndex;
					segments.append(temp);
					temp = qMakePair(-1, -1);
				}
			}

			// Update GUI to avoid freezing
			qApp->processEvents();
		}

		//qDebug() << "segments size:" << segments.size();

		// Threshold segment height
		int thresholdHeight = 0.3 * srcContainer->getHeight();

		// Previous horizontal coordinate for error correction
		int xPrevious = srcContainer->getX() + anchor.x();

		// Create container for each segment
		for (int i = 0; i < segments.size(); i++)
		{
//			qDebug() << "segment from" << segments.at(i).first + scanStartVertical
//					<< "to" << segments.at(i).second  + scanStartVertical
//					<< "- length" << segments.at(i).second - segments.at(i).first;

			if (segments.at(i).second - segments.at(i).first
				< thresholdHeight)
				continue;

			int y = scanStartVertical
					+ (segments.at(i).first + segments.at(i).second) / 2
					- srcContainer->getHeight() / 2;

			// If there are previous empty segments, calculation can be more accurate
			if (i > 0)
			{
				y = scanStartVertical
					+ (segments.at(i-1).second + segments.at(i).first) / 2;
			}

			TemplateContainer * temp = new TemplateContainer(srcContainer);
			temp->setY(y);
			temp->setX(srcContainer->getX() + anchor.x());

			// Scan for non-empty pixels to the right of segment
			int x = -1; int y_temp = -1;

			//qDebug() << "y from" << y + 0.25 * srcContainer->getHeight()
			//		<< "to" << y + 0.75 * srcContainer->getHeight()
			//		<< "- horiz. scan from" << scanBorderLeft << "to" << scanBorderRight;

			for (int j = scanBorderLeft; j < scanBorderRight; j++)
			{
			   for (int k = y + 0.25 * srcContainer->getHeight();
						k < y + 0.75 * srcContainer->getHeight(); k++)
				{
				   y_temp = k;

				   if ((image->scanLine(k))[j] < thresholdEmptyValue)
					{
						x = j;
						break;
					}

				}

			   if (x > -1)
				   break;

				// Update GUI to avoid freezing
				qApp->processEvents();
			}

			if (x > -1)
			{
				// Correct coordinate value
				x = double(x) * 0.25 + double(xPrevious) * 0.75;
				xPrevious = x;
			}
			else
				x = xPrevious;

			//qDebug() << "resulting x:" << x << "at y:" << y_temp;

			temp->setX(x);

			body.append(temp);

			// Update GUI to avoid freezing
			qApp->processEvents();
		}

	}

}
