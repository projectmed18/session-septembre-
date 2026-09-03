#include "capacitedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

CapaciteDialog::CapaciteDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Vérification des capacités — Cours");
    setMinimumSize(900, 600);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 16);
    mainLayout->setSpacing(14);

    // ── En-tête ───────────────────────────────────────────────────
    QFrame *header = new QFrame(this);
    header->setStyleSheet("background-color: #cba6f7; border-radius: 8px;");
    header->setFixedHeight(64);
    QHBoxLayout *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(20, 0, 20, 0);

    QLabel *titre = new QLabel("Capacité des Cours — Places Disponibles");
    titre->setStyleSheet("color: #1e1e2e; font: bold 14pt 'Segoe UI';");
    hLay->addWidget(titre);
    hLay->addStretch();

    m_labelLegendeHeader = new QLabel(this);
    m_labelLegendeHeader->setStyleSheet("color: #1e1e2e; font: 9pt 'Segoe UI';");
    hLay->addWidget(m_labelLegendeHeader);
    mainLayout->addWidget(header);

    // ── Cartes résumé ─────────────────────────────────────────────
    QHBoxLayout *cardsLay = new QHBoxLayout();
    cardsLay->setSpacing(12);

    auto creerCarte = [&](QLabel *&valLabel, const QString &lbl,
                          const QString &couleur) -> QFrame* {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString(
            "background-color: #181825; border-radius: 8px;"
            "border-left: 4px solid %1;").arg(couleur));
        card->setFixedHeight(68);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 6, 16, 6);
        valLabel = new QLabel("0");
        valLabel->setStyleSheet(QString("color:%1; font:bold 18pt 'Segoe UI';"
                                        "background:transparent;").arg(couleur));
        QLabel *l = new QLabel(lbl);
        l->setStyleSheet("color:#6c7086; font:8pt 'Segoe UI';"
                         "background:transparent;");
        cl->addWidget(valLabel); cl->addWidget(l);
        return card;
    };

    cardsLay->addWidget(creerCarte(m_valTotal, "Total cours",       "#89b4fa"));
    cardsLay->addWidget(creerCarte(m_valDispo, "Cours disponibles", "#a6e3a1"));
    cardsLay->addWidget(creerCarte(m_valPlein, "Cours complets",    "#f38ba8"));
    mainLayout->addLayout(cardsLay);

    // ── Tableau ───────────────────────────────────────────────────
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(
        {"Intitulé", "Niveau", "Date début", "Date fin",
         "Capacité max", "Inscrits", "Places restantes"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->setAlternatingRowColors(true);
    m_table->setStyleSheet(
        "QTableWidget {"
        "  background-color:#181825; color:#cdd6f4;"
        "  gridline-color:#313244; border:1px solid #313244;"
        "  border-radius:6px; font:9pt 'Segoe UI'; outline:none;"
        "}"
        "QTableWidget::item { padding:6px 10px; }"
        "QTableWidget::item:selected { background-color:#313244; color:#89b4fa; }"
        "QTableWidget::item:alternate { background-color:#252535; }"
        "QHeaderView::section {"
        "  background-color:#181825; color:#cba6f7;"
        "  font:bold 9pt 'Segoe UI'; padding:8px;"
        "  border:none; border-bottom:2px solid #cba6f7;"
        "}");

    mainLayout->addWidget(m_table);

    // Double clic pour tenter une inscription
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int){
        onInscrireClicked();
    });

    // ── Légende & Boutons ──────────────────────────────────────────
    QHBoxLayout *bottomLay = new QHBoxLayout();

    auto dot = [&](const QString &couleur, const QString &texte) {
        QLabel *l = new QLabel(QString("● %1").arg(texte));
        l->setStyleSheet(QString("color:%1; font:8pt 'Segoe UI';").arg(couleur));
        bottomLay->addWidget(l);
    };
    dot("#a6e3a1", "Disponible");
    dot("#fab387", "Presque complet (< 20% places)");
    dot("#f38ba8", "Complet (Inscription bloquée)");
    bottomLay->addStretch();

    QPushButton *btnInscrire = new QPushButton("Inscrire un Stagiaire", this);
    btnInscrire->setFixedHeight(36);
    btnInscrire->setStyleSheet(
        "QPushButton { background-color:#89b4fa; color:#1e1e2e;"
        "  font:bold 9pt 'Segoe UI'; border-radius:6px; border:none; padding:0 14px; }"
        "QPushButton:hover { background-color:#74c7ec; }");
    connect(btnInscrire, &QPushButton::clicked, this, &CapaciteDialog::onInscrireClicked);
    bottomLay->addWidget(btnInscrire);

    QPushButton *btnFermer = new QPushButton("Fermer", this);
    btnFermer->setFixedHeight(36);
    btnFermer->setStyleSheet(
        "QPushButton { background-color:#45475a; color:#cdd6f4;"
        "  font:9pt 'Segoe UI'; border-radius:6px; border:none; padding:0 14px; }"
        "QPushButton:hover { background-color:#585b70; }");
    connect(btnFermer, &QPushButton::clicked, this, &QDialog::accept);
    bottomLay->addWidget(btnFermer);

    mainLayout->addLayout(bottomLay);
    setLayout(mainLayout);

    chargerDonnees();
}

