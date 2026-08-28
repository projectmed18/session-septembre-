#include "classementcoursdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QFont>

ClassementCoursDialog::ClassementCoursDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Classement — Cours les plus demandés");
    setMinimumSize(860, 560);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 16);
    mainLayout->setSpacing(14);

    // ── En-tête ───────────────────────────────────────────────────
    QFrame *header = new QFrame(this);
    header->setStyleSheet("background-color: #89dceb; border-radius: 8px;");
    header->setFixedHeight(64);
    QHBoxLayout *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(20, 0, 20, 0);
    QLabel *titre = new QLabel(" Classement des cours par popularité");
    titre->setStyleSheet("color: #1e1e2e; font: bold 13pt 'Segoe UI'; background: transparent;");
    m_labelTotal = new QLabel();
    m_labelTotal->setStyleSheet("color: #1e1e2e; font: 9pt 'Segoe UI'; background: transparent;");
    hLay->addWidget(titre);
    hLay->addStretch();
    hLay->addWidget(m_labelTotal);
    mainLayout->addWidget(header);

    // ── Tableau ───────────────────────────────────────────────────
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {"Rang", "Intitulé", "Niveau", "Capacité", "Inscrits", "Places lib.", "Taux remplissage"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setStyleSheet(
        "QTableWidget { background-color:#181825; color:#cdd6f4;"
        "  gridline-color:#313244; border:1px solid #313244;"
        "  border-radius:6px; font:9pt 'Segoe UI'; outline:none; }"
        "QTableWidget::item { padding:6px 10px; border-bottom:1px solid #313244; }"
        "QTableWidget::item:selected { background-color:#313244; color:#89dceb; }"
        "QTableWidget::item:alternate { background-color:#252535; }"
        "QHeaderView::section { background-color:#181825; color:#89dceb;"
        "  font:bold 9pt 'Segoe UI'; padding:8px;"
        "  border:none; border-bottom:2px solid #89dceb; }");
    mainLayout->addWidget(m_table);

    // ── Fermer ────────────────────────────────────────────────────
    QHBoxLayout *botLay = new QHBoxLayout;
    botLay->addStretch();
    QPushButton *btnFermer = new QPushButton("Fermer", this);
    btnFermer->setFixedHeight(36);
    btnFermer->setStyleSheet(
        "QPushButton { background-color:#45475a; color:#cdd6f4;"
        "  font:9pt 'Segoe UI'; border-radius:6px; border:none; padding:0 14px; }"
        "QPushButton:hover { background-color:#585b70; }");
    connect(btnFermer, &QPushButton::clicked, this, &QDialog::accept);
    botLay->addWidget(btnFermer);
    mainLayout->addLayout(botLay);

    setLayout(mainLayout);
    chargerDonnees();
}

void ClassementCoursDialog::chargerDonnees()
{
    const auto liste = m_dao.classementCours();
    m_labelTotal->setText(QString("Total : %1 cours").arg(liste.size()));
    m_table->setRowCount(0);

    // Médailles pour le podium
    QStringList medailles = {"🥇", "🥈", "🥉"};

    for (const auto &c : liste) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setRowHeight(row, 44);

        auto item = [](const QString &txt,
                       Qt::Alignment align = Qt::AlignVCenter | Qt::AlignCenter) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        // Rang avec médaille pour le podium
        QString rangTxt = (c.rang <= 3)
            ? medailles[c.rang - 1] + "  " + QString::number(c.rang)
            : "" + QString::number(c.rang);
        QTableWidgetItem *rangItem = item(rangTxt);
        if (c.rang == 1) rangItem->setForeground(QColor("#f9e2af"));
        else if (c.rang == 2) rangItem->setForeground(QColor("#a6adc8"));
        else if (c.rang == 3) rangItem->setForeground(QColor("#fab387"));
        rangItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_table->setItem(row, 0, rangItem);

        m_table->setItem(row, 1, item(c.intitule, Qt::AlignVCenter | Qt::AlignLeft));

        // Badge niveau
        QTableWidgetItem *niv = item(c.niveau);
        if      (c.niveau == "Debutant")      niv->setForeground(QColor("#a6e3a1"));
        else if (c.niveau == "Intermediaire") niv->setForeground(QColor("#f9e2af"));
        else if (c.niveau == "Avance")        niv->setForeground(QColor("#f38ba8"));
        m_table->setItem(row, 2, niv);

        m_table->setItem(row, 3, item(QString::number(c.capaciteMax)));
        m_table->setItem(row, 4, item(QString::number(c.nbInscrits)));
        m_table->setItem(row, 5, item(QString::number(c.capaciteMax - c.nbInscrits)));

        // Barre de progression taux de remplissage
        QProgressBar *bar = new QProgressBar;
        bar->setMinimum(0);
        bar->setMaximum(100);
        bar->setValue(static_cast<int>(c.tauxRemplissage));
        bar->setFormat(QString("%1%").arg(static_cast<int>(c.tauxRemplissage)));
        bar->setTextVisible(true);
        bar->setFixedHeight(18);

        QString barColor;
        if (c.tauxRemplissage >= 100)      barColor = "#f38ba8";
        else if (c.tauxRemplissage >= 80)  barColor = "#fab387";
        else                                barColor = "#a6e3a1";

        bar->setStyleSheet(QString(
            "QProgressBar { background-color:#313244; border-radius:4px;"
            "  color:#1e1e2e; font:bold 8pt 'Segoe UI'; }"
            "QProgressBar::chunk { background-color:%1; border-radius:4px; }").arg(barColor));

        QWidget *cell = new QWidget;
        QHBoxLayout *cl = new QHBoxLayout(cell);
        cl->setContentsMargins(8, 8, 8, 8);
        cl->addWidget(bar);
        m_table->setCellWidget(row, 6, cell);
    }
}
