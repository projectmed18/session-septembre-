#include "InscriptionWidget.h"
#include "databasemanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QShowEvent>

// ─── CSS ─────────────────────────────────────────────────────────────────────
static const QString CSS_INPUT_I = R"(
    QLineEdit, QDateEdit, QComboBox {
        background-color: #313244;
        color: #cdd6f4;
        border: 1px solid #45475a;
        border-radius: 5px;
        padding: 4px 8px;
        font: 9pt "Segoe UI";
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus {
        border: 1px solid #fab387;
    }
    QComboBox::drop-down { border: none; }
    QDateEdit::drop-down  { border: none; }
    QComboBox QAbstractItemView {
        background-color: #313244; color: #cdd6f4;
        selection-background-color: #fab387; selection-color: #1e1e2e;
    }
)";

static const QString CSS_TABLE_I = R"(
    QTableWidget {
        background-color: #181825;
        color: #cdd6f4;
        gridline-color: #313244;
        border: 1px solid #313244;
        border-radius: 8px;
        font: 9pt "Segoe UI";
        outline: none;
    }
    QTableWidget::item { padding: 6px 10px; border-bottom: 1px solid #313244; }
    QTableWidget::item:selected { background-color: #313244; color: #fab387; }
    QTableWidget::item:hover    { background-color: #252535; }
    QHeaderView::section {
        background-color: #181825; color: #fab387;
        font: bold 9pt "Segoe UI"; padding: 8px 10px;
        border: none; border-bottom: 2px solid #fab387;
    }
    QScrollBar:vertical   { background: #181825; width: 8px; border-radius: 4px; }
    QScrollBar::handle:vertical { background: #45475a; border-radius: 4px; min-height: 20px; }
    QScrollBar::handle:vertical:hover { background: #fab387; }
    QScrollBar:horizontal { background: #181825; height: 8px; border-radius: 4px; }
    QScrollBar::handle:horizontal { background: #45475a; border-radius: 4px; min-width: 20px; }
)";

static const QString CSS_STAT_CARD = R"(
    QLabel {
        background-color: #181825;
        border: 1px solid #313244;
        border-radius: 8px;
        padding: 8px 14px;
        font: 9pt "Segoe UI";
    }
)";

// ─── showEvent — rechargement automatique à chaque affichage ─────────────────
void InscriptionWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // Recharger stagiaires et cours à chaque fois que le widget devient visible
    // pour refléter les ajouts faits dans les autres modules
    chargerComboStagiaires();
    chargerCombosCours();
    rafraichirTableau(false);
}

// ─── Constructeur ─────────────────────────────────────────────────────────────
InscriptionWidget::InscriptionWidget(QWidget *parent)
    : QWidget(parent), m_idSelectionne(-1)
{
    setStyleSheet("background-color: #1e1e2e;");

    // ══════════════════════════════════════════
    //  PANNEAU GAUCHE
    // ══════════════════════════════════════════
    QWidget *panneauGauche = new QWidget(this);
    panneauGauche->setFixedWidth(270);
    panneauGauche->setStyleSheet(
        "background-color: #181825; border-right: 2px solid #313244;");

    // Titre
    QLabel *labelTitre = new QLabel("  Nouvelle Inscription", panneauGauche);
    labelTitre->setFixedHeight(56);
    labelTitre->setStyleSheet(
        "background-color: #fab387; color: #1e1e2e;"
        "font: bold 12pt 'Segoe UI'; padding-left: 16px;");

    // Helper label
    auto makeLabel = [&](const QString &txt, bool muted = false) -> QLabel* {
        QLabel *l = new QLabel(txt, panneauGauche);
        l->setFixedHeight(20);
        l->setStyleSheet(muted
            ? "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;"
            : "color: #cdd6f4; font: 8pt 'Segoe UI'; background: transparent;");
        return l;
    };

    // Combo Stagiaire
    comboStagiaire = new QComboBox(panneauGauche);
    comboStagiaire->setFixedHeight(34);
    comboStagiaire->setStyleSheet(CSS_INPUT_I);

    // Combo Cours
    comboCours = new QComboBox(panneauGauche);
    comboCours->setFixedHeight(34);
    comboCours->setStyleSheet(CSS_INPUT_I);

    // Label places restantes
    labelPlaces = new QLabel("", panneauGauche);
    labelPlaces->setFixedHeight(24);
    labelPlaces->setAlignment(Qt::AlignCenter);
    labelPlaces->setStyleSheet(
        "color: #a6e3a1; font: bold 8pt 'Segoe UI';"
        "background: #252535; border-radius: 4px; border: none;");

    // Date inscription
    dateEditInscription = new QDateEdit(QDate::currentDate(), panneauGauche);
    dateEditInscription->setCalendarPopup(true);
    dateEditInscription->setDisplayFormat("dd/MM/yyyy");
    dateEditInscription->setFixedHeight(34);
    dateEditInscription->setStyleSheet(CSS_INPUT_I);

    // Statut
    comboStatut = new QComboBox(panneauGauche);
    comboStatut->addItems(Inscription::statutsDisponibles());
    comboStatut->setFixedHeight(34);
    comboStatut->setStyleSheet(CSS_INPUT_I);

    // Séparateur
    QFrame *sep1 = new QFrame(panneauGauche);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFixedHeight(2);
    sep1->setStyleSheet("background-color: #313244;");

    // Bouton Inscrire
    btnInscrire = new QPushButton("✔  Inscrire", panneauGauche);
    btnInscrire->setFixedHeight(40);
    btnInscrire->setStyleSheet(
        "QPushButton { background-color: #fab387; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #e89e72; }"
        "QPushButton:pressed { background-color: #d08855; }");

    // Séparateur 2
    QFrame *sep2 = new QFrame(panneauGauche);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFixedHeight(2);
    sep2->setStyleSheet("background-color: #313244;");

    // Bouton Modifier statut
    btnModifierStatut = new QPushButton("Modifier le statut", panneauGauche);
    btnModifierStatut->setFixedHeight(38);
    btnModifierStatut->setStyleSheet(
        "QPushButton { background-color: #f9e2af; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #f0d49a; }"
        "QPushButton:pressed { background-color: #e0c485; }");

    QLabel *labelObl = makeLabel("* Sélectionnez une ligne du tableau", true);
    QLabel *labelInfo = makeLabel("  Termine = automatique (date fin dépassée)", true);

    // Bouton Désinscrire
    btnDesinscrire = new QPushButton("Désinscrire", panneauGauche);
    btnDesinscrire->setFixedHeight(38);
    btnDesinscrire->setStyleSheet(
        "QPushButton { background-color: #f38ba8; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #e07a97; }"
        "QPushButton:pressed { background-color: #cc6a84; }");

    // Layout panneau gauche
    QVBoxLayout *leftLayout = new QVBoxLayout(panneauGauche);
    leftLayout->setContentsMargins(0, 0, 0, 8);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(labelTitre);
    leftLayout->addSpacing(10);

    auto addField = [&](QLabel *lbl, QWidget *field) {
        leftLayout->addWidget(lbl);
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(field);
        leftLayout->addLayout(row);
        leftLayout->addSpacing(6);
    };

    addField(makeLabel("STAGIAIRE *"),          comboStagiaire);
    addField(makeLabel("COURS *"),              comboCours);

    // Places restantes
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(labelPlaces);
        leftLayout->addLayout(row);
        leftLayout->addSpacing(4);
    }

    addField(makeLabel("DATE D'INSCRIPTION *"), dateEditInscription);
    addField(makeLabel("STATUT *"),             comboStatut);

    leftLayout->addWidget(sep1);
    leftLayout->addSpacing(6);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(btnInscrire);
        leftLayout->addLayout(row);
    }
    leftLayout->addSpacing(10);
    leftLayout->addWidget(sep2);
    leftLayout->addSpacing(6);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(btnModifierStatut);
        leftLayout->addLayout(row);
    }
    leftLayout->addWidget(labelObl);
    leftLayout->addWidget(labelInfo);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 4);
        row->addWidget(btnDesinscrire);
        leftLayout->addLayout(row);
    }
    leftLayout->addStretch();

    // ══════════════════════════════════════════
    //  PANNEAU DROIT
    // ══════════════════════════════════════════
    QWidget *panneauDroit = new QWidget(this);
    panneauDroit->setStyleSheet("background-color: #1e1e2e;");

    QLabel *labelTitreApp = new QLabel("Liste des Inscriptions", panneauDroit);
    labelTitreApp->setStyleSheet(
        "color: #cdd6f4; font: bold 14pt 'Segoe UI'; background: transparent;");

    // ── Cartes statistiques ──────────────────────────────────────
    labelStatTotal   = new QLabel("TOTAL\n—",    panneauDroit);
    labelStatEnCours = new QLabel("EN COURS\n—", panneauDroit);
    labelStatTermine = new QLabel("TERMINE\n—",  panneauDroit);
    labelStatAnnule  = new QLabel("ANNULE\n—",   panneauDroit);

    for (QLabel *lbl : {labelStatTotal, labelStatEnCours, labelStatTermine, labelStatAnnule}) {
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setFixedHeight(54);
        lbl->setStyleSheet(CSS_STAT_CARD);
    }
    labelStatTotal->setStyleSheet(CSS_STAT_CARD +
        "QLabel { color: #cdd6f4; border-color: #585b70; }");
    labelStatEnCours->setStyleSheet(CSS_STAT_CARD +
        "QLabel { color: #89b4fa; border-color: #89b4fa; }");
    labelStatTermine->setStyleSheet(CSS_STAT_CARD +
        "QLabel { color: #a6e3a1; border-color: #a6e3a1; }");
    labelStatAnnule->setStyleSheet(CSS_STAT_CARD +
        "QLabel { color: #f38ba8; border-color: #f38ba8; }");

    QHBoxLayout *statsRow = new QHBoxLayout;
    statsRow->setSpacing(10);
    statsRow->addWidget(labelStatTotal);
    statsRow->addWidget(labelStatEnCours);
    statsRow->addWidget(labelStatTermine);
    statsRow->addWidget(labelStatAnnule);

    // ── Barre de recherche ───────────────────────────────────────
    lineEditRecherche = new QLineEdit(panneauDroit);
    lineEditRecherche->setPlaceholderText("Rechercher par stagiaire ou cours...");
    lineEditRecherche->setFixedHeight(34);
    lineEditRecherche->setStyleSheet(
        "QLineEdit { background-color: #313244; color: #cdd6f4;"
        "border: 1px solid #45475a; border-radius: 6px;"
        "padding: 4px 12px; font: 9pt 'Segoe UI'; }"
        "QLineEdit:focus { border: 1px solid #fab387; }");

    comboFiltreStatut = new QComboBox(panneauDroit);
    comboFiltreStatut->addItems({"Tous statuts", "En cours", "Termine", "Annule"});
    comboFiltreStatut->setFixedHeight(34);
    comboFiltreStatut->setStyleSheet(CSS_INPUT_I);

    comboTri = new QComboBox(panneauDroit);
    comboTri->addItems({"Trier : Date (récent)", "Trier : Date (ancien)",
                        "Trier : Stagiaire (A→Z)", "Trier : Cours (A→Z)"});
    comboTri->setFixedHeight(34);
    comboTri->setStyleSheet(CSS_INPUT_I);

    btnReinit = new QPushButton("Réinitialiser", panneauDroit);
    btnReinit->setFixedHeight(34);
    btnReinit->setStyleSheet(
        "QPushButton { background-color: #45475a; color: #cdd6f4;"
        "font: 9pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #585b70; }"
        "QPushButton:pressed { background-color: #313244; }");

    QHBoxLayout *searchRow = new QHBoxLayout;
    searchRow->setSpacing(8);
    searchRow->addWidget(lineEditRecherche, 3);
    searchRow->addWidget(comboFiltreStatut, 2);
    searchRow->addWidget(comboTri, 2);
    searchRow->addWidget(btnReinit, 1);

    labelResultats = new QLabel(panneauDroit);
    labelResultats->setStyleSheet(
        "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;");

    // ── Tableau ──────────────────────────────────────────────────
    tableWidget = new QTableWidget(panneauDroit);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels(
        {"ID", "Stagiaire", "Prénom", "Cours", "Niveau", "Date inscr.", "Statut"});
    tableWidget->setColumnHidden(0, true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);
    tableWidget->setStyleSheet(CSS_TABLE_I);

    QPalette p = tableWidget->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base,          QColor("#181825"));
    tableWidget->setPalette(p);

    // Layout panneau droit
    QVBoxLayout *rightLayout = new QVBoxLayout(panneauDroit);
    rightLayout->setContentsMargins(20, 12, 20, 8);
    rightLayout->setSpacing(8);
    rightLayout->addWidget(labelTitreApp);
    rightLayout->addLayout(statsRow);
    rightLayout->addLayout(searchRow);
    rightLayout->addWidget(labelResultats);
    rightLayout->addWidget(tableWidget, 1);

    // ══════════════════════════════════════════
    //  LAYOUT PRINCIPAL
    // ══════════════════════════════════════════
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(panneauGauche);
    mainLayout->addWidget(panneauDroit, 1);
    setLayout(mainLayout);

    // ══════════════════════════════════════════
    //  CONNEXIONS
    // ══════════════════════════════════════════
    connect(btnInscrire,       &QPushButton::clicked, this, &InscriptionWidget::onInscrireClicked);
    connect(btnModifierStatut, &QPushButton::clicked, this, &InscriptionWidget::onModifierStatutClicked);
    connect(btnDesinscrire,    &QPushButton::clicked, this, &InscriptionWidget::onDesinscrireClicked);

    connect(tableWidget, &QTableWidget::cellClicked,
            this, &InscriptionWidget::onLigneSelectionnee);

    connect(lineEditRecherche, &QLineEdit::textChanged,
            this, &InscriptionWidget::onRechercheChanged);
    connect(comboFiltreStatut, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InscriptionWidget::onRechercheChanged);
    connect(comboTri, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InscriptionWidget::onRechercheChanged);
    connect(btnReinit, &QPushButton::clicked,
            this, &InscriptionWidget::onReinitClicked);
    connect(comboCours, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &InscriptionWidget::onCoursSelectionChanged);

    // Chargement initial
    chargerComboStagiaires();
    chargerCombosCours();
    rafraichirTableau();
}

