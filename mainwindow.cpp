#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include "statistiquedialog.h"
#include "pdfgenerator.h"
#include "fichesuividialog.h"
#include "capacitedialog.h"
#include "coursdao.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPalette>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_idSelectionne(-1)
{
    ui->setupUi(this);

    // --- Titre fenêtre ---
    setWindowTitle("Gestion des Stagiaires — Centre de Formation");

    // --- Configurer le tableau ---
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"ID", "Nom", "Prénom", "Email", "Téléphone", "D. Naissance", "Niveau", "D. Inscription"});
    ui->tableWidget->setColumnHidden(0, true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);

    // Couleur des lignes alternées (dark theme)
    QPalette p = ui->tableWidget->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base, QColor("#181825"));
    ui->tableWidget->setPalette(p);

    // --- Forcer les valeurs exactes du CHECK Oracle ---
    ui->comboBox->clear();
    ui->comboBox->addItems({"Debutant", "Intermediaire", "Avance"});

    // --- Connexions CRUD ---
    connect(ui->pushButton,   &QPushButton::clicked,      this, &MainWindow::onAjouterClicked);
    connect(ui->pushButton_2, &QPushButton::clicked,      this, &MainWindow::onModifierClicked);
    connect(ui->pushButton_3, &QPushButton::clicked,      this, &MainWindow::onSupprimerClicked);
    connect(ui->tableWidget,  &QTableWidget::cellClicked,       this, &MainWindow::onLigneSelectionnee);
    connect(ui->tableWidget,  &QTableWidget::cellDoubleClicked,  this, &MainWindow::onFicheSuiviDemandee);

    // --- Connexions Recherche / Tri ---
    connect(ui->lineEditRecherche,  &QLineEdit::textChanged,
            this, &MainWindow::onRechercheChanged);
    connect(ui->comboFiltreNiveau,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRechercheChanged);
    connect(ui->comboTri,           QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRechercheChanged);
    connect(ui->btnReinit,          &QPushButton::clicked,
            this, &MainWindow::onReinitClicked);
    connect(ui->btnCapacite,        &QPushButton::clicked,
            this, &MainWindow::onCapaciteClicked);
    connect(ui->btnStatistiques,    &QPushButton::clicked,
            this, &MainWindow::onStatistiquesClicked);
    connect(ui->btnFicheSuivi,      &QPushButton::clicked,
            this, &MainWindow::onFicheSuiviClicked);
    connect(ui->btnExportPdf,       &QPushButton::clicked,
            this, &MainWindow::onExportPdfClicked);

    rafraichirTableau();
}

MainWindow::~MainWindow() { delete ui; }

// ─────────────────────────────────────────────────────────────
//  RECHERCHE / TRI
// ─────────────────────────────────────────────────────────────

void MainWindow::onRechercheChanged()
{
    QString nom    = ui->lineEditRecherche->text().trimmed();
    int     idx    = ui->comboFiltreNiveau->currentIndex();
    QString niveau = (idx == 0) ? QString() : ui->comboFiltreNiveau->currentText();

    // Décoder le tri sélectionné
    QString colonne     = "nom";
    bool    ascendant   = true;
    switch (ui->comboTri->currentIndex()) {
        case 0: colonne = "nom";              ascendant = true;  break;
        case 1: colonne = "nom";              ascendant = false; break;
        case 2: colonne = "date_inscription"; ascendant = false; break;
        case 3: colonne = "date_inscription"; ascendant = true;  break;
        case 4: colonne = "niveau";           ascendant = true;  break;
    }

    QList<Stagiaire> resultats = m_dao.rechercherMulticritere(nom, niveau, -1, colonne, ascendant);
    afficherStagiaires(resultats);
}

void MainWindow::onReinitClicked()
{
    ui->lineEditRecherche->clear();
    ui->comboFiltreNiveau->setCurrentIndex(0);
    ui->comboTri->setCurrentIndex(0);
    rafraichirTableau();
}

// ─────────────────────────────────────────────────────────────
//  AFFICHAGE TABLEAU
// ─────────────────────────────────────────────────────────────

