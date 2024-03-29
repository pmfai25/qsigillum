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
	//this->trainClassifier();
	//return QString("");

	// Testing mode
	//init();
	//this->checkClassifier();
	//return QString("");

	// Features extraction mode
	//this->extractFeatures();
	//return QString("");

	// Checking input image
	if (image.isNull())
	{
		qDebug() << "Got empty image!";
		return QString("");
	}

	// Initialization and processing
	init();
	this->image = crop(image);
	if (image.isNull())
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
	//inputPattern.append(getOccupancyQ4());
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
	inputPattern.append(getOccupancyNC());
	inputPattern.append(getOccupancyCC());
	inputPattern.append(getOccupancySC());

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

			// Accumulate results
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
	result_threshold = 0.1;
	bin_threshold = 180;

	// Init color table
	initColorTable();

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
				   .append(QString("/../res/new_weights.dat")));
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

// Initialize color table
void PNNClassifier::initColorTable()
{
	// Set the color table
	grayColorTable.resize(256);
	for (int i = 0; i < 256; i++)
	{
		grayColorTable[i] = qRgb(i, i, i);
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

// Get relative distance from left border to a black point
// at top fifth
double PNNClassifier::getDistanceLeftBorderQv1()
{
	// Distance sum
	double sum = 0.0;

	for (int i = 0; i < int(image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += j;
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at second fifth
double PNNClassifier::getDistanceLeftBorderQv2()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(image.height() / 5); i < int(2 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += j;
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at third fifth
double PNNClassifier::getDistanceLeftBorderQv3()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(2 * image.height() / 5); i < int(3 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += j;
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at fourth fifth
double PNNClassifier::getDistanceLeftBorderQv4()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(3 * image.height() / 5); i < int(4 * image.height() / 5); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += j;
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from left border to a black point
// at bottom fifth
double PNNClassifier::getDistanceLeftBorderQv5()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(4 * image.height() / 5); i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += j;
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at top fifth
double PNNClassifier::getDistanceRightBorderQv1()
{
	// Distance sum
	double sum = 0.0;

	for (int i = 0; i < int(image.height() / 5); i++)
	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.width() - 1 - j);
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at second fifth
double PNNClassifier::getDistanceRightBorderQv2()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(image.height() / 5); i < int(2 * image.height() / 5); i++)
	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.width() - 1 - j);
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at third fifth
double PNNClassifier::getDistanceRightBorderQv3()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(2 * image.height() / 5); i < int(3 * image.height() / 5); i++)
	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.width() - 1 - j);
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at fourth fifth
double PNNClassifier::getDistanceRightBorderQv4()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(3 * image.height() / 5); i < int(4 * image.height() / 5); i++)
	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.width() - 1 - j);
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from right border to a black point
// at bottom fifth
double PNNClassifier::getDistanceRightBorderQv5()
{
	// Distance sum
	double sum = 0.0;

	for (int i = int(4 * image.height() / 5); i < image.height(); i++)
	for (int j = image.width() - 1; j >= 0; j--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.width() - 1 - j);
			break;
		}

		// If no black points are found at current line
		sum += image.width();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from top border to a black point
// at leftmost fifth
double PNNClassifier::getDistanceTopBorderQv1()
{
	// Distance sum
	double sum = 0.0;

	for (int j = 0; j < int(image.width() / 5); j++)
	for (int i = 0; i < image.height(); i++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += i;
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from top border to a black point
// at second fifth
double PNNClassifier::getDistanceTopBorderQv2()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(image.width() / 5); j < int(2 * image.width() / 5); j++)
	for (int i = 0; i < image.height(); i++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += i;
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from top border to a black point
// at third fifth
double PNNClassifier::getDistanceTopBorderQv3()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(2 * image.width() / 5); j < int(3 * image.width() / 5); j++)
	for (int i = 0; i < image.height(); i++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += i;
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from top border to a black point
// at fourth fifth
double PNNClassifier::getDistanceTopBorderQv4()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(3 * image.width() / 5); j < int(4 * image.width() / 5); j++)
	for (int i = 0; i < image.height(); i++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += i;
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from top border to a black point
// at rightmost fifth
double PNNClassifier::getDistanceTopBorderQv5()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(4 * image.width() / 5); j < image.width(); j++)
	for (int i = 0; i < image.height(); i++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += i;
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from bottom border to a black point
// at leftmost fifth
double PNNClassifier::getDistanceBottomBorderQv1()
{
	// Distance sum
	double sum = 0.0;

	for (int j = 0; j < int(image.width() / 5); j++)
	for (int i = image.height() - 1; i <= 0; i--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.height() - 1 - i);
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from bottom border to a black point
// at second fifth
double PNNClassifier::getDistanceBottomBorderQv2()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(image.width() / 5); j < int(2 * image.width() / 5); j++)
	for (int i = image.height() - 1; i <= 0; i--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.height() - 1 - i);
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from bottom border to a black point
// at third fifth
double PNNClassifier::getDistanceBottomBorderQv3()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(2 * image.width() / 5); j < int(3 * image.width() / 5); j++)
	for (int i = image.height() - 1; i <= 0; i--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.height() - 1 - i);
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from bottom border to a black point
// at fourth fifth
double PNNClassifier::getDistanceBottomBorderQv4()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(3 * image.width() / 5); j < int(4 * image.width() / 5); j++)
	for (int i = image.height() - 1; i <= 0; i--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.height() - 1 - i);
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get relative distance from bottom border to a black point
// at rightmost fifth
double PNNClassifier::getDistanceBottomBorderQv5()
{
	// Distance sum
	double sum = 0.0;

	for (int j = int(4 * image.width() / 5); j < image.width(); j++)
	for (int i = image.height() - 1; i <= 0; i--)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			sum += (image.height() - 1 - i);
			break;
		}

		// If no black points are found at current line
		sum += image.height();
	}

	if (sum >= 0.0)
		return sum / (double(image.width() * image.height()));
	else
		return 1.0;
}