// ─── Chargement des combos ────────────────────────────────────────────────────

void InscriptionWidget::chargerComboStagiaires()
{
    comboStagiaire->clear();
    comboStagiaire->addItem("-- Sélectionner un stagiaire --", -1);
    const auto liste = m_dayStagiaire.afficherTous();
    for (const Stagiaire &s : liste) {
        comboStagiaire->addItem(
            QString("%1 %2").arg(s.getNom()).arg(s.getPrenom()),
            s.getId());
    }
}

void InscriptionWidget::chargerCombosCours()
{
    comboCours->clear();
    comboCours->addItem("-- Sélectionner un cours --", -1);
    const auto liste = m_daoCours.afficherTous();
    for (const Cours &c : liste) {
        int places = m_dao.placesRestantes(c.getId());
        QString label = QString("%1 (%2) — %3 place(s)")
                        .arg(c.getIntitule())
                        .arg(c.getNiveau())
                        .arg(places);
        comboCours->addItem(label, c.getId());
    }
    onCoursSelectionChanged(comboCours->currentIndex());
}

void InscriptionWidget::onCoursSelectionChanged(int index)
{
    int idCours = comboCours->itemData(index).toInt();
    if (idCours <= 0) {
        labelPlaces->setText("");
        return;
    }
    int places = m_dao.placesRestantes(idCours);
    if (places <= 0) {
        labelPlaces->setText("⚠  Cours complet — aucune place disponible");
        labelPlaces->setStyleSheet(
            "color: #f38ba8; font: bold 8pt 'Segoe UI';"
            "background: #2a1e22; border-radius: 4px; border: none;");
    } else if (places <= 3) {
        labelPlaces->setText(QString("⚠  Plus que %1 place(s) disponible(s)").arg(places));
        labelPlaces->setStyleSheet(
            "color: #fab387; font: bold 8pt 'Segoe UI';"
            "background: #2a2218; border-radius: 4px; border: none;");
    } else {
        labelPlaces->setText(QString("✔  %1 places disponibles").arg(places));
        labelPlaces->setStyleSheet(
            "color: #a6e3a1; font: bold 8pt 'Segoe UI';"
            "background: #1e2a1e; border-radius: 4px; border: none;");
    }
}