void MainWindow::afficherStagiaires(const QList<Stagiaire> &liste)
{
    ui->tableWidget->setRowCount(0);

    for (const Stagiaire &s : liste) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        // Hauteur de ligne confortable
        ui->tableWidget->setRowHeight(row, 36);

        auto item = [](const QString &txt, Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        ui->tableWidget->setItem(row, 0, item(QString::number(s.getId())));
        ui->tableWidget->setItem(row, 1, item(s.getNom()));
        ui->tableWidget->setItem(row, 2, item(s.getPrenom()));
        ui->tableWidget->setItem(row, 3, item(s.getEmail()));
        ui->tableWidget->setItem(row, 4, item(s.getTelephone(), Qt::AlignCenter));
        ui->tableWidget->setItem(row, 5, item(s.getDateNaissance().toString("dd/MM/yyyy"), Qt::AlignCenter));

        // Badge couleur pour le niveau
        QTableWidgetItem *niveauItem = new QTableWidgetItem(s.getNiveau());
        niveauItem->setTextAlignment(Qt::AlignCenter);
        niveauItem->setFlags(niveauItem->flags() & ~Qt::ItemIsEditable);
        if (s.getNiveau() == "Debutant")
            niveauItem->setForeground(QColor("#a6e3a1"));      // vert
        else if (s.getNiveau() == "Intermediaire")
            niveauItem->setForeground(QColor("#f9e2af"));      // jaune
        else if (s.getNiveau() == "Avance")
            niveauItem->setForeground(QColor("#f38ba8"));      // rouge/rose
        ui->tableWidget->setItem(row, 6, niveauItem);

        ui->tableWidget->setItem(row, 7, item(s.getDateInscription().toString("dd/MM/yyyy"), Qt::AlignCenter));
    }

    // Compteur mis à jour
    ui->labelResultats->setText(
        QString("  %1 stagiaire(s) trouvé(s)").arg(liste.size()));

    // Message barre de statut
    statusBar()->showMessage(
        QString("Base connectée  •  %1 stagiaire(s) affiché(s)").arg(liste.size()));
}

void MainWindow::rafraichirTableau()
{
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::critical(this, "Erreur BD", "Pas de connexion à la base de données.");
        return;
    }
    afficherStagiaires(m_dao.afficherTous());
}

// ─────────────────────────────────────────────────────────────
//  FORMULAIRE
// ─────────────────────────────────────────────────────────────

Stagiaire MainWindow::construireStagiaireDepuisFormulaire() const
{
    return Stagiaire(
        m_idSelectionne,
        ui->lineEdit->text().trimmed(),
        ui->lineEdit_2->text().trimmed(),
        ui->lineEdit_3->text().trimmed(),
        ui->lineEdit_4->text().trimmed(),
        ui->dateEdit->date(),
        ui->comboBox->currentText());
}

void MainWindow::remplirFormulaire(const Stagiaire &s)
{
    ui->lineEditId->setText(QString::number(s.getId()));
    ui->lineEdit->setText(s.getNom());
    ui->lineEdit_2->setText(s.getPrenom());
    ui->lineEdit_3->setText(s.getEmail());
    ui->lineEdit_4->setText(s.getTelephone());
    ui->dateEdit->setDate(s.getDateNaissance());
    ui->comboBox->setCurrentText(s.getNiveau());
    m_idSelectionne = s.getId();
}

void MainWindow::viderFormulaire()
{
    ui->lineEditId->clear();
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->dateEdit->setDate(QDate(2000, 1, 1));
    ui->comboBox->setCurrentIndex(0);
    m_idSelectionne = -1;
}

void MainWindow::onLigneSelectionnee(int row, int)
{
    Stagiaire s(
        ui->tableWidget->item(row, 0)->text().toInt(),
        ui->tableWidget->item(row, 1)->text(),
        ui->tableWidget->item(row, 2)->text(),
        ui->tableWidget->item(row, 3)->text(),
        ui->tableWidget->item(row, 4)->text(),
        QDate::fromString(ui->tableWidget->item(row, 5)->text(), "dd/MM/yyyy"),
        ui->tableWidget->item(row, 6)->text(),
        QDate::fromString(ui->tableWidget->item(row, 7)->text(), "dd/MM/yyyy"));
    remplirFormulaire(s);
}

// ─────────────────────────────────────────────────────────────
//  CRUD
// ─────────────────────────────────────────────────────────────

void MainWindow::onAjouterClicked()
{
    Stagiaire s = construireStagiaireDepuisFormulaire();
    QString erreur;
    if (!s.estValide(erreur)) { QMessageBox::warning(this, "Saisie invalide", erreur); return; }
    if (m_dao.emailExiste(s.getEmail())) {
        QMessageBox::warning(this, "Email déjà utilisé", "Un stagiaire avec cet email existe déjà.");
        return;
    }
    if (m_dao.ajouter(s)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur d'ajout",
            QString("Échec de l'ajout en base.\n\nDétail : %1").arg(m_dao.dernierErreur()));
    }
}

