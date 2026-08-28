#include "statistiquedialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>

StatistiqueDialog::StatistiqueDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Statistiques des Stagiaires");
    setMinimumSize(900, 580);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // Titre
    QLabel *titre = new QLabel("Statistiques — Gestion des Stagiaires", this);
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
    construireCourbe(tabs);

    mainLayout->addWidget(tabs);
    setLayout(mainLayout);
}

// ─── Camembert : répartition par niveau ───────────────────────────────────────
void StatistiqueDialog::construireCamembert(QTabWidget *tabs)
{
    // Récupérer tous les stagiaires
    QList<Stagiaire> tous = m_dao.afficherTous();

    int nDebutant      = 0;
    int nIntermediaire = 0;
    int nAvance        = 0;

    for (const Stagiaire &s : tous) {
        if      (s.getNiveau() == "Debutant")      nDebutant++;
        else if (s.getNiveau() == "Intermediaire") nIntermediaire++;
        else if (s.getNiveau() == "Avance")        nAvance++;
    }

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.35); // donut style

    auto ajouterTranche = [&](const QString &label, int val, const QColor &couleur) {
        if (val == 0) return;
        QPieSlice *slice = series->append(
            QString("%1\n%2 stagiaire(s)").arg(label).arg(val), val);
        slice->setColor(couleur);
        slice->setLabelColor(Qt::white);
        slice->setLabelFont(QFont("Segoe UI", 9));
        slice->setLabelVisible(true);
        slice->setBorderColor(QColor("#1e1e2e"));
        slice->setBorderWidth(2);
    };

    ajouterTranche("Debutant",      nDebutant,      QColor("#a6e3a1"));
    ajouterTranche("Intermediaire", nIntermediaire, QColor("#f9e2af"));
    ajouterTranche("Avance",        nAvance,        QColor("#f38ba8"));

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des stagiaires par niveau");
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

    // Résumé texte
    QLabel *resume = new QLabel(
        QString("  Total : %1 stagiaire(s)   |   Debutant : %2   |   "
                "Intermediaire : %3   |   Avance : %4")
            .arg(tous.size()).arg(nDebutant).arg(nIntermediaire).arg(nAvance));
    resume->setStyleSheet("color: #6c7086; font: 8pt 'Segoe UI'; padding: 6px;");

    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    lay->addWidget(resume);
    page->setStyleSheet("background-color: #181825;");

    tabs->addTab(page, "Répartition par niveau");
}

// ─── Courbe : évolution des inscriptions par mois ────────────────────────────
void StatistiqueDialog::construireCourbe(QTabWidget *tabs)
{
    QList<Stagiaire> tous = m_dao.afficherTous();

    // Compter les inscriptions par mois (année-mois)
    QMap<QString, int> parMois;
    for (const Stagiaire &s : tous) {
        QString cle = s.getDateInscription().toString("yyyy-MM");
        parMois[cle]++;
    }

    // Barres par mois
    QBarSet *barSet = new QBarSet("Inscriptions");
    barSet->setColor(QColor("#89b4fa"));
    barSet->setBorderColor(QColor("#1e1e2e"));
    barSet->setLabelColor(QColor("#cdd6f4"));

    QStringList categories;
    QList<QString> cles = parMois.keys();
    std::sort(cles.begin(), cles.end());

    for (const QString &cle : cles) {
        // Afficher mois/année lisible
        QDate d = QDate::fromString(cle + "-01", "yyyy-MM-dd");
        categories << d.toString("MMM yyyy");
        *barSet << parMois[cle];
    }

    QBarSeries *series = new QBarSeries();
    series->append(barSet);
    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Evolution des inscriptions par mois");
    chart->setTitleFont(QFont("Segoe UI", 12, QFont::Bold));
    chart->setTitleBrush(QBrush(QColor("#cdd6f4")));
    chart->setBackgroundBrush(QBrush(QColor("#181825")));
    chart->setBackgroundRoundness(8);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    // Axe X — catégories (mois)
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#cdd6f4"));
    axisX->setLabelsFont(QFont("Segoe UI", 8));
    axisX->setGridLineColor(QColor("#313244"));
    axisX->setLinePen(QPen(QColor("#45475a")));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Axe Y — nombre
    QValueAxis *axisY = new QValueAxis();
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

    QWidget *page = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->addWidget(view);
    page->setStyleSheet("background-color: #181825;");

    tabs->addTab(page, "Inscriptions par mois");
}