// ─── Tableau ─────────────────────────────────────────────────────────────────

void InscriptionWidget::afficherInscriptions(const QList<Inscription> &liste)
{
    tableWidget->setRowCount(0);

    int total = liste.size();
    int enCours = 0, termine = 0, annule = 0;

    for (const Inscription &insc : liste) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setRowHeight(row, 36);

        auto item = [](const QString &txt,
                       Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft)
        {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        tableWidget->setItem(row, 0, item(QString::number(insc.getId())));
        tableWidget->setItem(row, 1, item(insc.getNomStagiaire()));
        tableWidget->setItem(row, 2, item(insc.getPrenomStagiaire()));
        tableWidget->setItem(row, 3, item(insc.getIntituleCours()));

        // Badge couleur niveau
        QTableWidgetItem *niveauItem = new QTableWidgetItem(insc.getNiveauCours());
        niveauItem->setTextAlignment(Qt::AlignCenter);
        niveauItem->setFlags(niveauItem->flags() & ~Qt::ItemIsEditable);
        if      (insc.getNiveauCours() == "Debutant")      niveauItem->setForeground(QColor("#a6e3a1"));
        else if (insc.getNiveauCours() == "Intermediaire") niveauItem->setForeground(QColor("#f9e2af"));
        else if (insc.getNiveauCours() == "Avance")        niveauItem->setForeground(QColor("#f38ba8"));
        tableWidget->setItem(row, 4, niveauItem);

        tableWidget->setItem(row, 5,
            item(insc.getDateInscription().toString("dd/MM/yyyy"), Qt::AlignCenter));

        // Badge couleur statut
        QTableWidgetItem *statutItem = new QTableWidgetItem(insc.getStatut());
        statutItem->setTextAlignment(Qt::AlignCenter);
        statutItem->setFlags(statutItem->flags() & ~Qt::ItemIsEditable);
        statutItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        if (insc.getStatut() == "En cours") {
            statutItem->setForeground(QColor("#89b4fa"));
            enCours++;
        } else if (insc.getStatut() == "Termine") {
            statutItem->setForeground(QColor("#a6e3a1"));
            termine++;
        } else if (insc.getStatut() == "Annule") {
            statutItem->setForeground(QColor("#f38ba8"));
            annule++;
        }
        tableWidget->setItem(row, 6, statutItem);
    }

    labelResultats->setText(QString("  %1 inscription(s) trouvée(s)").arg(liste.size()));

    // Mise à jour cartes stats
    labelStatTotal->setText(QString("TOTAL\n%1").arg(total));
    labelStatEnCours->setText(QString("EN COURS\n%1").arg(enCours));
    labelStatTermine->setText(QString("TERMINE\n%1").arg(termine));
    labelStatAnnule->setText(QString("ANNULE\n%1").arg(annule));
}

