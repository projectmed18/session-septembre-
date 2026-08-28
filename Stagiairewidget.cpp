#include "Stagiairewidget.h"
#include "databasemanager.h"
#include "statistiquedialog.h"
#include "fichesuividialog.h"
#include "capacitedialog.h"
#include "pdfgenerator.h"

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

// ─── Helpers CSS ────────────────────────────────────────────────────
static const QString CSS_INPUT = R"(
    QLineEdit, QDateEdit, QComboBox {
        background-color: #313244;
        color: #cdd6f4;
        border: 1px solid #45475a;
        border-radius: 5px;
        padding: 4px 8px;
        font: 9pt "Segoe UI";
    }
    QLineEdit:focus, QDateEdit:focus, QComboBox:focus {
        border: 1px solid #89b4fa;
    }
    QComboBox::drop-down { border: none; }
    QDateEdit::drop-down  { border: none; }
    QComboBox QAbstractItemView {
        background-color: #313244; color: #cdd6f4;
        selection-background-color: #89b4fa; selection-color: #1e1e2e;
    }
)";

static const QString CSS_TABLE = R"(
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
    QTableWidget::item:selected { background-color: #313244; color: #89b4fa; }
    QTableWidget::item:hover    { background-color: #252535; }
    QHeaderView::section {
        background-color: #181825; color: #89b4fa;
        font: bold 9pt "Segoe UI"; padding: 8px 10px;
        border: none; border-bottom: 2px solid #89b4fa;
    }
    QScrollBar:vertical   { background: #181825; width: 8px; border-radius: 4px; }
    QScrollBar::handle:vertical { background: #45475a; border-radius: 4px; min-height: 20px; }
    QScrollBar::handle:vertical:hover { background: #89b4fa; }
    QScrollBar:horizontal { background: #181825; height: 8px; border-radius: 4px; }
    QScrollBar::handle:horizontal { background: #45475a; border-radius: 4px; min-width: 20px; }
)";

// ─── Constructeur ────────────────────────────────────────────────────
StagiaireWidget::StagiaireWidget(QWidget *parent)
    : QWidget(parent), m_idSelectionne(-1)
{
    setStyleSheet("background-color: #1e1e2e;");

    // ══════════════════════════════════════════
    //  PANNEAU GAUCHE
    // ══════════════════════════════════════════
    QWidget *panneauGauche = new QWidget(this);
    panneauGauche->setFixedWidth(240);
    panneauGauche->setStyleSheet(
        "background-color: #181825; border-right: 2px solid #313244;");

    // Titre
    QLabel *labelTitre = new QLabel("  Détails Stagiaire", panneauGauche);
    labelTitre->setFixedHeight(56);
    labelTitre->setStyleSheet(
        "background-color: #89b4fa; color: #1e1e2e;"
        "font: bold 13pt 'Segoe UI'; padding-left: 16px;");

    // Bouton Ajouter
    pushButton = new QPushButton("+ Ajouter", panneauGauche);
    pushButton->setFixedHeight(38);
    pushButton->setStyleSheet(
        "QPushButton { background-color: #a6e3a1; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #94d690; }"
        "QPushButton:pressed { background-color: #7ec87a; }");

    // Séparateur
    QFrame *sep1 = new QFrame(panneauGauche);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFixedHeight(2);
    sep1->setStyleSheet("background-color: #313244;");

    // Champs formulaire
    auto makeLabel = [&](const QString &txt, bool muted = false) -> QLabel * {
        QLabel *l = new QLabel(txt, panneauGauche);
        l->setFixedHeight(20);
        l->setStyleSheet(muted
            ? "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;"
            : "color: #cdd6f4; font: 8pt 'Segoe UI'; background: transparent;");
        return l;
    };

    lineEditId = new QLineEdit(panneauGauche);
    lineEditId->setReadOnly(true);
    lineEditId->setPlaceholderText("Généré automatiquement");
    lineEditId->setFixedHeight(32);
    lineEditId->setStyleSheet(
        "QLineEdit { background-color: #252535; color: #6c7086;"
        "border: 1px solid #313244; border-radius: 5px; padding: 4px 8px;"
        "font: 9pt 'Segoe UI'; }");

    lineEdit   = new QLineEdit(panneauGauche); lineEdit->setFixedHeight(32);
    lineEdit_2 = new QLineEdit(panneauGauche); lineEdit_2->setFixedHeight(32);
    lineEdit_3 = new QLineEdit(panneauGauche); lineEdit_3->setFixedHeight(32);
    lineEdit_4 = new QLineEdit(panneauGauche); lineEdit_4->setFixedHeight(32);

    dateEdit = new QDateEdit(QDate(2000, 1, 1), panneauGauche);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setFixedHeight(32);

    comboBox = new QComboBox(panneauGauche);
    comboBox->addItems({"Debutant", "Intermediaire", "Avance"});
    comboBox->setFixedHeight(32);

    for (auto *w : {lineEdit, lineEdit_2, lineEdit_3, lineEdit_4})
        w->setStyleSheet(CSS_INPUT);
    dateEdit->setStyleSheet(CSS_INPUT);
    comboBox->setStyleSheet(CSS_INPUT);

    // Séparateur 2
    QFrame *sep2 = new QFrame(panneauGauche);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFixedHeight(2);
    sep2->setStyleSheet("background-color: #313244;");

    // Bouton Modifier
    pushButton_2 = new QPushButton("Modifier", panneauGauche);
    pushButton_2->setFixedHeight(38);
    pushButton_2->setStyleSheet(
        "QPushButton { background-color: #f9e2af; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #f0d49a; }"
        "QPushButton:pressed { background-color: #e0c485; }");

    // Note obligatoire
    QLabel *labelObl = makeLabel("* Champs obligatoires", true);

    // Bouton Supprimer
    pushButton_3 = new QPushButton("Supprimer", panneauGauche);
    pushButton_3->setFixedHeight(38);
    pushButton_3->setStyleSheet(
        "QPushButton { background-color: #f38ba8; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #e07a97; }"
        "QPushButton:pressed { background-color: #cc6a84; }");

    // Layout panneau gauche
    QVBoxLayout *leftLayout = new QVBoxLayout(panneauGauche);
    leftLayout->setContentsMargins(0, 0, 0, 8);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(labelTitre);
    leftLayout->addSpacing(8);

    auto addField = [&](QLabel *lbl, QWidget *field) {
        leftLayout->addSpacing(4);
        leftLayout->addWidget(lbl);
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(field);
        leftLayout->addLayout(row);
    };

    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 0);
        row->addWidget(pushButton);
        leftLayout->addLayout(row);
    }
    leftLayout->addWidget(sep1);
    addField(makeLabel("ID (automatique)", true), lineEditId);
    addField(makeLabel("NOM *"),         lineEdit);
    addField(makeLabel("PRÉNOM *"),      lineEdit_2);
    addField(makeLabel("EMAIL *"),       lineEdit_3);
    addField(makeLabel("TÉLÉPHONE"),     lineEdit_4);
    addField(makeLabel("DATE DE NAISSANCE"), dateEdit);
    addField(makeLabel("NIVEAU *"),      comboBox);
    leftLayout->addSpacing(8);
    leftLayout->addWidget(sep2);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 4, 16, 0);
        row->addWidget(pushButton_2);
        leftLayout->addLayout(row);
    }
    leftLayout->addWidget(labelObl);
    {
        QHBoxLayout *row = new QHBoxLayout;
        row->setContentsMargins(16, 0, 16, 4);
        row->addWidget(pushButton_3);
        leftLayout->addLayout(row);
    }
    leftLayout->addStretch();

    // ══════════════════════════════════════════
    //  PANNEAU DROIT
    // ══════════════════════════════════════════
    QWidget *panneauDroit = new QWidget(this);
    panneauDroit->setStyleSheet("background-color: #1e1e2e;");

    QLabel *labelTitreApp = new QLabel("Liste des Stagiaires", panneauDroit);
    labelTitreApp->setStyleSheet(
        "color: #cdd6f4; font: bold 14pt 'Segoe UI'; background: transparent;");

    // Barre de recherche
    lineEditRecherche = new QLineEdit(panneauDroit);
    lineEditRecherche->setPlaceholderText("Rechercher par nom / prenom...");
    lineEditRecherche->setFixedHeight(34);
    lineEditRecherche->setStyleSheet(
        "QLineEdit { background-color: #313244; color: #cdd6f4;"
        "border: 1px solid #45475a; border-radius: 6px;"
        "padding: 4px 12px; font: 9pt 'Segoe UI'; }"
        "QLineEdit:focus { border: 1px solid #89b4fa; }");

    comboFiltreNiveau = new QComboBox(panneauDroit);
    comboFiltreNiveau->addItems({"Tous niveaux", "Debutant", "Intermediaire", "Avance"});
    comboFiltreNiveau->setFixedHeight(34);
    comboFiltreNiveau->setStyleSheet(CSS_INPUT);

    comboTri = new QComboBox(panneauDroit);
    comboTri->addItems({"Trier : Nom (A → Z)", "Trier : Nom (Z → A)",
                        "Trier : Date insc. (récent)", "Trier : Date insc. (ancien)",
                        "Trier : Niveau (A → Z)"});
    comboTri->setFixedHeight(34);
    comboTri->setStyleSheet(CSS_INPUT);

    btnReinit = new QPushButton("Reinitialiser", panneauDroit);
    btnReinit->setFixedHeight(34);
    btnReinit->setStyleSheet(
        "QPushButton { background-color: #45475a; color: #cdd6f4;"
        "font: 9pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover   { background-color: #585b70; }"
        "QPushButton:pressed { background-color: #313244; }");

    // Ligne recherche
    QHBoxLayout *searchRow = new QHBoxLayout;
    searchRow->setSpacing(8);
    searchRow->addWidget(lineEditRecherche, 3);
    searchRow->addWidget(comboFiltreNiveau, 2);
    searchRow->addWidget(comboTri, 2);
    searchRow->addWidget(btnReinit, 1);

    labelResultats = new QLabel(panneauDroit);
    labelResultats->setStyleSheet(
        "color: #6c7086; font: 8pt 'Segoe UI'; background: transparent;");

    // Tableau
    tableWidget = new QTableWidget(panneauDroit);
    tableWidget->setColumnCount(8);
    tableWidget->setHorizontalHeaderLabels(
        {"ID", "Nom", "Prénom", "Email", "Téléphone",
         "D. Naissance", "Niveau", "D. Inscription"});
    tableWidget->setColumnHidden(0, true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);
    tableWidget->setStyleSheet(CSS_TABLE);

    QPalette p = tableWidget->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base,          QColor("#181825"));
    tableWidget->setPalette(p);

    // Boutons du bas
    btnCapacite = new QPushButton("Capacités des Cours", panneauDroit);
    btnStatistiques = new QPushButton("Statistiques",       panneauDroit);
    btnFicheSuivi   = new QPushButton("Fiche de Suivi",     panneauDroit);
    btnExportPdf    = new QPushButton("Export PDF",          panneauDroit);

    for (auto *b : {btnCapacite, btnStatistiques, btnFicheSuivi, btnExportPdf})
        b->setFixedHeight(40);

    btnCapacite->setStyleSheet(
        "QPushButton { background-color: #a6e3a1; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #94d690; }");
    btnStatistiques->setStyleSheet(
        "QPushButton { background-color: #cba6f7; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #b893e0; }");
    btnFicheSuivi->setStyleSheet(
        "QPushButton { background-color: #89dceb; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #74c9d8; }");
    btnExportPdf->setStyleSheet(
        "QPushButton { background-color: #fab387; color: #1e1e2e;"
        "font: bold 10pt 'Segoe UI'; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #e89e72; }");

    QHBoxLayout *bottomRow = new QHBoxLayout;
    bottomRow->setSpacing(8);
    bottomRow->addWidget(btnCapacite);
    bottomRow->addWidget(btnStatistiques);
    bottomRow->addWidget(btnFicheSuivi);
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
    connect(pushButton,   &QPushButton::clicked, this, &StagiaireWidget::onAjouterClicked);
    connect(pushButton_2, &QPushButton::clicked, this, &StagiaireWidget::onModifierClicked);
    connect(pushButton_3, &QPushButton::clicked, this, &StagiaireWidget::onSupprimerClicked);

    connect(tableWidget, &QTableWidget::cellClicked,
            this, &StagiaireWidget::onLigneSelectionnee);
    connect(tableWidget, &QTableWidget::cellDoubleClicked,
            this, &StagiaireWidget::onFicheSuiviDemandee);

    connect(lineEditRecherche, &QLineEdit::textChanged,
            this, &StagiaireWidget::onRechercheChanged);
    connect(comboFiltreNiveau, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StagiaireWidget::onRechercheChanged);
    connect(comboTri, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StagiaireWidget::onRechercheChanged);
    connect(btnReinit,      &QPushButton::clicked, this, &StagiaireWidget::onReinitClicked);
    connect(btnCapacite,    &QPushButton::clicked, this, &StagiaireWidget::onCapaciteClicked);
    connect(btnStatistiques,&QPushButton::clicked, this, &StagiaireWidget::onStatistiquesClicked);
    connect(btnFicheSuivi,  &QPushButton::clicked, this, &StagiaireWidget::onFicheSuiviClicked);
    connect(btnExportPdf,   &QPushButton::clicked, this, &StagiaireWidget::onExportPdfClicked);

    rafraichirTableau();
}

// ─── Formulaire ──────────────────────────────────────────────────────

Stagiaire StagiaireWidget::construireStagiaireDepuisFormulaire() const
{
    return Stagiaire(
        m_idSelectionne,
        lineEdit->text().trimmed(),
        lineEdit_2->text().trimmed(),
        lineEdit_3->text().trimmed(),
        lineEdit_4->text().trimmed(),
        dateEdit->date(),
        comboBox->currentText());
}

void StagiaireWidget::remplirFormulaire(const Stagiaire &s)
{
    lineEditId->setText(QString::number(s.getId()));
    lineEdit->setText(s.getNom());
    lineEdit_2->setText(s.getPrenom());
    lineEdit_3->setText(s.getEmail());
    lineEdit_4->setText(s.getTelephone());
    dateEdit->setDate(s.getDateNaissance());
    comboBox->setCurrentText(s.getNiveau());
    m_idSelectionne = s.getId();
}

void StagiaireWidget::viderFormulaire()
{
    lineEditId->clear();
    lineEdit->clear();
    lineEdit_2->clear();
    lineEdit_3->clear();
    lineEdit_4->clear();
    dateEdit->setDate(QDate(2000, 1, 1));
    comboBox->setCurrentIndex(0);
    m_idSelectionne = -1;
}

// ─── Tableau ─────────────────────────────────────────────────────────

void StagiaireWidget::afficherStagiaires(const QList<Stagiaire> &liste)
{
    tableWidget->setRowCount(0);

    for (const Stagiaire &s : liste) {
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

        tableWidget->setItem(row, 0, item(QString::number(s.getId())));
        tableWidget->setItem(row, 1, item(s.getNom()));
        tableWidget->setItem(row, 2, item(s.getPrenom()));
        tableWidget->setItem(row, 3, item(s.getEmail()));
        tableWidget->setItem(row, 4, item(s.getTelephone(),   Qt::AlignCenter));
        tableWidget->setItem(row, 5, item(s.getDateNaissance().toString("dd/MM/yyyy"), Qt::AlignCenter));

        // Badge couleur niveau
        QTableWidgetItem *niveauItem = new QTableWidgetItem(s.getNiveau());
        niveauItem->setTextAlignment(Qt::AlignCenter);
        niveauItem->setFlags(niveauItem->flags() & ~Qt::ItemIsEditable);
        if      (s.getNiveau() == "Debutant")      niveauItem->setForeground(QColor("#a6e3a1"));
        else if (s.getNiveau() == "Intermediaire") niveauItem->setForeground(QColor("#f9e2af"));
        else if (s.getNiveau() == "Avance")        niveauItem->setForeground(QColor("#f38ba8"));
        tableWidget->setItem(row, 6, niveauItem);

        tableWidget->setItem(row, 7, item(s.getDateInscription().toString("dd/MM/yyyy"), Qt::AlignCenter));
    }

    labelResultats->setText(QString("  %1 stagiaire(s) trouvé(s)").arg(liste.size()));
}

void StagiaireWidget::rafraichirTableau()
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::critical(this, "Erreur BD", "Pas de connexion à la base de données.");
        return;
    }
    afficherStagiaires(m_dao.afficherTous());
}

