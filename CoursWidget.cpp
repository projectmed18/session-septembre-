#include "CoursWidget.h"
#include "databasemanager.h"
#include "alertescoursdialog.h"
#include "classementcoursdialog.h"
#include "statistiquescoursdialog.h"
#include "pdfgeneratorcours.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QScrollArea>

// ─── CSS ─────────────────────────────────────────────────────────────────────
static const QString CSS_INPUT_C = R"(
    QLineEdit, QDateEdit, QComboBox, QSpinBox, QPlainTextEdit {
        background-color: #313244;
        color: #cdd6f4;
        border: 1px solid #45475a;
        border-radius: 5px;
        padding: 4px 8px;
        font: 9pt "Segoe UI";
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus,
    QSpinBox:focus, QPlainTextEdit:focus {
        border: 1px solid #cba6f7;
    }
    QComboBox::drop-down { border: none; }
    QDateEdit::drop-down  { border: none; }
    QSpinBox::up-button, QSpinBox::down-button { border: none; width: 16px; }
    QComboBox QAbstractItemView {
        background-color: #313244; color: #cdd6f4;
        selection-background-color: #cba6f7; selection-color: #1e1e2e;
    }
)";

static const QString CSS_TABLE_C = R"(
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
    QTableWidget::item:selected { background-color: #313244; color: #cba6f7; }
    QTableWidget::item:hover    { background-color: #252535; }
    QHeaderView::section {
        background-color: #181825; color: #cba6f7;
        font: bold 9pt "Segoe UI"; padding: 8px 10px;
        border: none; border-bottom: 2px solid #cba6f7;
    }
    QScrollBar:vertical   { background: #181825; width: 8px; border-radius: 4px; }
    QScrollBar::handle:vertical { background: #45475a; border-radius: 4px; min-height: 20px; }
    QScrollBar::handle:vertical:hover { background: #cba6f7; }
    QScrollBar:horizontal { background: #181825; height: 8px; border-radius: 4px; }
    QScrollBar::handle:horizontal { background: #45475a; border-radius: 4px; min-width: 20px; }
)";

// ─── Constructeur ─────────────────────────────────────────────────────────────
CoursWidget::CoursWidget(QWidget *parent)
    : QWidget(parent), m_idSelectionne(-1)
{
    setStyleSheet("background-color: #1e1e2e;");

    // ══════════════════════════════════════════
    //  PANNEAU GAUCHE
    // ══════════════════════════════════════════
    QWidget *panneauGauche = new QWidget(this);
    panneauGauche->setFixedWidth(220);
    panneauGauche->setStyleSheet(
        "background-color: #181825; border-right: 2px solid #313244;");

    // Titre
    QLabel *labelTitre = new QLabel("  Détails du Cours", panneauGauche);
    labelTitre->setFixedHeight(56);
    labelTitre->setStyleSheet(
        "background-color: #cba6f7; color: #1e1e2e;"
        "font: bold 12pt 'Segoe UI'; padding-left: 16px;");

    // Bouton Ajouter
    btnAjouter = new QPushButton("+ Ajouter", panneauGauche);
    btnAjouter->setFixedHeight(36);
    btnAjouter->setStyleSheet(
        "QPushButton { background-color: #a6e3a1; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #94d690; }"
        "QPushButton:pressed { background-color: #7ec87a; }");

    QFrame *sep1 = new QFrame(panneauGauche);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFixedHeight(2);
    sep1->setStyleSheet("background-color: #313244;");

    // Helper label
    auto makeLabel = [&](const QString &txt, bool muted = false) -> QLabel* {
        QLabel *l = new QLabel(txt, panneauGauche);
        l->setFixedHeight(18);
        l->setStyleSheet(muted
            ? "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;"
            : "color: #cdd6f4; font: 8pt 'Segoe UI'; background: transparent;");
        return l;
    };

    // ID (lecture seule)
    lineEditId = new QLineEdit(panneauGauche);
    lineEditId->setReadOnly(true);
    lineEditId->setPlaceholderText("Généré automatiquement");
    lineEditId->setFixedHeight(28);
    lineEditId->setStyleSheet(
        "QLineEdit { background-color: #252535; color: #6c7086;"
        "border: 1px solid #313244; border-radius: 5px;"
        "padding: 4px 8px; font: 9pt 'Segoe UI'; }");

    lineEditIntitule = new QLineEdit(panneauGauche);
    lineEditIntitule->setPlaceholderText("ex: Maroquinerie Avancée");
    lineEditIntitule->setFixedHeight(28);
    lineEditIntitule->setStyleSheet(CSS_INPUT_C);

    plainEditDescription = new QPlainTextEdit(panneauGauche);
    plainEditDescription->setPlaceholderText("Description détaillée du cours...");
    plainEditDescription->setFixedHeight(56);
    plainEditDescription->setStyleSheet(CSS_INPUT_C);

    spinDuree = new QSpinBox(panneauGauche);
    spinDuree->setRange(1, 9999);
    spinDuree->setValue(20);
    spinDuree->setFixedHeight(28);
    spinDuree->setStyleSheet(CSS_INPUT_C);

    comboNiveau = new QComboBox(panneauGauche);
    comboNiveau->addItems({"Debutant", "Intermediaire", "Avance"});
    comboNiveau->setFixedHeight(28);
    comboNiveau->setStyleSheet(CSS_INPUT_C);

    dateEditDebut = new QDateEdit(QDate::currentDate(), panneauGauche);
    dateEditDebut->setCalendarPopup(true);
    dateEditDebut->setDisplayFormat("dd/MM/yyyy");
    dateEditDebut->setFixedHeight(28);
    dateEditDebut->setStyleSheet(CSS_INPUT_C);

    dateEditFin = new QDateEdit(QDate::currentDate().addMonths(1), panneauGauche);
    dateEditFin->setCalendarPopup(true);
    dateEditFin->setDisplayFormat("dd/MM/yyyy");
    dateEditFin->setFixedHeight(28);
    dateEditFin->setStyleSheet(CSS_INPUT_C);

    spinCapacite = new QSpinBox(panneauGauche);
    spinCapacite->setRange(1, 9999);
    spinCapacite->setValue(20);
    spinCapacite->setFixedHeight(28);
    spinCapacite->setStyleSheet(CSS_INPUT_C);

    QFrame *sep2 = new QFrame(panneauGauche);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFixedHeight(2);
    sep2->setStyleSheet("background-color: #313244;");

    btnModifier = new QPushButton("Modifier", panneauGauche);
    btnModifier->setFixedHeight(36);
    btnModifier->setStyleSheet(
        "QPushButton { background-color: #f9e2af; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #f0d49a; }"
        "QPushButton:pressed { background-color: #e0c485; }");

    QLabel *labelObl = makeLabel("* Champs obligatoires", true);

    btnSupprimer = new QPushButton("Supprimer", panneauGauche);
    btnSupprimer->setFixedHeight(36);
    btnSupprimer->setStyleSheet(
        "QPushButton { background-color: #f38ba8; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #e07a97; }"
        "QPushButton:pressed { background-color: #cc6a84; }");

    // Layout panneau gauche
    QVBoxLayout *leftLayout = new QVBoxLayout(panneauGauche);
    leftLayout->setContentsMargins(0, 0, 0, 8);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(labelTitre);
    leftLayout->addSpacing(6);

    auto addField = [&](QLabel *lbl, QWidget *field) {
        leftLayout->addWidget(lbl);
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(12, 0, 12, 0);
        row->addWidget(field);
        leftLayout->addLayout(row);
        leftLayout->addSpacing(4);
    };

    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(12, 0, 12, 0);
        row->addWidget(btnAjouter);
        leftLayout->addLayout(row);
    }
    leftLayout->addWidget(sep1);
    leftLayout->addSpacing(4);

    addField(makeLabel("ID (automatique)", true), lineEditId);
    addField(makeLabel("INTITULÉ DU COURS *"),     lineEditIntitule);
    addField(makeLabel("DESCRIPTION"),             plainEditDescription);
    addField(makeLabel("DURÉE (HEURES) *"),        spinDuree);
    addField(makeLabel("NIVEAU *"),                comboNiveau);
    addField(makeLabel("DATE DE DÉBUT *"),         dateEditDebut);
    addField(makeLabel("DATE DE FIN *"),           dateEditFin);
    addField(makeLabel("CAPACITÉ MAXIMALE *"),     spinCapacite);

    leftLayout->addSpacing(4);
    leftLayout->addWidget(sep2);
    leftLayout->addSpacing(4);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(12, 0, 12, 0);
        row->addWidget(btnModifier);
        leftLayout->addLayout(row);
    }
    leftLayout->addWidget(labelObl);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(12, 0, 12, 4);
        row->addWidget(btnSupprimer);
        leftLayout->addLayout(row);
    }
    leftLayout->addStretch();

    // ══════════════════════════════════════════
    //  PANNEAU DROIT
    // ══════════════════════════════════════════
    QWidget *panneauDroit = new QWidget(this);
    panneauDroit->setStyleSheet("background-color: #1e1e2e;");

    QLabel *labelTitreApp = new QLabel("Liste des Cours", panneauDroit);
    labelTitreApp->setStyleSheet(
        "color: #cdd6f4; font: bold 14pt 'Segoe UI'; background: transparent;");

    // Barre de recherche
    lineEditRecherche = new QLineEdit(panneauDroit);
    lineEditRecherche->setPlaceholderText("Rechercher par intitulé...");
    lineEditRecherche->setFixedHeight(34);
    lineEditRecherche->setStyleSheet(
        "QLineEdit { background-color: #313244; color: #cdd6f4;"
        "border: 1px solid #45475a; border-radius: 6px;"
        "padding: 4px 12px; font: 9pt 'Segoe UI'; }"
        "QLineEdit:focus { border: 1px solid #cba6f7; }");

    comboFiltreNiveau = new QComboBox(panneauDroit);
    comboFiltreNiveau->addItems({"Tous niveaux", "Debutant", "Intermediaire", "Avance"});
    comboFiltreNiveau->setFixedHeight(34);
    comboFiltreNiveau->setStyleSheet(CSS_INPUT_C);

    comboTri = new QComboBox(panneauDroit);
    comboTri->addItems({"Trier : Intitulé (A → Z)", "Trier : Intitulé (Z → A)",
                        "Trier : Date début (récent)", "Trier : Date début (ancien)",
                        "Trier : Durée (croissant)"});
    comboTri->setFixedHeight(34);
    comboTri->setStyleSheet(CSS_INPUT_C);

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
    searchRow->addWidget(comboFiltreNiveau, 2);
    searchRow->addWidget(comboTri, 2);
    searchRow->addWidget(btnReinit, 1);

    labelResultats = new QLabel(panneauDroit);
    labelResultats->setStyleSheet(
        "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;");

    // Tableau — 9 colonnes (ID caché + 8 visibles)
    tableWidget = new QTableWidget(panneauDroit);
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels(
        {"ID", "Intitulé", "Description", "Durée (h)",
         "Niveau", "Date début", "Date fin",
         "Capacité", "Places restantes"});
    tableWidget->setColumnHidden(0, true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);
    tableWidget->setStyleSheet(CSS_TABLE_C);

    QPalette p = tableWidget->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base,          QColor("#181825"));
    tableWidget->setPalette(p);

    // Boutons métier du bas
    btnAlertes      = new QPushButton("Alertes",      panneauDroit);
    btnClassement   = new QPushButton("Classement",   panneauDroit);
    btnStatistiques = new QPushButton("Statistiques", panneauDroit);
    btnExportPdf    = new QPushButton("Export PDF",   panneauDroit);

    for (auto *b : {btnAlertes, btnClassement, btnStatistiques, btnExportPdf})
        b->setFixedHeight(40);

    btnAlertes->setStyleSheet(
        "QPushButton { background-color: #f38ba8; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #e07a97; }");
    btnClassement->setStyleSheet(
        "QPushButton { background-color: #89dceb; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #74c9d8; }");
    btnStatistiques->setStyleSheet(
        "QPushButton { background-color: #cba6f7; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #b893e0; }");
    btnExportPdf->setStyleSheet(
        "QPushButton { background-color: #fab387; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #e89e72; }");

    QHBoxLayout *bottomRow = new QHBoxLayout;
    bottomRow->setSpacing(8);
    bottomRow->addWidget(btnAlertes);
    bottomRow->addWidget(btnClassement);
    bottomRow->addWidget(btnStatistiques);
    bottomRow->addWidget(btnExportPdf);

    // Layout panneau droit
    QVBoxLayout *rightLayout = new QVBoxLayout(panneauDroit);
    rightLayout->setContentsMargins(20, 12, 20, 8);
    rightLayout->setSpacing(6);
    rightLayout->addWidget(labelTitreApp);
    rightLayout->addLayout(searchRow);
    rightLayout->addWidget(labelResultats);
    rightLayout->addWidget(tableWidget, 1);
    rightLayout->addLayout(bottomRow);

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
    connect(btnAjouter,   &QPushButton::clicked, this, &CoursWidget::onAjouterClicked);
    connect(btnModifier,  &QPushButton::clicked, this, &CoursWidget::onModifierClicked);
    connect(btnSupprimer, &QPushButton::clicked, this, &CoursWidget::onSupprimerClicked);

    connect(tableWidget, &QTableWidget::cellClicked,
            this, &CoursWidget::onLigneSelectionnee);

    connect(lineEditRecherche, &QLineEdit::textChanged,
            this, &CoursWidget::onRechercheChanged);
    connect(comboFiltreNiveau, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CoursWidget::onRechercheChanged);
    connect(comboTri, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CoursWidget::onRechercheChanged);
    connect(btnReinit,       &QPushButton::clicked, this, &CoursWidget::onReinitClicked);
    connect(btnAlertes,      &QPushButton::clicked, this, &CoursWidget::onAlertesClicked);
    connect(btnClassement,   &QPushButton::clicked, this, &CoursWidget::onClassementClicked);
    connect(btnStatistiques, &QPushButton::clicked, this, &CoursWidget::onStatistiquesClicked);
    connect(btnExportPdf,    &QPushButton::clicked, this, &CoursWidget::onExportPdfClicked);

    rafraichirTableau();
}

// ─── Formulaire ──────────────────────────────────────────────────────────────

Cours CoursWidget::construireCoursDepuisFormulaire() const
{
    return Cours(
        m_idSelectionne,
        lineEditIntitule->text().trimmed(),
        plainEditDescription->toPlainText().trimmed(),
        comboNiveau->currentText(),
        dateEditDebut->date(),
        dateEditFin->date(),
        spinDuree->value(),
        spinCapacite->value());
}

void CoursWidget::remplirFormulaire(const Cours &c)
{
    lineEditId->setText(QString::number(c.getId()));
    lineEditIntitule->setText(c.getIntitule());
    plainEditDescription->setPlainText(c.getDescription());
    spinDuree->setValue(c.getDureeHeures());
    comboNiveau->setCurrentText(c.getNiveau());
    dateEditDebut->setDate(c.getDateDebut());
    dateEditFin->setDate(c.getDateFin());
    spinCapacite->setValue(c.getCapaciteMax());
    m_idSelectionne = c.getId();
}

void CoursWidget::viderFormulaire()
{
    lineEditId->clear();
    lineEditIntitule->clear();
    plainEditDescription->clear();
    spinDuree->setValue(20);
    comboNiveau->setCurrentIndex(0);
    dateEditDebut->setDate(QDate::currentDate());
    dateEditFin->setDate(QDate::currentDate().addMonths(1));
    spinCapacite->setValue(20);
    m_idSelectionne = -1;
}

// ─── Tableau ─────────────────────────────────────────────────────────────────

void CoursWidget::afficherCours(const QList<Cours> &liste)
{
    // Places restantes pour chaque cours
    QMap<int, int> placesMap;
    const auto infos = m_dao.capaciteTousCours();
    for (const auto &info : infos)
        placesMap[info.idCours] = info.placesRestantes;

    tableWidget->setRowCount(0);

    for (const Cours &c : liste) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setRowHeight(row, 36);

        auto item = [](const QString &txt,
                       Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        tableWidget->setItem(row, 0, item(QString::number(c.getId())));
        tableWidget->setItem(row, 1, item(c.getIntitule()));

        // Description tronquée
        QString desc = c.getDescription();
        if (desc.length() > 30) desc = desc.left(28) + "…";
        tableWidget->setItem(row, 2, item(desc));

        tableWidget->setItem(row, 3, item(QString::number(c.getDureeHeures()) + " h",
                                         Qt::AlignCenter));

        // Badge couleur niveau
        QTableWidgetItem *niveauItem = new QTableWidgetItem(c.getNiveau());
        niveauItem->setTextAlignment(Qt::AlignCenter);
        niveauItem->setFlags(niveauItem->flags() & ~Qt::ItemIsEditable);
        if      (c.getNiveau() == "Debutant")      niveauItem->setForeground(QColor("#a6e3a1"));
        else if (c.getNiveau() == "Intermediaire") niveauItem->setForeground(QColor("#f9e2af"));
        else if (c.getNiveau() == "Avance")        niveauItem->setForeground(QColor("#f38ba8"));
        tableWidget->setItem(row, 4, niveauItem);

        tableWidget->setItem(row, 5, item(c.getDateDebut().toString("dd/MM/yyyy"), Qt::AlignCenter));
        tableWidget->setItem(row, 6, item(c.getDateFin().toString("dd/MM/yyyy"),   Qt::AlignCenter));
        tableWidget->setItem(row, 7, item(QString::number(c.getCapaciteMax()),      Qt::AlignCenter));

        // Places restantes colorées
        int places = placesMap.value(c.getId(), c.getCapaciteMax());
        QTableWidgetItem *placesItem = new QTableWidgetItem;
        placesItem->setTextAlignment(Qt::AlignCenter);
        placesItem->setFlags(placesItem->flags() & ~Qt::ItemIsEditable);
        if (places <= 0) {
            placesItem->setText("COMPLET");
            placesItem->setForeground(QColor("#f38ba8"));
            placesItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        } else if (c.getCapaciteMax() > 0 && places <= c.getCapaciteMax() * 0.2) {
            placesItem->setText(QString::number(places));
            placesItem->setForeground(QColor("#fab387"));
        } else {
            placesItem->setText(QString::number(places));
            placesItem->setForeground(QColor("#a6e3a1"));
        }
        tableWidget->setItem(row, 8, placesItem);
    }

    labelResultats->setText(QString("  %1 cours trouvé(s)").arg(liste.size()));
}

void CoursWidget::rafraichirTableau()
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::critical(this, "Erreur BD", "Pas de connexion à la base de données.");
        return;
    }
    afficherCours(m_dao.afficherTous());
}

