#ifndef CLASSEMENTCOURSDIALOG_H
#define CLASSEMENTCOURSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include "coursdao.h"

class ClassementCoursDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClassementCoursDialog(QWidget *parent = nullptr);

private:
    void chargerClassement();

    QTableWidget *m_tableau;
    CoursDAO      m_dao;
};

#endif // CLASSEMENTCOURSDIALOG_H