void MainWindow::onModifierClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection", "Sélectionnez un stagiaire dans le tableau.");
        return;
    }
    Stagiaire s = construireStagiaireDepuisFormulaire();
    QString erreur;
    if (!s.estValide(erreur)) { QMessageBox::warning(this, "Saisie invalide", erreur); return; }
    if (m_dao.emailExiste(s.getEmail(), m_idSelectionne)) {
        QMessageBox::warning(this, "Email déjà utilisé", "Un autre stagiaire utilise déjà cet email.");
        return;
    }
    if (m_dao.modifier(s)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification.");
    }
}

void MainWindow::onSupprimerClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection", "Sélectionnez un stagiaire dans le tableau.");
        return;
    }
    auto rep = QMessageBox::question(this, "Confirmation", "Supprimer ce stagiaire ?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (rep != QMessageBox::Yes) return;
    if (m_dao.supprimer(m_idSelectionne)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression.");
    }
}

// ─────────────────────────────────────────────────────────────
//  CAPACITES DES COURS
// ─────────────────────────────────────────────────────────────
void MainWindow::onCapaciteClicked()
{
    CapaciteDialog dlg(this);
    dlg.exec();
}

// ─────────────────────────────────────────────────────────────
//  STATISTIQUES
// ─────────────────────────────────────────────────────────────
void MainWindow::onStatistiquesClicked()
{
    StatistiqueDialog dlg(this);
    dlg.exec();
}

// ─────────────────────────────────────────────────────────────
//  EXPORT PDF
// ─────────────────────────────────────────────────────────────
void MainWindow::onExportPdfClicked()
{
    // Vérifier qu'un stagiaire est sélectionné
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Veuillez sélectionner un stagiaire dans le tableau\n"
            "avant de générer son attestation PDF.");
        return;
    }

    // Récupérer le stagiaire complet depuis la base
    Stagiaire s = m_dao.afficherParId(m_idSelectionne);
    if (s.getId() == -1) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger les données du stagiaire.");
        return;
    }

    // Demander où sauvegarder
    QString nomFichier = QString("Attestation_%1_%2.pdf")
                             .arg(s.getNom()).arg(s.getPrenom());
    QString chemin = QFileDialog::getSaveFileName(
        this,
        "Enregistrer l'attestation PDF",
        QDir::homePath() + "/" + nomFichier,
        "Fichiers PDF (*.pdf)");

    if (chemin.isEmpty()) return;  // annulé par l'utilisateur

    if (PdfGenerator::genererAttestation(s, chemin)) {
        int rep = QMessageBox::information(
            this, "PDF généré",
            QString("Attestation créée avec succès :\n%1\n\nVoulez-vous l'ouvrir ?")
                .arg(chemin),
            QMessageBox::Yes | QMessageBox::No);

        if (rep == QMessageBox::Yes)
            QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
    } else {
        QMessageBox::critical(this, "Erreur PDF",
            "La génération du PDF a échoué.\n"
            "Vérifiez que le dossier de destination est accessible.");
    }
}

// ─────────────────────────────────────────────────────────────
//  FICHE DE SUIVI — double-clic sur une ligne
// ─────────────────────────────────────────────────────────────
void MainWindow::onFicheSuiviDemandee(int row, int /*column*/)
{
    // Récupérer le stagiaire depuis la ligne du tableau
    Stagiaire s(
        ui->tableWidget->item(row, 0)->text().toInt(),
        ui->tableWidget->item(row, 1)->text(),
        ui->tableWidget->item(row, 2)->text(),
        ui->tableWidget->item(row, 3)->text(),
        ui->tableWidget->item(row, 4)->text(),
        QDate::fromString(ui->tableWidget->item(row, 5)->text(), "dd/MM/yyyy"),
        ui->tableWidget->item(row, 6)->text(),
        QDate::fromString(ui->tableWidget->item(row, 7)->text(), "dd/MM/yyyy"));

    FicheSuiviDialog dlg(s, this);
    dlg.exec();
}

// ─────────────────────────────────────────────────────────────
//  FICHE DE SUIVI — bouton
// ─────────────────────────────────────────────────────────────
void MainWindow::onFicheSuiviClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
            "Veuillez sélectionner un stagiaire dans le tableau\n"
            "avant d'ouvrir sa fiche de suivi.");
        return;
    }

    Stagiaire s = m_dao.afficherParId(m_idSelectionne);
    if (s.getId() == -1) {
        QMessageBox::warning(this, "Erreur",
            "Impossible de charger les données du stagiaire.");
        return;
    }

    FicheSuiviDialog dlg(s, this);
    dlg.exec();
}
