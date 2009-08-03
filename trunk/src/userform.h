#ifndef USERFORM_H
#define USERFORM_H

#include <QtGui/QMainWindow>

namespace Ui {
    class UserForm;
}

class UserForm : public QMainWindow {
    Q_OBJECT
public:
    UserForm(QWidget *parent = 0);
    ~UserForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserForm *m_ui;
};

#endif // USERFORM_H