// ─── Sélection ligne ─────────────────────────────────────────────────────────

void CoursWidget::onLigneSelectionnee(int row, int)
{
    int id = tableWidget->item(row, 0)->text().toInt();
    Cours c = m_dao.afficherParId(id);
    if (c.getId() != -1)
        remplirFormulaire(c);
}

// ─── Recherche / Tri ─────────────────────────────────────────────────────────

void CoursWidget::onRechercheChanged()
{
    QString intitule = lineEditRecherche->text().trimmed();
    int     idx      = comboFiltreNiveau->currentIndex();
    QString niveau   = (idx == 0) ? QString() : comboFiltreNiveau->currentText();

    QString colonne   = "intitule";
    bool    ascendant = true;
    switch (comboTri->currentIndex()) {
        case 0: colonne = "intitule";     ascendant = true;  break;
        case 1: colonne = "intitule";     ascendant = false; break;
        case 2: colonne = "date_debut";   ascendant = false; break;
        case 3: colonne = "date_debut";   ascendant = true;  break;
        case 4: colonne = "duree_heures"; ascendant = true;  break;
    }
    afficherCours(m_dao.rechercherMulticritere(intitule, niveau, colonne, ascendant));
}

void CoursWidget::onReinitClicked()
{
    lineEditRecherche->clear();
    comboFiltreNiveau->setCurrentIndex(0);
    comboTri->setCurrentIndex(0);
    rafraichirTableau();
}

