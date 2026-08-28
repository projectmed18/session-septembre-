#ifndef STATISTIQUESCOURSDIALOG_H
#define STATISTIQUESCOURSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QtCharts/QChartView>
#include "coursdao.h"

class StatistiquesCoursDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatistiquesCoursDialog(QWidget *parent = nullptr);

private:
    void construireCamembertNiveaux(QTabWidget *tabs);
    void construireBarresRemplissage(QTabWidget *tabs);

    CoursDAO m_dao;
};

#endif // STATISTIQUESCOURSDIALOG_H
