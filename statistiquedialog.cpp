#include "statistiquedialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

StatistiqueDialog::StatistiqueDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Statistiques des Cours — Centre de Formation");
    setMinimumSize(900, 580);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // Titre
    QLabel *titre = new QLabel("Statistiques — Gestion des Cours", this);
    titre->setStyleSheet("font: bold 14pt 'Segoe UI'; color: #89b4fa; "
                         "padding-bottom: 8px;");
    mainLayout->addWidget(titre);

    // Onglets
    QTabWidget *tabs = new QTabWidget(this);
    tabs->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #313244; border-radius: 6px; }"
        "QTabBar::tab { background: #313244; color: #cdd6f4; padding: 8px 20px; "
        "               font: 9pt 'Segoe UI'; border-radius: 4px; margin-right: 4px; }"
        "QTabBar::tab:selected { background: #89b4fa; color: #1e1e2e; font: bold 9pt 'Segoe UI'; }"
        "QTabBar::tab:hover { background: #45475a; }");

    construireCamembert(tabs);
    construireBarChart(tabs);

    mainLayout->addWidget(tabs);
    setLayout(mainLayout);
}

// ─── Camembert : répartition par niveau ───────────────────────────────────────
void StatistiqueDialog::construireCamembert(QTabWidget *tabs)
{
    QList<Cours> tous = m_dao.afficherTous();

    int nDebutant      = 0;
    int nIntermediaire = 0;
    int nAvance        = 0;

    for (const Cours &c : tous) {
        QString niv = c.getNiveau();
        if      (niv == "Débutant" || niv == "Debutant")           nDebutant++;
        else if (niv == "Intermédiaire" || niv == "Intermediaire") nIntermediaire++;
        else if (niv == "Avancé" || niv == "Avance")               nAvance++;
    }

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.35); // donut style

    auto ajouterTranche = [&](const QString &label, int val, const QColor &couleur) {
        if (val == 0) return;
        QPieSlice *slice = series->append(
            QString("%1\n%2 cours").arg(label).arg(val), val);
        slice->setColor(couleur);
        slice->setLabelColor(Qt::white);
        slice->setLabelFont(QFont("Segoe UI", 9));
        slice->setLabelVisible(true);
        slice->setBorderColor(QColor("#1e1e2e"));
        slice->setBorderWidth(2);
    };

    ajouterTranche("Débutant",      nDebutant,      QColor("#a6e3a1"));
    ajouterTranche("Intermédiaire", nIntermediaire, QColor("#f9e2af"));
    ajouterTranche("Avancé",        nAvance,        QColor("#f38ba8"));

    QChart *chart = new QChart();
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
        QString("  Total : %1 cours   |   Débutant : %2   |   "
                "Intermédiaire : %3   |   Avancé : %4")
            .arg(tous.size()).arg(nDebutant).arg(nIntermediaire).arg(nAvance));
    resume->setStyleSheet("color: #6c7086; font: 8pt 'Segoe UI'; padding: 6px;");

    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    lay->addWidget(resume);
    page->setStyleSheet("background-color: #181825;");

    tabs->addTab(page, "Répartition par niveau");
}

// ─── Bar Chart : capacité & durée des cours ───────────────────────────────────
void StatistiqueDialog::construireBarChart(QTabWidget *tabs)
{
    QList<Cours> tous = m_dao.afficherTous();

    QBarSet *barSetCapacite = new QBarSet("Capacité max");
    barSetCapacite->setColor(QColor("#89b4fa"));
    barSetCapacite->setBorderColor(QColor("#1e1e2e"));
    barSetCapacite->setLabelColor(QColor("#cdd6f4"));

    QBarSet *barSetDuree = new QBarSet("Durée (h)");
    barSetDuree->setColor(QColor("#cba6f7"));
    barSetDuree->setBorderColor(QColor("#1e1e2e"));
    barSetDuree->setLabelColor(QColor("#cdd6f4"));

    QStringList categories;
    int maxVal = 10;

    for (const Cours &c : tous) {
        QString titreCourt = c.getIntitule();
        if (titreCourt.length() > 15)
            titreCourt = titreCourt.left(12) + "...";
        categories << titreCourt;
        *barSetCapacite << c.getCapaciteMax();
        *barSetDuree << c.getDureeHeures();
        if (c.getCapaciteMax() > maxVal) maxVal = c.getCapaciteMax();
        if (c.getDureeHeures() > maxVal) maxVal = c.getDureeHeures();
    }

    QBarSeries *series = new QBarSeries();
    series->append(barSetCapacite);
    series->append(barSetDuree);
    series->setLabelsVisible(true);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Capacité et Durée des cours");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setTitleBrush(QBrush(QColor("#cdd6f4")));
    chart->setBackgroundBrush(QBrush(QColor("#181825")));
    chart->setBackgroundRoundness(8);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#cdd6f4"));
    chart->legend()->setFont(QFont("Segoe UI", 9));

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#cdd6f4"));
    axisX->setLabelsFont(QFont("Segoe UI", 8));
    axisX->setGridLineColor(QColor("#313244"));
    axisX->setLinePen(QPen(QColor("#45475a")));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#cdd6f4"));
    axisY->setLabelsFont(QFont("Segoe UI", 8));
    axisY->setGridLineColor(QColor("#313244"));
    axisY->setLinePen(QPen(QColor("#45475a")));
    axisY->setMin(0);
    axisY->setMax(maxVal + 5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background: transparent; border: none;");

    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    page->setStyleSheet("background-color: #181825;");

    tabs->addTab(page, "Capacité & Durée par cours");
}