// ─── Slots tableau ───────────────────────────────────────────────────

void StagiaireWidget::onLigneSelectionnee(int row, int)
{
    Stagiaire s(
        tableWidget->item(row, 0)->text().toInt(),
        tableWidget->item(row, 1)->text(),
        tableWidget->item(row, 2)->text(),
        tableWidget->item(row, 3)->text(),
        tableWidget->item(row, 4)->text(),
        QDate::fromString(tableWidget->item(row, 5)->text(), "dd/MM/yyyy"),
        tableWidget->item(row, 6)->text(),
        QDate::fromString(tableWidget->item(row, 7)->text(), "dd/MM/yyyy"));
    remplirFormulaire(s);
}

void StagiaireWidget::onFicheSuiviDemandee(int row, int)
{
    Stagiaire s(
        tableWidget->item(row, 0)->text().toInt(),
        tableWidget->item(row, 1)->text(),
        tableWidget->item(row, 2)->text(),
        tableWidget->item(row, 3)->text(),
        tableWidget->item(row, 4)->text(),
        QDate::fromString(tableWidget->item(row, 5)->text(), "dd/MM/yyyy"),
        tableWidget->item(row, 6)->text(),
        QDate::fromString(tableWidget->item(row, 7)->text(), "dd/MM/yyyy"));
    FicheSuiviDialog dlg(s, this);
    dlg.exec();
}