void InscriptionWidget::rafraichirTableau(bool rechargerCombos)
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::critical(this, "Erreur BD", "Pas de connexion à la base de données.");
        return;
    }
    afficherInscriptions(m_dao.afficherTous());
    // Recharger les combos uniquement quand le nombre d'inscrits change
    // (ajout ou suppression), pas pour une simple modification de statut
    if (rechargerCombos)
        chargerCombosCours();
}

// ─── Sélection ligne ─────────────────────────────────────────────────────────

void InscriptionWidget::onLigneSelectionnee(int row, int)
{
    m_idSelectionne = tableWidget->item(row, 0)->text().toInt();

    // Pré-remplir le statut dans le combo pour modification rapide
    QString statut = tableWidget->item(row, 6)->text();
    comboStatut->setCurrentText(statut);
}

// ─── Recherche / Tri ─────────────────────────────────────────────────────────

void InscriptionWidget::onRechercheChanged()
{
    QString nom    = lineEditRecherche->text().trimmed();
    int     idx    = comboFiltreStatut->currentIndex();
    QString statut = (idx == 0) ? QString() : comboFiltreStatut->currentText();

    QString colonne   = "date_inscription";
    bool    ascendant = false;
    switch (comboTri->currentIndex()) {
        case 0: colonne = "date_inscription"; ascendant = false; break;
        case 1: colonne = "date_inscription"; ascendant = true;  break;
        case 2: colonne = "nom_stagiaire";    ascendant = true;  break;
        case 3: colonne = "intitule_cours";   ascendant = true;  break;
    }
    afficherInscriptions(
        m_dao.rechercherMulticritere(nom, QString(), statut, colonne, ascendant));
}

