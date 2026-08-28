#include "fichesuividialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFrame>
#include <QPushButton>
#include <QProgressBar>

FicheSuiviDialog::FicheSuiviDialog(const Stagiaire &stagiaire, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString("Fiche de suivi — %1 %2")
                       .arg(stagiaire.getNom(), stagiaire.getPrenom()));
    setMinimumSize(820, 580);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    StagiaireDAO dao;
    QList<StagiaireDAO::LigneSuivi> cours = dao.suiviStagiaire(stagiaire.getId());

    // Calcul des stats
    int total    = cours.size();
    int termine  = 0, enCours = 0, annule = 0, totalHeures = 0;
    for (const auto &l : cours) {
        if      (l.statut == "Termine")  termine++;
        else if (l.statut == "En cours") enCours++;
        else if (l.statut == "Annule")   annule++;
        totalHeures += l.dureeHeures;
    }

    // ── Layout principal ──────────────────────────────────────────
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    // ── En-tête ───────────────────────────────────────────────────
    QFrame *header = new QFrame(this);
    header->setStyleSheet("background-color: #89b4fa; border-radius: 8px;");
    header->setFixedHeight(70);
    QHBoxLayout *hLay = new QHBoxLayout(header);
    hLay->setContentsMargins(20, 0, 20, 0);

    QLabel *nomLabel = new QLabel(
        QString("%1 %2").arg(stagiaire.getNom(), stagiaire.getPrenom()));
    nomLabel->setStyleSheet("color: #1e1e2e; font: bold 16pt 'Segoe UI';");

    QLabel *niveauLabel = new QLabel("Niveau : " + stagiaire.getNiveau());
    niveauLabel->setStyleSheet("color: #1e1e2e; font: 10pt 'Segoe UI';");

    QLabel *dateLabel = new QLabel(
        "Inscrit le : " + stagiaire.getDateInscription().toString("dd/MM/yyyy"));
    dateLabel->setStyleSheet("color: #1e1e2e; font: 9pt 'Segoe UI';");

    hLay->addWidget(nomLabel);
    hLay->addStretch();
    hLay->addWidget(niveauLabel);
    hLay->addSpacing(20);
    hLay->addWidget(dateLabel);

    mainLayout->addWidget(header);

    // ── Cartes de statistiques ────────────────────────────────────
    QHBoxLayout *cardsLay = new QHBoxLayout();
    cardsLay->setSpacing(12);

    auto creerCarte = [&](const QString &valeur, const QString &label,
                          const QString &couleur) -> QFrame* {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString(
            "background-color: #181825; border-radius: 8px; "
            "border-left: 4px solid %1;").arg(couleur));
        card->setFixedHeight(72);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16, 8, 16, 8);

        QLabel *val = new QLabel(valeur);
        val->setStyleSheet(QString("color: %1; font: bold 18pt 'Segoe UI'; "
                                   "background: transparent;").arg(couleur));
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("color: #6c7086; font: 8pt 'Segoe UI'; "
                           "background: transparent;");
        cl->addWidget(val);
        cl->addWidget(lbl);
        return card;
    };

    cardsLay->addWidget(creerCarte(QString::number(total),       "Total cours",       "#89b4fa"));
    cardsLay->addWidget(creerCarte(QString::number(termine),     "Terminés",          "#a6e3a1"));
    cardsLay->addWidget(creerCarte(QString::number(enCours),     "En cours",          "#f9e2af"));
    cardsLay->addWidget(creerCarte(QString::number(annule),      "Annulés",           "#f38ba8"));
    cardsLay->addWidget(creerCarte(QString::number(totalHeures)+"h", "Heures totales","#cba6f7"));
    mainLayout->addLayout(cardsLay);

    // ── Barre de progression globale ─────────────────────────────
    if (total > 0) {
        QHBoxLayout *progLay = new QHBoxLayout();
        QLabel *progLabel = new QLabel(
            QString("Progression : %1 cours terminé(s) sur %2")
                .arg(termine).arg(total));
        progLabel->setStyleSheet("color: #cdd6f4; font: 9pt 'Segoe UI';");

        QProgressBar *bar = new QProgressBar();
        bar->setMinimum(0);
        bar->setMaximum(total);
        bar->setValue(termine);
        bar->setTextVisible(false);
        bar->setFixedHeight(10);
        bar->setStyleSheet(
            "QProgressBar { background-color: #313244; border-radius: 5px; }"
            "QProgressBar::chunk { background-color: #a6e3a1; border-radius: 5px; }");

        progLay->addWidget(progLabel, 1);
        progLay->addWidget(bar, 2);
        mainLayout->addLayout(progLay);
    }

    // ── Tableau des cours ─────────────────────────────────────────
    QLabel *titreTableau = new QLabel("Détail des cours suivis");
    titreTableau->setStyleSheet(
        "color: #89b4fa; font: bold 10pt 'Segoe UI'; padding-top: 4px;");
    mainLayout->addWidget(titreTableau);

    QTableWidget *table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {"Intitulé du cours", "Niveau", "Date début", "Date fin", "Durée (h)", "Statut"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    table->setStyleSheet(
        "QTableWidget {"
        "  background-color: #181825; color: #cdd6f4;"
        "  gridline-color: #313244; border: 1px solid #313244;"
        "  border-radius: 6px; font: 9pt 'Segoe UI'; outline: none;"
        "}"
        "QTableWidget::item { padding: 6px 10px; }"
        "QTableWidget::item:selected { background-color: #313244; color: #89b4fa; }"
        "QTableWidget::item:alternate { background-color: #252535; }"
        "QHeaderView::section {"
        "  background-color: #181825; color: #89b4fa;"
        "  font: bold 9pt 'Segoe UI'; padding: 8px;"
        "  border: none; border-bottom: 2px solid #89b4fa;"
        "}");

    // Remplir le tableau
    table->setRowCount(cours.size());
    for (int i = 0; i < cours.size(); ++i) {
        const auto &l = cours[i];
        table->setRowHeight(i, 34);

        auto item = [](const QString &txt, Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        table->setItem(i, 0, item(l.intituleCours));
        table->setItem(i, 1, item(l.niveau, Qt::AlignCenter));
        table->setItem(i, 2, item(l.dateDebut.toString("dd/MM/yyyy"), Qt::AlignCenter));
        table->setItem(i, 3, item(l.dateFin.toString("dd/MM/yyyy"),   Qt::AlignCenter));
        table->setItem(i, 4, item(QString::number(l.dureeHeures) + " h", Qt::AlignCenter));

        // Badge statut coloré
        QTableWidgetItem *statutItem = new QTableWidgetItem(l.statut);
        statutItem->setTextAlignment(Qt::AlignCenter);
        statutItem->setFlags(statutItem->flags() & ~Qt::ItemIsEditable);
        if      (l.statut == "Termine")  statutItem->setForeground(QColor("#a6e3a1"));
        else if (l.statut == "En cours") statutItem->setForeground(QColor("#f9e2af"));
        else if (l.statut == "Annule")   statutItem->setForeground(QColor("#f38ba8"));
        table->setItem(i, 5, statutItem);
    }

    // Message si aucun cours
    if (cours.isEmpty()) {
        QLabel *vide = new QLabel("Aucun cours inscrit pour ce stagiaire.");
        vide->setStyleSheet("color: #6c7086; font: italic 10pt 'Segoe UI';"
                            "padding: 20px;");
        vide->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(vide);
    } else {
        mainLayout->addWidget(table);
    }

    // ── Bouton fermer ─────────────────────────────────────────────
    QPushButton *btnFermer = new QPushButton("Fermer", this);
    btnFermer->setFixedHeight(36);
    btnFermer->setStyleSheet(
        "QPushButton { background-color: #45475a; color: #cdd6f4;"
        "  font: 9pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #585b70; }");
    connect(btnFermer, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(btnFermer);

    setLayout(mainLayout);
}