// ─── Recherche / Tri ─────────────────────────────────────────────────

void StagiaireWidget::onRechercheChanged()
{
    QString nom    = lineEditRecherche->text().trimmed();
    int     idx    = comboFiltreNiveau->currentIndex();
    QString niveau = (idx == 0) ? QString() : comboFiltreNiveau->currentText();

    QString colonne   = "nom";
    bool    ascendant = true;
    switch (comboTri->currentIndex()) {
        case 0: colonne = "nom";              ascendant = true;  break;
        case 1: colonne = "nom";              ascendant = false; break;
        case 2: colonne = "date_inscription"; ascendant = false; break;
        case 3: colonne = "date_inscription"; ascendant = true;  break;
        case 4: colonne = "niveau";           ascendant = true;  break;
    }
    afficherStagiaires(m_dao.rechercherMulticritere(nom, niveau, -1, colonne, ascendant));
}

void StagiaireWidget::onReinitClicked()
{
    lineEditRecherche->clear();
    comboFiltreNiveau->setCurrentIndex(0);
    comboTri->setCurrentIndex(0);
    rafraichirTableau();
}

// ─── CRUD ────────────────────────────────────────────────────────────

void StagiaireWidget::onAjouterClicked()
{
    Stagiaire s = construireStagiaireDepuisFormulaire();
    QString erreur;
    if (!s.estValide(erreur)) { QMessageBox::warning(this, "Saisie invalide", erreur); return; }
    if (m_dao.emailExiste(s.getEmail())) {
        QMessageBox::warning(this, "Email déjà utilisé",
                             "Un stagiaire avec cet email existe déjà."); return;
    }
    if (m_dao.ajouter(s)) { rafraichirTableau(); viderFormulaire(); }
    else QMessageBox::critical(this, "Erreur d'ajout",
             QString("Échec de l'ajout.\n%1").arg(m_dao.dernierErreur()));
}

