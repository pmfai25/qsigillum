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

void Segmentator::segmentate()
{
	if (!segTemplate || !image)
		return;

	while (!body.isEmpty())
	 delete body.takeFirst();

	// Scanning zone height
	int scan_height = (segTemplate->getBody().length() > 0)
					  ? segTemplate->getBody().at(0)->getHeight() / 4
					  : 30;

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
		// Set correct coordinates
		temp->setX(temp->getX() + anchor.x());
		temp->setY(temp->getY() + anchor.y());

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
				temp->setY(i);
				temp->setX(temp->getX() + anchor.x());
				body.append(temp);
				//qDebug() << "container at (" << temp->getX() << "; "
				//		<< temp->getY() << ")";

				// Incrementing vertical coordinate
				i += srcContainer->getHeight() + srcContainer->getInterval();

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
		if (qGray(image->pixel(j, y)) <= threshold)
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
	// Load anchor image
	QImage anchorImage = QImage(QString("../data/")
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

	QImage imScaled = image->scaled(im_width, im_height,
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
	// Deviation sums calculation
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
			if (matrix[i * m_width + j] > min)
				max = matrix[i * m_width + j];
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	delete[] matrix;

	// Intermediate result
	int ix = qAbs(anchor.x() * ratio);
	int iy = qAbs(anchor.y() * ratio);

	/*qDebug() << "first pass result: (" << ix << ", " << iy
			<< ") min: " << min << " max: " << max
			<< " matrix size: (" << m_height << ", " << m_width << ")";
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
				sum += qAbs(qGray(image->pixel(bl+j+x, bt+i+y))
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
	// Deviation sums calculation
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
			if (matrix[i * m_width + j] > min)
				max = matrix[i * m_width + j];
		}

		// Update GUI to avoid freezing
		qApp->processEvents();
	}

	delete[] matrix;

	anchor.setX(anchor.x() + bl);
	anchor.setY(anchor.y() + bt);
	anchorFound = true;

	/*qDebug() << "second pass result: (" << result.x() << ", " << result.y()
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
