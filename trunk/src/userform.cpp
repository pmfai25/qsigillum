#include "userform.h"
#include "ui_userform.h"

UserForm::UserForm(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::UserForm)
{
    m_ui->setupUi(this);
}

UserForm::~UserForm()
{
    delete m_ui;
}

void UserForm::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