void StagiaireWidget::onModifierClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
                                 "Sélectionnez un stagiaire dans le tableau."); return;
    }
    Stagiaire s = construireStagiaireDepuisFormulaire();
    QString erreur;
    if (!s.estValide(erreur)) { QMessageBox::warning(this, "Saisie invalide", erreur); return; }
    if (m_dao.emailExiste(s.getEmail(), m_idSelectionne)) {
        QMessageBox::warning(this, "Email déjà utilisé",
                             "Un autre stagiaire utilise déjà cet email."); return;
    }
    if (m_dao.modifier(s)) { rafraichirTableau(); viderFormulaire(); }
    else QMessageBox::critical(this, "Erreur", "Échec de la modification.");
}

void StagiaireWidget::onSupprimerClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
                                 "Sélectionnez un stagiaire dans le tableau."); return;
    }
    if (QMessageBox::question(this, "Confirmation", "Supprimer ce stagiaire ?",
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    if (m_dao.supprimer(m_idSelectionne)) { rafraichirTableau(); viderFormulaire(); }
    else QMessageBox::critical(this, "Erreur", "Échec de la suppression.");
}

// ─── Dialogs ─────────────────────────────────────────────────────────

void StagiaireWidget::onCapaciteClicked()
{
    CapaciteDialog dlg(this);
    dlg.exec();
}

void StagiaireWidget::onStatistiquesClicked()
{
    StatistiqueDialog dlg(this);
    dlg.exec();
}

void StagiaireWidget::onFicheSuiviClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Veuillez sélectionner un stagiaire dans le tableau."); return;
    }
    Stagiaire s = m_dao.afficherParId(m_idSelectionne);
    if (s.getId() == -1) {
        QMessageBox::warning(this, "Erreur",
            "Impossible de charger les données du stagiaire."); return;
    }
    FicheSuiviDialog dlg(s, this);
    dlg.exec();
}

void StagiaireWidget::onExportPdfClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Veuillez sélectionner un stagiaire avant de générer l'attestation PDF."); return;
    }
    Stagiaire s = m_dao.afficherParId(m_idSelectionne);
    if (s.getId() == -1) {
        QMessageBox::warning(this, "Erreur",
            "Impossible de charger les données du stagiaire."); return;
    }

    QString nomFichier = QString("Attestation_%1_%2.pdf").arg(s.getNom()).arg(s.getPrenom());
    QString chemin = QFileDialog::getSaveFileName(
        this, "Enregistrer l'attestation PDF",
        QDir::homePath() + "/" + nomFichier, "Fichiers PDF (*.pdf)");
    if (chemin.isEmpty()) return;

    if (PdfGenerator::genererAttestation(s, chemin)) {
        int rep = QMessageBox::information(this, "PDF généré",
            QString("Attestation créée :\n%1\n\nVoulez-vous l'ouvrir ?").arg(chemin),
            QMessageBox::Yes | QMessageBox::No);
        if (rep == QMessageBox::Yes)
            QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
    } else {
        QMessageBox::critical(this, "Erreur PDF",
            "La génération du PDF a échoué.");
    }
}
