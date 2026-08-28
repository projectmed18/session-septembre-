#ifndef CLASSEMENTCOURSDIALOG_H
#define CLASSEMENTCOURSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include "coursdao.h"

class ClassementCoursDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClassementCoursDialog(QWidget *parent = nullptr);

private:
    void chargerDonnees();

    QTableWidget *m_table;
    QLabel       *m_labelTotal;
    CoursDAO      m_dao;
};

#endif // CLASSEMENTCOURSDIALOG_H
