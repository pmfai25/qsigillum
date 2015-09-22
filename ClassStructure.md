# Introduction #
QSigillum structure consists of core module and external plugins.
This is the current concept of class diagram for QSigillum:
![http://qsigillum.googlecode.com/files/ClassDiagram.png](http://qsigillum.googlecode.com/files/ClassDiagram.png)
It is supposed that **UserForm** will be loaded on default application launch; to use editor mode and to load **EditorForm**, application should be invoked with command-line parameter "--editor".


# Core module classes details #
  * **UserForm** contains controls for retrieving image, processing it and saving output data. It should provide simplified GUI with abilities to launch routine in one click. Also it should provide access to separate user functions via menu.
  * **EditorForm** is used for editing segmentation templates - i.e., user document layouts.
  * **LogicCore** is the interface to business logic layer from UI layer. It controls utilitary classes and contains intermediate and output data itself.
  * **Preprocessor** provides functions for converting image to B&W, noise removal, etc.
  * **Segmentator**  uses **SegmentationTemplate** object to retrieve list of image fragments with their relative coordinates.
  * **SegmentationTemplate** is structure that contains list of containers and defines input image requirements.
  * **TemplateContainer** is a list of fields that can be imagined as a rectangle or a row on image. It can be marked as repeatable.
  * **TemplateField** represents possible input data fields to be recognized. It defines geometry, value and visual representation (as a text edit widget).

# Core module interfaces details #
  * **ImageLoader** returns image for given filename string (this string can be ignored in some cases).
  * **Classifier** returns number (integer value) for given image.
  * **OutputExporter** saves output data to external file and/or executes some other export operations.

# Plugins details #
  * **FileImageLoader** loads image from existing file. As this class will use Qt, various formats will be supported.
  * **ScannerImageLoader** loads image from scanner. As for this class, it will probably use some external libraries and may be platform-dependent.
  * **ANNClassifier** is implementation of ANN.
  * **TextFileExporter** saves **OutputData** object line by line to plain text file.
  * **SQLQueryExporter** constructs SQL query and saves it to file (maybe DB call will be implemented too).