void CapaciteDialog::chargerDonnees()
{
    m_cours = m_dao.capaciteTousCours();

    int totalPlein = 0, totalDisponible = 0;
    for (const auto &c : m_cours) {
        if (c.placesRestantes <= 0) totalPlein++;
        else totalDisponible++;
    }

    m_valTotal->setText(QString::number(m_cours.size()));
    m_valDispo->setText(QString::number(totalDisponible));
    m_valPlein->setText(QString::number(totalPlein));

    m_labelLegendeHeader->setText(
        QString("Complet : %1   |   Disponible : %2")
            .arg(totalPlein).arg(totalDisponible));

    m_table->setRowCount(m_cours.size());
    for (int i = 0; i < m_cours.size(); ++i) {
        const auto &c = m_cours[i];
        m_table->setRowHeight(i, 36);

        auto item = [](const QString &txt,
                       Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        m_table->setItem(i, 0, item(c.intitule));
        m_table->setItem(i, 1, item(c.niveau, Qt::AlignCenter));
        m_table->setItem(i, 2, item(c.dateDebut.toString("dd/MM/yyyy"), Qt::AlignCenter));
        m_table->setItem(i, 3, item(c.dateFin.toString("dd/MM/yyyy"),   Qt::AlignCenter));
        m_table->setItem(i, 4, item(QString::number(c.capaciteMax),     Qt::AlignCenter));
        m_table->setItem(i, 5, item(QString::number(c.nbInscrits),      Qt::AlignCenter));

        // Places restantes — colorées selon disponibilité
        QTableWidgetItem *placesItem = new QTableWidgetItem(
            QString::number(c.placesRestantes));
        placesItem->setTextAlignment(Qt::AlignCenter);
        placesItem->setFlags(placesItem->flags() & ~Qt::ItemIsEditable);

        if (c.placesRestantes <= 0) {
            // Complet — rouge
            placesItem->setText("COMPLET");
            placesItem->setForeground(QColor("#f38ba8"));
            placesItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        } else if (c.placesRestantes <= c.capaciteMax * 0.2) {
            // Moins de 20% de places — orange
            placesItem->setForeground(QColor("#fab387"));
        } else {
            // Disponible — vert
            placesItem->setForeground(QColor("#a6e3a1"));
        }
        m_table->setItem(i, 6, placesItem);

        // Colorer toute la ligne si complet
        if (c.placesRestantes <= 0) {
            for (int col = 0; col < 7; ++col) {
                if (m_table->item(i, col))
                    m_table->item(i, col)->setBackground(QColor("#2d1e2a"));
            }
        }
    }
}

void CapaciteDialog::onInscrireClicked()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_cours.size()) {
        QMessageBox::information(this, "Sélection requise",
                                 "Veuillez sélectionner un cours dans la liste avant de tenter une inscription.");
        return;
    }

    const auto &c = m_cours[row];

    // --- CONTRÔLE ET BLOCAGE MÉTIER : vérification si le cours est complet ---
    if (!m_dao.placesDisponibles(c.idCours) || c.placesRestantes <= 0) {
        QMessageBox::critical(
            this,
            "INSCRIPTION BLOQUÉE — Cours Complet",
            QString("⛔ INSCRIPTION IMPOSSIBLE !\n\n"
                    "Le cours « %1 » a atteint sa capacité maximale (%2 inscrits sur %3 places).\n\n"
                    "L'inscription à ce cours est actuellement bloquée.")
                .arg(c.intitule)
                .arg(c.nbInscrits)
                .arg(c.capaciteMax));
        return;
    }

    // Inscription autorisée
    bool ok = false;
    int idStagiaire = QInputDialog::getInt(
        this,
        "Inscrire un stagiaire",
        QString("Cours sélectionné : « %1 » (%2 places restantes)\n\n"
                "Saisissez l'ID du stagiaire à inscrire :")
            .arg(c.intitule)
            .arg(c.placesRestantes),
        1, 1, 999999, 1, &ok);

    if (!ok) return;

    if (m_dao.inscrireStagiaire(idStagiaire, c.idCours)) {
        QMessageBox::information(
            this,
            "Inscription réussie",
            QString("Le stagiaire ID %1 a été inscrit avec succès au cours « %2 ».")
                .arg(idStagiaire)
                .arg(c.intitule));
        chargerDonnees();
    } else {
        QMessageBox::critical(
            this,
            "Erreur d'inscription",
            QString("Échec de l'inscription du stagiaire ID %1.\n\nDétail : %2")
                .arg(idStagiaire)
                .arg(m_dao.dernierErreur()));
    }
}
