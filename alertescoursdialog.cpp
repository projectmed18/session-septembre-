#include "alertescoursdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFont>

AlertesCoursDialog::AlertesCoursDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Alertes Capacité — Cours");
    setMinimumSize(860, 560);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 16);
    mainLayout->setSpacing(14);

    // ── En-tête ───────────────────────────────────────────────────
    QFrame *header = new QFrame(this);
    header->setStyleSheet("background-color: #f38ba8; border-radius: 8px;");
    header->setFixedHeight(64);
    QHBoxLayout *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(20, 0, 20, 0);
    QLabel *titre = new QLabel("  Alertes — Cours Complets & Presque Complets");
    titre->setStyleSheet("color: #1e1e2e; font: bold 13pt 'Segoe UI'; background: transparent;");
    hLay->addWidget(titre);
    mainLayout->addWidget(header);

    // ── Cartes résumé ─────────────────────────────────────────────
    QHBoxLayout *cardsLay = new QHBoxLayout;
    cardsLay->setSpacing(12);

    auto creerCarte = [&](QLabel *&valLabel, const QString &lbl,
                          const QString &couleur) -> QFrame* {
        QFrame *card = new QFrame;
        card->setStyleSheet(QString(
            "background-color:#181825; border-radius:8px;"
            "border-left:4px solid %1;").arg(couleur));
        card->setFixedHeight(68);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 6, 16, 6);
        valLabel = new QLabel("0");
        valLabel->setStyleSheet(QString("color:%1; font:bold 18pt 'Segoe UI';"
                                        "background:transparent;").arg(couleur));
        QLabel *l = new QLabel(lbl);
        l->setStyleSheet("color:#6c7086; font:8pt 'Segoe UI'; background:transparent;");
        cl->addWidget(valLabel);
        cl->addWidget(l);
        return card;
    };

    cardsLay->addWidget(creerCarte(m_valComplet, "Cours complets",           "#f38ba8"));
    cardsLay->addWidget(creerCarte(m_valAlerte,  "Presque complets (< 20%)", "#fab387"));
    cardsLay->addWidget(creerCarte(m_valOk,      "Cours disponibles",        "#a6e3a1"));
    mainLayout->addLayout(cardsLay);

    // ── Tableau ───────────────────────────────────────────────────
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {"Intitulé", "Niveau", "Date début", "Date fin",
         "Capacité", "Inscrits", "État"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setStyleSheet(
        "QTableWidget { background-color:#181825; color:#cdd6f4;"
        "  gridline-color:#313244; border:1px solid #313244;"
        "  border-radius:6px; font:9pt 'Segoe UI'; outline:none; }"
        "QTableWidget::item { padding:6px 10px; border-bottom:1px solid #313244; }"
        "QTableWidget::item:selected { background-color:#313244; color:#f38ba8; }"
        "QTableWidget::item:alternate { background-color:#252535; }"
        "QHeaderView::section { background-color:#181825; color:#f38ba8;"
        "  font:bold 9pt 'Segoe UI'; padding:8px;"
        "  border:none; border-bottom:2px solid #f38ba8; }");
    mainLayout->addWidget(m_table);

    // ── Légende + Fermer ──────────────────────────────────────────
    QHBoxLayout *botLay = new QHBoxLayout;
    auto dot = [&](const QString &c, const QString &t) {
        QLabel *l = new QLabel(QString("● %1").arg(t));
        l->setStyleSheet(QString("color:%1; font:8pt 'Segoe UI';").arg(c));
        botLay->addWidget(l);
    };
    dot("#f38ba8", "Complet");
    dot("#fab387", "Alerte (< 20% places)");
    dot("#a6e3a1", "Disponible");
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

void AlertesCoursDialog::chargerDonnees()
{
    const auto liste = m_dao.capaciteTousCours();

    int complet = 0, alerte = 0, ok = 0;
    m_table->setRowCount(0);

    for (const auto &c : liste) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setRowHeight(row, 36);

        auto item = [](const QString &txt,
                       Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        m_table->setItem(row, 0, item(c.intitule));

        // Badge niveau
        QTableWidgetItem *niv = new QTableWidgetItem(c.niveau);
        niv->setTextAlignment(Qt::AlignCenter);
        niv->setFlags(niv->flags() & ~Qt::ItemIsEditable);
        if      (c.niveau == "Debutant")      niv->setForeground(QColor("#a6e3a1"));
        else if (c.niveau == "Intermediaire") niv->setForeground(QColor("#f9e2af"));
        else if (c.niveau == "Avance")        niv->setForeground(QColor("#f38ba8"));
        m_table->setItem(row, 1, niv);

        m_table->setItem(row, 2, item(c.dateDebut.toString("dd/MM/yyyy"), Qt::AlignCenter));
        m_table->setItem(row, 3, item(c.dateFin.toString("dd/MM/yyyy"),   Qt::AlignCenter));
        m_table->setItem(row, 4, item(QString::number(c.capaciteMax),     Qt::AlignCenter));
        m_table->setItem(row, 5, item(QString::number(c.nbInscrits),      Qt::AlignCenter));

        // État
        QString etat;
        QColor  couleur;
        if (c.placesRestantes <= 0) {
            etat = "COMPLET"; couleur = QColor("#f38ba8"); complet++;
            // Colorer toute la ligne
            for (int col = 0; col < 7; ++col)
                if (m_table->item(row, col))
                    m_table->item(row, col)->setBackground(QColor("#2d1e2a"));
        } else if (c.capaciteMax > 0 && c.placesRestantes <= c.capaciteMax * 0.2) {
            etat = QString("ALERTE (%1 place(s))").arg(c.placesRestantes);
            couleur = QColor("#fab387"); alerte++;
            for (int col = 0; col < 7; ++col)
                if (m_table->item(row, col))
                    m_table->item(row, col)->setBackground(QColor("#2a2218"));
        } else {
            etat = QString("%1 place(s)").arg(c.placesRestantes);
            couleur = QColor("#a6e3a1"); ok++;
        }

        QTableWidgetItem *etatItem = new QTableWidgetItem(etat);
        etatItem->setTextAlignment(Qt::AlignCenter);
        etatItem->setFlags(etatItem->flags() & ~Qt::ItemIsEditable);
        etatItem->setForeground(couleur);
        etatItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        m_table->setItem(row, 6, etatItem);
    }

    m_valComplet->setText(QString::number(complet));
    m_valAlerte->setText(QString::number(alerte));
    m_valOk->setText(QString::number(ok));
}
