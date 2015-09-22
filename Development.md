# Introduction #
QSigillum development is supposed to be executed using several technologies and conventions.

# General information #
  * C++ and [Qt 4.5](http://www.qtsoftware.com/products/) are used for development.
  * No exception handling!
  * SVN is used as central VCS. Nice tutorial is [here](http://svnbook.red-bean.com/).
  * SVN is used with username and password. You can see your password in your **Profile** (link in the top right of this page).
  * It is recommended to use [QtCreator](http://www.qtsoftware.com/products/developer-tools) for development. To get started with SVN usage in QtCreator, read this: http://www.qtcentre.org/forum/f-qt-software-16/t-solved-using-a-subversion-client-with-qtcreator-19375.html

# Coding convention #
  * Qt coding style is used as base style (with some modifications): http://qt.gitorious.org/qt/pages/QtCodingStyle
  * 4 tabs are used for indentation (not spaces!)
  * Allman style is used for braces: http://en.wikipedia.org/wiki/Indent_style#Allman_style_.28bsd_in_Emacs.29
  * Braces style exception for class declarations: keywords (such as `public:`) and macros (such as `Q_OBJECT`) are aligned at previous left brace level