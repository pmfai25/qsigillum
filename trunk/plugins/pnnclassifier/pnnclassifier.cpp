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

#include "pnnclassifier.h"

QString PNNClassifier::classify(QImage image)
{
	// Training mode
	//trainClassifier();
	//return QString();

	// Checking input image
	if (image.isNull())
	{
		qDebug() << "Got empty image!";
		return QString("");
	}

	// Initialization and processing
	init();
	this->image = crop(image);
	if (this->image.isNull())
		return QString("");

	// Features vector for input pattern
	QVector<double> inputPattern;
	// Calculate features and append to vector
	inputPattern.append(getOccupancyVerticalT1());
	inputPattern.append(getOccupancyVerticalT2());
	inputPattern.append(getOccupancyVerticalT3());
	inputPattern.append(getOccupancyVerticalQv1());
	inputPattern.append(getOccupancyVerticalQv2());
	inputPattern.append(getOccupancyVerticalQv3());
	inputPattern.append(getOccupancyVerticalQv4());
	inputPattern.append(getOccupancyVerticalQv5());
	inputPattern.append(getOccupancyQ1());
	inputPattern.append(getOccupancyQ2());
	inputPattern.append(getOccupancyQ3());
	inputPattern.append(getOccupancyQ4());
	inputPattern.append(getDistanceLeftBorder0p());
	inputPattern.append(getDistanceLeftBorder25p());
	inputPattern.append(getDistanceLeftBorder50p());
	inputPattern.append(getDistanceLeftBorder75p());
	inputPattern.append(getDistanceLeftBorder100p());
	inputPattern.append(getDistanceRightBorder0p());
	inputPattern.append(getDistanceRightBorder25p());
	inputPattern.append(getDistanceRightBorder50p());
	inputPattern.append(getDistanceRightBorder75p());
	inputPattern.append(getDistanceRightBorder100p());

	//qDebug() << "image size:" << this->image.size();
	//for (int i = 0; i < inputPattern.size(); i++)
	//	qDebug() << inputPattern.at(i);

	// Calculation results vector
	QVector<double> results;
	results.fill(0.0, weights.size());
	// Temp value
	double temp = 0.0;
	// Current features number
	int features_number = 0;

	// Activation function calculation
	for (int i = 0; i < weights.size(); i++)
	{
		// Adding values for each pattern
		for (int j = 0; j < weights.at(i).size(); j++)
		{
			temp = 0.0;
			// Get real features number
			features_number = (inputPattern.size() < weights.at(i).at(j).size())
							  ? inputPattern.size() : weights.at(i).at(j).size();

			// Looping over features vector
			for (int k = 0; k < features_number; k++)
			{
				temp += pow(weights.at(i).at(j).at(k) - inputPattern.at(k), 2);
			}

			results[i] += exp(-temp/pow(deviation, 2));
		}
	}

	// Searching for maximum value and its index
	int index = 0;
	double max = 0.0;
	for (int i = 0; i < results.size(); i++)
	{
		if (results.at(i) > max)
		{
			max = results.at(i);
			index = i;
		}
	}

	//qDebug() << "max:" << index << "value:" << max;

	// Comparing value to threshold
	if (max < result_threshold)
		return QString("");
	else
		return labels.at(index);
}

void PNNClassifier::init()
{
	// Set parameters
	deviation = 0.3;
	result_threshold = 0.01;
	bin_threshold = 180;

	// Set labels
	if (labels.size() <= 0)
	{
		labels.clear();
		labels.append(QString("0"));
		labels.append(QString("1"));
		labels.append(QString("2"));
		labels.append(QString("3"));
		labels.append(QString("4"));
		labels.append(QString("5"));
		labels.append(QString("6"));
		labels.append(QString("7"));
		labels.append(QString("8"));
		labels.append(QString("9"));
	}

	// Check for weights
	if (weights.size() <= 0)
	{
		// Try to load weights file
		QFile data(qApp->applicationDirPath()
				   .append(QString("/../res/weights.dat")));
		if (data.open(QFile::ReadOnly))
		{
			// Temp pattern class vector
			QVector< QVector<double> > tempClass;
			// Temp pattern features vector
			QVector<double> tempPattern;

			QTextStream stream(&data);

			// Read line by line
			QString line;
			do
			{
				line = stream.readLine();
				if (line.isNull())
					break;

				// Check line
				if (line.length() > 0)
				{
					// Add another pattern features vector
					QStringList numbers = line.split(" ", QString::KeepEmptyParts);
					for (int i = 0; i < numbers.size(); i++)
					{
						tempPattern.append(numbers.at(i).toDouble());
					}

					tempClass.append(tempPattern);
					tempPattern = QVector<double>();
				}
				else
				{
					// Create new pattern class vector
					weights.append(tempClass);
					tempClass = QVector< QVector<double> >();
				}

			} while (!line.isNull());

		}
		data.close();
	}
}