// Get occupancy in north-west image ninth part
double PNNClassifier::getOccupancyNW()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i < qRound(double(image.height()) / 3); i++)
	for (int j = 0; j < qRound(double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in north-centre image ninth part
double PNNClassifier::getOccupancyNC()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i < qRound(double(image.height()) / 3); i++)
	for (int j = qRound(double(image.width()) / 3);
	j < qRound(2 * double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in north-east image ninth part
double PNNClassifier::getOccupancyNE()
{
	// Points counter
	int temp = 0;

	for (int i = 0; i < qRound(double(image.height()) / 3); i++)
	for (int j = qRound(2 * double(image.width()) / 3); j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in centre-west image ninth part
double PNNClassifier::getOccupancyCW()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(double(image.height()) / 3);
	i < qRound(2 * double(image.height()) / 3); i++)
	for (int j = 0; j < qRound(double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in centre-centre image ninth part
double PNNClassifier::getOccupancyCC()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(double(image.height()) / 3);
	i < qRound(2 * double(image.height()) / 3); i++)
	for (int j = qRound(double(image.width()) / 3);
	j < qRound(2 * double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in centre-east image ninth part
double PNNClassifier::getOccupancyCE()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(double(image.height()) / 3);
	i < qRound(2 * double(image.height()) / 3); i++)
	for (int j = qRound(2 * double(image.width()) / 3); j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in south-west image ninth part
double PNNClassifier::getOccupancySW()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(2 * double(image.height()) / 3); i < image.height(); i++)
	for (int j = 0; j < qRound(double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in south-centre image ninth part
double PNNClassifier::getOccupancySC()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(2 * double(image.height()) / 3); i < image.height(); i++)
	for (int j = qRound(double(image.width()) / 3);
	j < qRound(2 * double(image.width()) / 3); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get occupancy in south-east image ninth part
double PNNClassifier::getOccupancySE()
{
	// Points counter
	int temp = 0;

	for (int i = qRound(2 * double(image.height()) / 3); i < image.height(); i++)
	for (int j = qRound(2 * double(image.width()) / 3); j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			temp++;
		}
	}

	return (double(temp) / (image.height() * image.width() / 9));
}

// Get base plugin name
QString PNNClassifier::getBaseName()
{
	return QString("PNNClassifier");
}

// Convert image to grayscale format
QImage PNNClassifier::grayscale(QImage src)
{
	// This function is duplicating Preprocessor's one. It's a pity.
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

// Train classifier using MNIST testing set images
void PNNClassifier::trainClassifier()
{
	// Set parameters
	deviation = 0.3;
	result_threshold = 0.01;
	bin_threshold = 180;

	// Init color table
	initColorTable();

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

	// Go to training set directory
	QDir dir = QDir(qApp->applicationDirPath());
	dir.cd("/home/konst/coding/projects/mnist/training/");
	qDebug() << "path: " << dir.path();

	// Get file list
	QStringList filelist = dir.entryList(QStringList("*.bmp"));

	// Digit image width and height
	int part_size = 28;

	// Class image sheet containing a number of digit images
	QImage classImage;

	// Class image coordinates
	int class_x = 0;
	int class_y = 0;

	// Total number of patterns in class
	int total_number = 0;

	// Finish flag
	bool isFinished = false;

	// Looping over classes
	for (int l = 0; l < labels.size(); l++)
	{
		tempClass.clear();

		qDebug() << "Processing file " << filelist.at(l);

		// Load image
		classImage = QImage(dir.absoluteFilePath(filelist.at(l)));

		isFinished = false;
		class_x = 0;
		class_y = 0;
		total_number = 0;

		do
		{
			tempPattern.clear();

			// Get image part
			this->image = crop(autoRotate(grayscale(
					classImage.copy(class_x, class_y, part_size, part_size))));
			// Check image
			if (this->image.isNull())
			{
				isFinished = true;
				continue;
			}

			//qDebug() << "---- processing image " << l << "at x:" << class_x << "y:" << class_y;

			// Update counter
			total_number++;

			// Calculate features and append to vector
			tempPattern.append(getOccupancyVerticalT1());
			tempPattern.append(getOccupancyVerticalT2());
			tempPattern.append(getOccupancyVerticalT3());
			tempPattern.append(getOccupancyVerticalQv1());
			tempPattern.append(getOccupancyVerticalQv2());
			tempPattern.append(getOccupancyVerticalQv3());
			tempPattern.append(getOccupancyVerticalQv4());
			tempPattern.append(getOccupancyVerticalQv5());
			tempPattern.append(getOccupancyQ1());
			tempPattern.append(getOccupancyQ2());
			tempPattern.append(getOccupancyQ3());
			//tempPattern.append(getOccupancyQ4());
			tempPattern.append(getDistanceLeftBorder0p());
			tempPattern.append(getDistanceLeftBorder25p());
			tempPattern.append(getDistanceLeftBorder50p());
			tempPattern.append(getDistanceLeftBorder75p());
			tempPattern.append(getDistanceLeftBorder100p());
			tempPattern.append(getDistanceRightBorder0p());
			tempPattern.append(getDistanceRightBorder25p());
			tempPattern.append(getDistanceRightBorder50p());
			tempPattern.append(getDistanceRightBorder75p());
			tempPattern.append(getDistanceRightBorder100p());

//			tempPattern.append(getDistanceLeftBorderQv1());
//			tempPattern.append(getDistanceLeftBorderQv2());
//			tempPattern.append(getDistanceLeftBorderQv3());
//			tempPattern.append(getDistanceLeftBorderQv4());
//			tempPattern.append(getDistanceLeftBorderQv5());
//			tempPattern.append(getDistanceRightBorderQv1());
//			tempPattern.append(getDistanceRightBorderQv2());
//			tempPattern.append(getDistanceRightBorderQv3());
//			tempPattern.append(getDistanceRightBorderQv4());
//			tempPattern.append(getDistanceRightBorderQv5());
//			tempPattern.append(getDistanceTopBorderQv1());
//			tempPattern.append(getDistanceTopBorderQv2());
//			tempPattern.append(getDistanceTopBorderQv3());
//			tempPattern.append(getDistanceTopBorderQv4());
//			tempPattern.append(getDistanceTopBorderQv5());
//			tempPattern.append(getDistanceBottomBorderQv1());
//			tempPattern.append(getDistanceBottomBorderQv2());
//			tempPattern.append(getDistanceBottomBorderQv3());
//			tempPattern.append(getDistanceBottomBorderQv4());
//			tempPattern.append(getDistanceBottomBorderQv5());

//			tempPattern.append(getOccupancyNW());
			tempPattern.append(getOccupancyNC());
//			tempPattern.append(getOccupancyNE());
//			tempPattern.append(getOccupancyCW());
			tempPattern.append(getOccupancyCC());
//			tempPattern.append(getOccupancyCE());
//			tempPattern.append(getOccupancySW());
			tempPattern.append(getOccupancySC());
//			tempPattern.append(getOccupancySE());

			// Append pattern to class vector
			tempClass.append(tempPattern);

			// Update class image coordinates
			class_y += part_size;
			if (class_y >= classImage.height())
			{
				class_y = 0;
				class_x += part_size;
			}
			if (class_x >= classImage.width())
			{
				isFinished = true;
				continue;
			}

		// Class size threshold
		} while (!isFinished && total_number < 2000);

		// Append class to weights vector
		weights.append(tempClass);

	}

	qDebug() << "Write weights file...";
	QFile data(QString("../res/new_weights.dat"));
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
	qDebug() << "Training finished";

}

// Check classifier for training set
void PNNClassifier::checkClassifier()
{
	// Temp pattern features vector
	QVector<double> tempPattern;
	// Calculation results vector
	QVector<double> results;

	// Go to training set directory
	QDir dir = QDir(qApp->applicationDirPath());
	dir.cd("/home/konst/coding/projects/mnist/testing/");
	qDebug() << "PNNClassifier path: " << dir.path();

	// Get file list
	QStringList filelist = dir.entryList(QStringList("*.bmp"));

	// Digit image width and height
	int part_size = 28;

	// Class image sheet containing a number of digit images
	QImage classImage;

	// Class image coordinates
	int class_x = 0;
	int class_y = 0;

	// Finish flag
	bool isFinished = false;

	// Total number of patterns in class
	int total_number = 0;
	// Number of correctly recognized patterns
	int correct_number = 0;

	// Looping over classes
	for (int l = 0; l < labels.size(); l++)
	{
		// Load image
		classImage = QImage(dir.absoluteFilePath(filelist.at(l)));

		isFinished = false;
		class_x = 0;
		class_y = 0;

		total_number = 0;
		correct_number = 0;

		do
		{
			tempPattern.clear();

			// Get image part
			this->image = crop(autoRotate(grayscale(
					classImage.copy(class_x, class_y, part_size, part_size))));
			// Check image
			if (this->image.isNull())
			{
				isFinished = true;
				continue;
			}

			// Update class image counter
			total_number++;

			// Calculate features and append to vector
			tempPattern.append(getOccupancyVerticalT1());
			tempPattern.append(getOccupancyVerticalT2());
			tempPattern.append(getOccupancyVerticalT3());
			tempPattern.append(getOccupancyVerticalQv1());
			tempPattern.append(getOccupancyVerticalQv2());
			tempPattern.append(getOccupancyVerticalQv3());
			tempPattern.append(getOccupancyVerticalQv4());
			tempPattern.append(getOccupancyVerticalQv5());
			tempPattern.append(getOccupancyQ1());
			tempPattern.append(getOccupancyQ2());
			tempPattern.append(getOccupancyQ3());
			//tempPattern.append(getOccupancyQ4());
			tempPattern.append(getDistanceLeftBorder0p());
			tempPattern.append(getDistanceLeftBorder25p());
			tempPattern.append(getDistanceLeftBorder50p());
			tempPattern.append(getDistanceLeftBorder75p());
			tempPattern.append(getDistanceLeftBorder100p());
			tempPattern.append(getDistanceRightBorder0p());
			tempPattern.append(getDistanceRightBorder25p());
			tempPattern.append(getDistanceRightBorder50p());
			tempPattern.append(getDistanceRightBorder75p());
			tempPattern.append(getDistanceRightBorder100p());

//			tempPattern.append(getDistanceLeftBorderQv1());
//			tempPattern.append(getDistanceLeftBorderQv2());
//			tempPattern.append(getDistanceLeftBorderQv3());
//			tempPattern.append(getDistanceLeftBorderQv4());
//			tempPattern.append(getDistanceLeftBorderQv5());
//			tempPattern.append(getDistanceRightBorderQv1());
//			tempPattern.append(getDistanceRightBorderQv2());
//			tempPattern.append(getDistanceRightBorderQv3());
//			tempPattern.append(getDistanceRightBorderQv4());
//			tempPattern.append(getDistanceRightBorderQv5());
//			tempPattern.append(getDistanceTopBorderQv1());
//			tempPattern.append(getDistanceTopBorderQv2());
//			tempPattern.append(getDistanceTopBorderQv3());
//			tempPattern.append(getDistanceTopBorderQv4());
//			tempPattern.append(getDistanceTopBorderQv5());
//			tempPattern.append(getDistanceBottomBorderQv1());
//			tempPattern.append(getDistanceBottomBorderQv2());
//			tempPattern.append(getDistanceBottomBorderQv3());
//			tempPattern.append(getDistanceBottomBorderQv4());
//			tempPattern.append(getDistanceBottomBorderQv5());

//			tempPattern.append(getOccupancyNW());
			tempPattern.append(getOccupancyNC());
//			tempPattern.append(getOccupancyNE());
//			tempPattern.append(getOccupancyCW());
			tempPattern.append(getOccupancyCC());
//			tempPattern.append(getOccupancyCE());
//			tempPattern.append(getOccupancySW());
			tempPattern.append(getOccupancySC());
//			tempPattern.append(getOccupancySE());

			results.clear();
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
					features_number = (tempPattern.size() < weights.at(i).at(j).size())
									  ? tempPattern.size() : weights.at(i).at(j).size();

					// Looping over features vector
					for (int k = 0; k < features_number; k++)
					{
						temp += pow(weights.at(i).at(j).at(k) - tempPattern.at(k), 2);
					}

					// Accumulate results
					results[i] += exp(-temp/pow(deviation, 2));
				}
			}


			// Normalization is used to correct the difference in class sets' sizes
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

			//if (l == 4 || l == 5 || l == 7 || l == 8)
			//qDebug() << "digit:" << l << "max:" << max << "index:" << index;

			// Comparing value to threshold and checking label
			if (max >= result_threshold && index == l)
				correct_number++;

			// Update class image coordinates
			class_y += part_size;
			if (class_y >= classImage.height())
			{
				class_y = 0;
				class_x += part_size;
			}
			if (class_x >= classImage.width())
			{
				isFinished = true;
				continue;
			}

		//} while (!isFinished && total_number < 100);
		} while (!isFinished && total_number < 2000);

		// Output total statistics
		qDebug() << "class" << l << ":" << correct_number
				<< "/" << total_number << "("
				<< double(correct_number) / total_number << ")";

	}
}

// Compensate rotation for grayscale digit image
QImage PNNClassifier::autoRotate(const QImage& image)
{
	if (image.height() <= 1 || image.width() <= 1)
		return image;

	// POI counter
	int totalNumber = 0;

	// Mean coordinates of upper part
	double upperX = 0.0;
	double upperY = 0.0;

	// Analyze upper 25% of image
	for (int i = 0; i <= int(image.height() / 4); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			totalNumber++;
			upperX += (double) j;
			upperY += (double) i;
		}
	}

	// Calculate mean coordinates
	if (totalNumber <= 0)
		return image;

	upperX /= totalNumber;
	upperY /= totalNumber;

	totalNumber = 0;

	// Mean coordinates of lower part
	double lowerX = 0.0;
	double lowerY = 0.0;

	// Analyze lower 25% of image
	for (int i = int(3 * image.height() / 4); i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			totalNumber++;
			lowerX += (double) j;
			lowerY += (double) i;
		}
	}

	// Calculate mean coordinates
	if (totalNumber <= 0)
		return image;

	lowerX /= totalNumber;
	lowerY /= totalNumber;

	// Check horizontal difference
	if (abs(upperX - lowerX) < 1.0)
		return image;

	// Calculate rotation angle using norm value
	double angle = 0.2 * atan((upperY - lowerY) / (upperX - lowerX));

	// Create temporary image for pixel rotation
	QImage temp = QImage(qRound(image.width() * 1.5),
						 qRound(image.height() * 1.5), QImage::Format_Indexed8);
	temp.setColorTable(grayColorTable);
	temp.fill(255);

	// Rotation centre
	int x0 = image.width() / 2;
	int y0 = image.height() / 2;

	// Resulting image pixel coordinates
	int x = 0;
	int y = 0;

	// Rotation
	for (int i = 0; i < image.height(); i++)
	for (int j = 0; j < image.width(); j++)
	{
		if ((image.scanLine(i))[j] < bin_threshold)
		{
			x = x0 + qRound(double(j - x0) * cos(angle)
							- double(i - y0) * sin(angle));
			y = y0 + qRound(double(j - x0) * sin(angle)
							+ double(i - y0) * cos(angle));

			if (x >= 0 && x < temp.width()
				&& y >= 0 && y < temp.height())
			{
				(temp.scanLine(y))[x] = (image.scanLine(i))[j];
			}
		}
	}

	// Additional dilation to compensate sampling errors
	for (int i = 1; i < temp.height() - 1; i++)
	for (int j = 1; j < temp.width() - 1; j++)
	{
		if (((temp.scanLine(i))[j-1] < bin_threshold)
			&& ((temp.scanLine(i-1))[j] < bin_threshold)
			&& ((temp.scanLine(i+1))[j] < bin_threshold)
			&& ((temp.scanLine(i))[j+1] < bin_threshold))
		{
			(temp.scanLine(i))[j] = 0;
		}

	}

	return temp;
}

// Get and export features for analysis
void PNNClassifier::extractFeatures()
{
	// Set parameters
	deviation = 0.3;
	result_threshold = 0.01;
	bin_threshold = 180;

	// Init color table
	initColorTable();

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

	// Temp pattern class vector
	QVector< QVector<double> > tempClass;
	// Temp pattern features vector
	QVector<double> tempPattern;

	// Go to training set directory
	QDir dir = QDir(qApp->applicationDirPath());
	dir.cd("/home/konst/coding/projects/mnist/training/");
	qDebug() << "path: " << dir.path();

	// Get file list
	QStringList filelist = dir.entryList(QStringList("*.bmp"));

	// Digit image width and height
	int part_size = 28;

	// Class image sheet containing a number of digit images
	QImage classImage;

	// Class image coordinates
	int class_x = 0;
	int class_y = 0;

	// Total number of patterns in class
	int total_number = 0;

	// Finish flag
	bool isFinished = false;

	// Looping over classes
	for (int l = 0; l < labels.size(); l++)
	{
		tempClass.clear();

		qDebug() << "Processing file " << filelist.at(l);

		// Load image
		classImage = QImage(dir.absoluteFilePath(filelist.at(l)));

		isFinished = false;
		class_x = 0;
		class_y = 0;
		total_number = 0;

		do
		{
			tempPattern.clear();

			// Get image part
			this->image = crop(autoRotate(grayscale(
					classImage.copy(class_x, class_y, part_size, part_size))));
			// Check image
			if (this->image.isNull())
			{
				isFinished = true;
				continue;
			}

			// Update counter
			total_number++;

			// Calculate features and append to vector
			tempPattern.append(getOccupancyVerticalT1());
			tempPattern.append(getOccupancyVerticalT2());
			tempPattern.append(getOccupancyVerticalT3());
			tempPattern.append(getOccupancyVerticalQv1());
			tempPattern.append(getOccupancyVerticalQv2());
			tempPattern.append(getOccupancyVerticalQv3());
			tempPattern.append(getOccupancyVerticalQv4());
			tempPattern.append(getOccupancyVerticalQv5());
			tempPattern.append(getOccupancyQ1());
			tempPattern.append(getOccupancyQ2());
			tempPattern.append(getOccupancyQ3());
			tempPattern.append(getOccupancyQ4());
			tempPattern.append(getDistanceLeftBorder0p());
			tempPattern.append(getDistanceLeftBorder25p());
			tempPattern.append(getDistanceLeftBorder50p());
			tempPattern.append(getDistanceLeftBorder75p());
			tempPattern.append(getDistanceLeftBorder100p());
			tempPattern.append(getDistanceRightBorder0p());
			tempPattern.append(getDistanceRightBorder25p());
			tempPattern.append(getDistanceRightBorder50p());
			tempPattern.append(getDistanceRightBorder75p());
			tempPattern.append(getDistanceRightBorder100p());

			tempPattern.append(getOccupancyNW());
			tempPattern.append(getOccupancyNC());
			tempPattern.append(getOccupancyNE());
			tempPattern.append(getOccupancyCW());
			tempPattern.append(getOccupancyCC());
			tempPattern.append(getOccupancyCE());
			tempPattern.append(getOccupancySW());
			tempPattern.append(getOccupancySC());
			tempPattern.append(getOccupancySE());

			// Append pattern to class vector
			tempClass.append(tempPattern);

			// Update class image coordinates
			class_y += part_size;
			if (class_y >= classImage.height())
			{
				class_y = 0;
				class_x += part_size;
			}
			if (class_x >= classImage.width())
			{
				isFinished = true;
				continue;
			}

		// Class size threshold
		} while (!isFinished && total_number < 2000);

		QFile data(dir.absolutePath()
				   .append(QString("/../analysis/"))
				   .append(QString::number(l))
				   .append(QString(".dat")));
		qDebug() << "Writing file" << data.fileName();
		if (data.open(QFile::WriteOnly))
		{
			QTextStream stream(&data);

			// Write line by line
			QString line;

			// Looping over patterns
			for (int j = 0; j < tempClass.size(); j++)
			{
				// Looping over features vector
				for (int k = 0; k < tempClass.at(j).size(); k++)
				{
					stream << tempClass.at(j).at(k) << " ";
				}

				stream << "\n";
			}

			stream.flush();
			data.close();
		}

	}

	qDebug() << "Analysis finished";
}

Q_EXPORT_PLUGIN2(pnnclassifier, PNNClassifier)
