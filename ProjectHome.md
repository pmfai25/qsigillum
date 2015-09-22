The goal of this project is creating free software with two main functions:
  * Retrieving pieces of data from a scanned document (an image) with predefined structure and saving the system of these image pieces as a document.
  * Recognition of handwritten digits in fields of this document.

Factically the software is intended for dean's office marksheets recognition.

Currently implemented features:
  * Retrieving images directly from scanner (using external CLI utilities) or from files (currently algorithms require 300 DPI input images)
  * Preprocessing of the whole sheet image (bulk noise removal)
  * Segmentating the image using XML document template, invariant image part (for correlation) and pixel data analysis
  * Digit images extraction and classification using radial-based (probabilistic) neural network
  * Some validation and GUI notification

The other planned features:
  * Export to PostreSQL DB

Development is based on C++ and Qt 4.6. The project is used primarily on GNU/Linux and MS Windows.
This project is run as scientific project by student of ICS ONPU, Odessa, Ukraine.