// Crop image
QImage PNNClassifier::crop(QImage image)
{
	// Crop borders
	int y_min = image.height()-1;
	int y_max = 0;
	int x_min = image.width()-1;
	int x_max = 0;

	// Loop on whole image
	for (int i = 0; i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		// Check pixel value
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			// Test values and assign new borders
			if (i < y_min)
				y_min = i;
			if (i > y_max)
				y_max = i;
			if (j < x_min)
				x_min = j;
			if (j > x_max)
				x_max = j;
		}
	}

	if (x_min >= x_max || y_min >= y_max)
		return QImage();
	else
		return image.copy(x_min, y_min, x_max - x_min + 1, y_max - y_min + 1);
}

// Get points occupancy in north-east image quarter
double PNNClassifier::getOccupancyQ1()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i <= int(image.height() / 2); i++)
	for (int j = int(image.width() / 2); j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 4));
}

// Get points occupancy in north-west image quarter
double PNNClassifier::getOccupancyQ2()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i <= int(image.height() / 2); i++)
	for (int j = 0; j <= int(image.width() / 2); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 4));
}

// Get points occupancy in south-west image quarter
double PNNClassifier::getOccupancyQ3()
{
	// Points counter
	int temp = 0;

	for (int i = int(image.height() / 2); i < image.height() ; i++)
	for (int j = 0; j <= int(image.width() / 2); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 4));
}

// Get points occupancy in south-east image quarter
double PNNClassifier::getOccupancyQ4()
{
	// Points counter
	int temp = 0;

	for (int i = int(image.height() / 2); i < image.height() ; i++)
	for (int j = int(image.width() / 2); j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 4));
}

// Get points occupancy in top third part
double PNNClassifier::getOccupancyVerticalT1()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i <= int(image.height() / 3); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 3));
}

// Get points occupancy in middle third part
double PNNClassifier::getOccupancyVerticalT2()
{
	// Points counter
	int temp = 0;

	for (int i = int(image.height() / 3); i <= int(2 * image.height() / 3); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 3));
}

// Get points occupancy in bottom third part
double PNNClassifier::getOccupancyVerticalT3()
{
	// Points counter
	int temp = 0;

	for (int i = int(2 * image.height() / 3); i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 3));
}

// Get points occupancy in top fifth part
double PNNClassifier::getOccupancyVerticalQv1()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i <= int(image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 5));
}

// Get points occupancy in second fifth part
double PNNClassifier::getOccupancyVerticalQv2()
{
	// Points counter
	int temp = 0;

	for (int i = int(image.height() / 5); i <= int(2 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 5));
}

// Get points occupancy in middle fifth part
double PNNClassifier::getOccupancyVerticalQv3()
{
	// Points counter
	int temp = 0;

	for (int i = int(2 * image.height() / 5); i <= int(3 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 5));
}

// Get points occupancy in fourth fifth part
double PNNClassifier::getOccupancyVerticalQv4()
{
	// Points counter
	int temp = 0;

	for (int i = int(3 * image.height() / 5); i <= int(4 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 5));
}

// Get points occupancy in bottom fifth part
double PNNClassifier::getOccupancyVerticalQv5()
{
	// Points counter
	int temp = 0;

	for (int i = int(4 * image.height() / 5); i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 5));
}

// Get relative distance from left border to a black point
// at height = 0%
double PNNClassifier::getDistanceLeftBorder0p()
{
	// Nearest point coordinate
	int x = -1;

	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(0))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at height = 25%
