#ifndef ALERTESCOURSDIALOG_H
#define ALERTESCOURSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include "coursdao.h"

class AlertesCoursDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AlertesCoursDialog(QWidget *parent = nullptr);

private:
    void chargerDonnees();

    QTableWidget *m_table;
    QLabel       *m_valComplet;
    QLabel       *m_valAlerte;
    QLabel       *m_valOk;
    CoursDAO      m_dao;
};

#endif // ALERTESCOURSDIALOG_H
