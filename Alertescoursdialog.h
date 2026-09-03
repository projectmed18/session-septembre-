#ifndef ALERTESCOURSDIALOG_H
#define ALERTESCOURSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include "coursdao.h"

class AlertesCoursDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlertesCoursDialog(QWidget *parent = nullptr);

private:
    void chargerAlertes();
    QListWidget *m_liste;
    CoursDAO m_dao;
};

#endif // ALERTESCOURSDIALOG_H
