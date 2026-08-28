#include "statistiquescoursdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QPainter>

StatistiquesCoursDialog::StatistiquesCoursDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Statistiques — Gestion des Cours");
    setMinimumSize(900, 580);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    QLabel *titre = new QLabel("Statistiques — Gestion des Cours", this);
    titre->setStyleSheet("font: bold 14pt 'Segoe UI'; color: #cba6f7; "
                         "padding-bottom: 8px; background: transparent;");
    mainLayout->addWidget(titre);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #313244; border-radius: 6px; }"
        "QTabBar::tab { background:#313244; color:#cdd6f4; padding:8px 20px;"
        "               font:9pt 'Segoe UI'; border-radius:4px; margin-right:4px; }"
        "QTabBar::tab:selected { background:#cba6f7; color:#1e1e2e;"
        "                        font:bold 9pt 'Segoe UI'; }"
        "QTabBar::tab:hover { background:#45475a; }");

    construireCamembertNiveaux(tabs);
    construireBarresRemplissage(tabs);

    mainLayout->addWidget(tabs);
    setLayout(mainLayout);
}

void StatistiquesCoursDialog::construireCamembertNiveaux(QTabWidget *tabs)
{
    const QList<Cours> tous = m_dao.afficherTous();

    int nDeb = 0, nInt = 0, nAv = 0;
    for (const Cours &c : tous) {
        if      (c.getNiveau() == "Debutant")      nDeb++;
        else if (c.getNiveau() == "Intermediaire") nInt++;
        else if (c.getNiveau() == "Avance")        nAv++;
    }

    QPieSeries *series = new QPieSeries;
    series->setHoleSize(0.35);

    auto addSlice = [&](const QString &lbl, int val, const QColor &col) {
        if (val == 0) return;
        QPieSlice *sl = series->append(
            QString("%1\n%2 cours").arg(lbl).arg(val), val);
        sl->setColor(col);
        sl->setLabelColor(Qt::white);
        sl->setLabelFont(QFont("Segoe UI", 9));
        sl->setLabelVisible(true);
        sl->setBorderColor(QColor("#1e1e2e"));
        sl->setBorderWidth(2);
    };

    addSlice("Debutant",      nDeb, QColor("#a6e3a1"));
    addSlice("Intermediaire", nInt, QColor("#f9e2af"));
    addSlice("Avance",        nAv,  QColor("#f38ba8"));

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("Répartition des cours par niveau");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setTitleBrush(QBrush(QColor("#cdd6f4")));
    chart->setBackgroundBrush(QBrush(QColor("#181825")));
    chart->setBackgroundRoundness(8);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#cdd6f4"));
    chart->legend()->setFont(QFont("Segoe UI", 9));
    chart->setAnimationOptions(QChart::AllAnimations);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background: transparent; border: none;");

    QLabel *resume = new QLabel(
        QString("  Total : %1 cours  |  Debutant : %2  |  Intermediaire : %3  |  Avance : %4")
            .arg(tous.size()).arg(nDeb).arg(nInt).arg(nAv));
    resume->setStyleSheet("color:#6c7086; font:8pt 'Segoe UI'; padding:6px;");

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    lay->addWidget(resume);
    page->setStyleSheet("background-color:#181825;");
    tabs->addTab(page, "Répartition par niveau");
}

void StatistiquesCoursDialog::construireBarresRemplissage(QTabWidget *tabs)
{
    const auto infos = m_dao.capaciteTousCours();

    QBarSet *setInscrits  = new QBarSet("Inscrits");
    QBarSet *setRestantes = new QBarSet("Places libres");
    setInscrits->setColor(QColor("#cba6f7"));
    setInscrits->setBorderColor(QColor("#1e1e2e"));
    setRestantes->setColor(QColor("#313244"));
    setRestantes->setBorderColor(QColor("#1e1e2e"));

    QStringList categories;
    for (const auto &c : infos) {
        // Tronquer l'intitulé si trop long
        QString lbl = c.intitule.length() > 14
            ? c.intitule.left(12) + "…" : c.intitule;
        categories << lbl;
        *setInscrits  << c.nbInscrits;
        *setRestantes << qMax(0, c.placesRestantes);
    }

    QBarSeries *series = new QBarSeries;
    series->append(setInscrits);
    series->append(setRestantes);
    series->setLabelsVisible(false);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("Taux de remplissage par cours");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setTitleBrush(QBrush(QColor("#cdd6f4")));
    chart->setBackgroundBrush(QBrush(QColor("#181825")));
    chart->setBackgroundRoundness(8);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#cdd6f4"));
    chart->legend()->setFont(QFont("Segoe UI", 9));

    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#cdd6f4"));
    axisX->setLabelsFont(QFont("Segoe UI", 8));
    axisX->setGridLineColor(QColor("#313244"));
    axisX->setLinePen(QPen(QColor("#45475a")));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#cdd6f4"));
    axisY->setLabelsFont(QFont("Segoe UI", 8));
    axisY->setGridLineColor(QColor("#313244"));
    axisY->setLinePen(QPen(QColor("#45475a")));
    axisY->setMin(0);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background: transparent; border: none;");

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    page->setStyleSheet("background-color:#181825;");
    tabs->addTab(page, "Taux de remplissage");
}