void InscriptionWidget::onReinitClicked()
{
    lineEditRecherche->clear();
    comboFiltreStatut->setCurrentIndex(0);
    comboTri->setCurrentIndex(0);
    rafraichirTableau(true);
}

void InscriptionWidget::viderFormulaire()
{
    comboStagiaire->setCurrentIndex(0);
    comboCours->setCurrentIndex(0);
    dateEditInscription->setDate(QDate::currentDate());
    comboStatut->setCurrentIndex(0);
    labelPlaces->setText("");
    m_idSelectionne = -1;
}

// ─── CRUD ─────────────────────────────────────────────────────────────────────

void InscriptionWidget::onInscrireClicked()
{
    int idStagiaire = comboStagiaire->currentData().toInt();
    int idCours     = comboCours->currentData().toInt();

    if (idStagiaire <= 0 || idCours <= 0) {
        QMessageBox::warning(this, "Saisie incomplète",
            "Veuillez sélectionner un stagiaire et un cours."); return;
    }

    // Doublon
    if (m_dao.existeDeja(idStagiaire, idCours)) {
        QMessageBox::warning(this, "Déjà inscrit",
            "Ce stagiaire est déjà inscrit à ce cours."); return;
    }

    Inscription insc;
    insc.setIdStagiaire(idStagiaire);
    insc.setIdCours(idCours);
    insc.setDateInscription(dateEditInscription->date());
    insc.setStatut(comboStatut->currentText());

    QString erreur;
    if (!insc.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur); return;
    }

    if (m_dao.ajouter(insc)) {
        QMessageBox::information(this, "Inscription réussie",
            "Le stagiaire a bien été inscrit au cours.");
        rafraichirTableau(true);   // true = recharger les places dans comboCours
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de l'inscription.\n%1").arg(m_dao.dernierErreur()));
    }
}

void InscriptionWidget::onModifierStatutClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Sélectionnez une inscription dans le tableau."); return;
    }
    QString nouveauStatut = comboStatut->currentText();

    // Sauvegarder l'id sélectionné avant le refresh
    int idSauvegarde = m_idSelectionne;

    if (m_dao.modifierStatut(idSauvegarde, nouveauStatut)) {
        // false = ne pas recharger les combos (aucun changement de capacité)
        rafraichirTableau(false);
        // Restaurer la sélection pour que l'utilisateur puisse enchaîner
        m_idSelectionne = idSauvegarde;
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la modification.\n%1").arg(m_dao.dernierErreur()));
    }
}

void InscriptionWidget::onDesinscrireClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Sélectionnez une inscription dans le tableau."); return;
    }
    if (QMessageBox::question(this, "Confirmation",
            "Supprimer cette inscription définitivement ?",
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    if (m_dao.supprimer(m_idSelectionne)) {
        rafraichirTableau(true);   // true = recharger les places (libérées)
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la suppression.\n%1").arg(m_dao.dernierErreur()));
    }
}