// ─── CRUD ─────────────────────────────────────────────────────────────────────

void CoursWidget::onAjouterClicked()
{
    Cours c = construireCoursDepuisFormulaire();
    QString erreur;
    if (!c.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur); return;
    }
    if (m_dao.ajouter(c)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur d'ajout",
            QString("Échec de l'ajout.\n%1").arg(m_dao.dernierErreur()));
    }
}

void CoursWidget::onModifierClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Sélectionnez un cours dans le tableau."); return;
    }
    Cours c = construireCoursDepuisFormulaire();
    QString erreur;
    if (!c.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur); return;
    }
    if (m_dao.modifier(c)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la modification.\n%1").arg(m_dao.dernierErreur()));
    }
}

void CoursWidget::onSupprimerClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Sélectionnez un cours dans le tableau."); return;
    }
    if (QMessageBox::question(this, "Confirmation",
            "Supprimer ce cours ? Les inscriptions associées seront aussi supprimées.",
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    if (m_dao.supprimer(m_idSelectionne)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la suppression.\n%1").arg(m_dao.dernierErreur()));
    }
}

// ─── Dialogs métier ───────────────────────────────────────────────────────────

void CoursWidget::onAlertesClicked()
{
    AlertesCoursDialog dlg(this);
    dlg.exec();
}

void CoursWidget::onClassementClicked()
{
    ClassementCoursDialog dlg(this);
    dlg.exec();
}

void CoursWidget::onStatistiquesClicked()
{
    StatistiquesCoursDialog dlg(this);
    dlg.exec();
}

void CoursWidget::onExportPdfClicked()
{
    const auto cours = m_dao.capaciteTousCours();
    if (cours.isEmpty()) {
        QMessageBox::information(this, "Aucun cours",
            "Aucun cours à exporter."); return;
    }

    QString chemin = QFileDialog::getSaveFileName(
        this, "Enregistrer le rapport PDF",
        QDir::homePath() + "/Rapport_Cours.pdf",
        "Fichiers PDF (*.pdf)");
    if (chemin.isEmpty()) return;

    if (PdfGeneratorCours::genererRapport(cours, chemin)) {
        int rep = QMessageBox::information(this, "PDF généré",
            QString("Rapport créé :\n%1\n\nVoulez-vous l'ouvrir ?").arg(chemin),
            QMessageBox::Yes | QMessageBox::No);
        if (rep == QMessageBox::Yes)
            QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
    } else {
        QMessageBox::critical(this, "Erreur PDF",
            "La génération du PDF a échoué.");
    }
}