double PNNClassifier::getDistanceLeftBorder25p()
{
	// Nearest point coordinate
	int x = -1;

	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(int(image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at height = 50%
double PNNClassifier::getDistanceLeftBorder50p()
{
	// Nearest point coordinate
	int x = -1;

	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(int(2 * image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at height = 75%
double PNNClassifier::getDistanceLeftBorder75p()
{
	// Nearest point coordinate
	int x = -1;

	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(int(3 * image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at height = 100%
double PNNClassifier::getDistanceLeftBorder100p()
{
	// Nearest point coordinate
	int x = -1;

	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(image.height() -1))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at height = 0%
double PNNClassifier::getDistanceRightBorder0p()
{
	// Nearest point coordinate
	int x = image.width();

	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(0))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at height = 25%
double PNNClassifier::getDistanceRightBorder25p()
{
	// Nearest point coordinate
	int x = image.width();

	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(int(image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at height = 50%
double PNNClassifier::getDistanceRightBorder50p()
{
	// Nearest point coordinate
	int x = image.width();

	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(int(2 * image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at height = 75%
double PNNClassifier::getDistanceRightBorder75p()
{
	// Nearest point coordinate
	int x = image.width();

	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(int(3 * image.height() / 4)))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at height = 100%
double PNNClassifier::getDistanceRightBorder100p()
{
	// Nearest point coordinate
	int x = image.width();

	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(image.height() - 1))[j] < bin_threshold)
		{
			x = j;
			break;
		}
	}

	if (x >= 0 && x < image.width())
		return double(x) / image.width();
	else
		return 1.0;
}

// Get base plugin name
QString PNNClassifier::getBaseName()
{
	return QString("PNNClassifier");
}

// Convert image to grayscale format
QImage PNNClassifier::grayscale(QImage src)
{
	// This function is dublicating Preprocessor's one. It's a pity.
	QVector<QRgb> grayColorTable;

	// Set the color table
	grayColorTable.resize(256);
	for (int i = 0; i < 256; i++)
	{
		grayColorTable[i] = qRgb(i, i, i);
	}

	// Create 8-bit QImage and set appropriate color table
	QImage result = QImage(src.size(), QImage::Format_Indexed8);
	result.setColorTable(grayColorTable);

	// Loop on whole image
	for (int i = 0; i < result.height(); i++)
	for (int j = 0; j < result.width(); j++)
	{
		(result.scanLine(i))[j] = qGray(src.pixel(j, i));
	}

	return result;
}

// Train classifier
void PNNClassifier::trainClassifier()
{
	// Set parameters
	deviation = 0.3;
	result_threshold = 0.01;
	bin_threshold = 180;

	// Set labels
	if (labels.size() <= 0)
	{
		labels.clear();
		labels.append(QString("0"));
		labels.append(QString("1"));
		labels.append(QString("2"));
		labels.append(QString("3"));
		labels.append(QString("4"));
		labels.append(QString("5"));
		labels.append(QString("6"));
		labels.append(QString("7"));
		labels.append(QString("8"));
		labels.append(QString("9"));
	}

	// Reset weights
	weights.clear();

	// Temp pattern class vector
	QVector< QVector<double> > tempClass;
	// Temp pattern features vector
	QVector<double> tempPattern;

	QStringList filelist;

	// Go to training set directory
	QDir dir = QDir(qApp->applicationDirPath());
	dir.cd("../../digits-set/");
	qDebug() << "path: " << dir.path();

	// Looping over classes
	for (int l = 0; l < labels.size(); l++)
	{
		tempClass.clear();

		// Go to class directory
		dir.cd(labels.at(l));
		qDebug() << "Processing directory " << labels.at(l);
		filelist = dir.entryList(QStringList("*.bmp"));

		// Get files list
		for (int i = 0; i < filelist.size(); i++)
		{
			tempPattern.clear();

			qDebug() << "Processing file " << labels.at(l) << "/" << filelist.at(i);

			// Load file
			this->image = crop(grayscale(QImage(dir.absoluteFilePath(filelist.at(i)))));
			//qDebug() << "here1";
			// Calculate features and append to vector
			tempPattern.append(getOccupancyVerticalT1());
			//qDebug() << "here2";
			tempPattern.append(getOccupancyVerticalT2());
			//qDebug() << "here3";
			tempPattern.append(getOccupancyVerticalT3());
			//qDebug() << "here4";
			tempPattern.append(getOccupancyVerticalQv1());
			tempPattern.append(getOccupancyVerticalQv2());
			tempPattern.append(getOccupancyVerticalQv3());
			tempPattern.append(getOccupancyVerticalQv4());
			tempPattern.append(getOccupancyVerticalQv5());
			//qDebug() << "here5";
			tempPattern.append(getOccupancyQ1());
			//qDebug() << "here51";
			tempPattern.append(getOccupancyQ2());
			//qDebug() << "here52";
			tempPattern.append(getOccupancyQ3());
			//qDebug() << "here53";
			tempPattern.append(getOccupancyQ4());
			//qDebug() << "here6";
			tempPattern.append(getDistanceLeftBorder0p());
			tempPattern.append(getDistanceLeftBorder25p());
			tempPattern.append(getDistanceLeftBorder50p());
			tempPattern.append(getDistanceLeftBorder75p());
			tempPattern.append(getDistanceLeftBorder100p());
			//qDebug() << "here7";
			tempPattern.append(getDistanceRightBorder0p());
			tempPattern.append(getDistanceRightBorder25p());
			tempPattern.append(getDistanceRightBorder50p());
			tempPattern.append(getDistanceRightBorder75p());
			tempPattern.append(getDistanceRightBorder100p());
			//qDebug() << "here8";

			// Append pattern to class vector
			tempClass.append(tempPattern);
		}

		// Append class to weights vector
		weights.append(tempClass);

		dir.cd("../");
	}

	qDebug() << "Write weights file...";
	QFile data(qApp->applicationDirPath()
			   .append(QString("/../res/weights.dat")));
	if (data.open(QFile::WriteOnly))
	{
		QTextStream stream(&data);

		// Write line by line
		QString line;

		// Looping over classes
		for (int i = 0; i < weights.size(); i++)
		{
			// Looping over patterns
			for (int j = 0; j < weights.at(i).size(); j++)
			{
				// Looping over features vector
				for (int k = 0; k < weights.at(i).at(j).size(); k++)
				{
					stream << weights.at(i).at(j).at(k) << " ";
				}

				stream << "\n";
			}

			stream << "\n";
		}

		stream.flush();
		data.close();
}

}

Q_EXPORT_PLUGIN2(pnnclassifier, PNNClassifier)
