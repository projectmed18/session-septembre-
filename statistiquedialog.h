#ifndef STATISTIQUEDIALOG_H
#define STATISTIQUEDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QLineSeries>
#include "stagiairedao.h"

class StatistiqueDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatistiqueDialog(QWidget *parent = nullptr);

private:
    void construireCamembert(QTabWidget *tabs);
    void construireCourbe(QTabWidget *tabs);

    StagiaireDAO m_dao;
};

#endif // STATISTIQUEDIALOG_H
