#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasemanager.h"
#include "statistiquedialog.h"
#include "alertescoursdialog.h"
#include "classementcoursdialog.h"
#include "pdfgenerator.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPalette>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_idSelectionne(-1)
{
    ui->setupUi(this);

    setWindowTitle("Gestion des Cours — Centre de Formation");

    // --- Configurer le tableau ---
    ui->tableWidget->setColumnCount(8);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"ID", "Intitulé", "Description", "Durée (h)", "Niveau", "Date Début", "Date Fin", "Capacité"});
    ui->tableWidget->setColumnHidden(0, true); // Masquer la colonne ID
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);

    // Couleurs du tableau (Catppuccin Mocha)
    QPalette p = ui->tableWidget->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base, QColor("#181825"));
    ui->tableWidget->setPalette(p);

    // Dates par défaut
    ui->dateEditDebut->setDate(QDate::currentDate());
    ui->dateEditFin->setDate(QDate::currentDate().addDays(30));

    // --- Connexions Boutons CRUD ---
    connect(ui->pushButton,   &QPushButton::clicked, this, &MainWindow::onAjouterClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onModifierClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onSupprimerClicked);
    connect(ui->tableWidget,  &QTableWidget::cellClicked, this, &MainWindow::onLigneSelectionnee);

    // --- Connexions Statistiques, PDF, Alertes & Classement ---
    connect(ui->btnStatistiques, &QPushButton::clicked, this, &MainWindow::onStatistiquesClicked);
    connect(ui->btnExportPdf,    &QPushButton::clicked, this, &MainWindow::onExportPdfClicked);
    connect(ui->btnAlertes,      &QPushButton::clicked, this, &MainWindow::onAlertesClicked);
    connect(ui->btnClassement,   &QPushButton::clicked, this, &MainWindow::onClassementClicked);

    // --- Connexions Recherche / Tri / Filtre (Compatibilité Qt 6 / Qt 5) ---
    connect(ui->lineEditRecherche, &QLineEdit::textChanged, this, &MainWindow::onRechercheChanged);
    connect(ui->comboFiltreNiveau, &QComboBox::currentIndexChanged, this, &MainWindow::onRechercheChanged);
    connect(ui->comboTri, &QComboBox::currentIndexChanged, this, &MainWindow::onRechercheChanged);
    connect(ui->btnReinit, &QPushButton::clicked, this, &MainWindow::onReinitClicked);

    rafraichirTableau();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─────────────────────────────────────────────────────────────
//  RECHERCHE & TRI
// ─────────────────────────────────────────────────────────────

void MainWindow::onRechercheChanged()
{
    if (!ui || !ui->lineEditRecherche || !ui->comboFiltreNiveau || !ui->comboTri) return;

    QString intitule = ui->lineEditRecherche->text().trimmed();
    int idx = ui->comboFiltreNiveau->currentIndex();
    QString niveau = (idx <= 0) ? QString() : ui->comboFiltreNiveau->currentText();

    QString colonne = "date_debut";
    bool ascendant = true;
    switch (ui->comboTri->currentIndex()) {
        case 0: colonne = "intitule";     ascendant = true;  break;
        case 1: colonne = "intitule";     ascendant = false; break;
        case 2: colonne = "duree_heures"; ascendant = true;  break;
        case 3: colonne = "duree_heures"; ascendant = false; break;
        case 4: colonne = "date_debut";   ascendant = false; break;
        case 5: colonne = "capacite_max"; ascendant = false; break;
    }

    QList<Cours> resultats = m_dao.rechercherMulticritere(intitule, niveau, QDate(), QDate(), colonne, ascendant);
    afficherCours(resultats);
}

void MainWindow::onReinitClicked()
{
    if (!ui) return;
    ui->lineEditRecherche->clear();
    ui->comboFiltreNiveau->setCurrentIndex(0);
    ui->comboTri->setCurrentIndex(0);
    rafraichirTableau();
}

// ─────────────────────────────────────────────────────────────
//  AFFICHAGE & RAFRAÎCHISSEMENT TABLEAU
// ─────────────────────────────────────────────────────────────

void MainWindow::afficherCours(const QList<Cours> &liste)
{
    if (!ui || !ui->tableWidget) return;

    ui->tableWidget->setRowCount(0);

    for (const Cours &c : liste) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setRowHeight(row, 36);

        auto item = [](const QString &txt, Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        ui->tableWidget->setItem(row, 0, item(QString::number(c.getId())));
        ui->tableWidget->setItem(row, 1, item(c.getIntitule()));
        ui->tableWidget->setItem(row, 2, item(c.getDescription()));
        ui->tableWidget->setItem(row, 3, item(QString::number(c.getDureeHeures()) + " h", Qt::AlignCenter));

        // Badge couleur pour le Niveau
        QTableWidgetItem *niveauItem = new QTableWidgetItem(c.getNiveau());
        niveauItem->setTextAlignment(Qt::AlignCenter);
        niveauItem->setFlags(niveauItem->flags() & ~Qt::ItemIsEditable);
        if (c.getNiveau() == "Débutant" || c.getNiveau() == "Debutant")
            niveauItem->setForeground(QColor("#a6e3a1"));
        else if (c.getNiveau() == "Intermédiaire" || c.getNiveau() == "Intermediaire")
            niveauItem->setForeground(QColor("#f9e2af"));
        else if (c.getNiveau() == "Avancé" || c.getNiveau() == "Avance")
            niveauItem->setForeground(QColor("#f38ba8"));
        ui->tableWidget->setItem(row, 4, niveauItem);

        ui->tableWidget->setItem(row, 5, item(c.getDateDebut().toString("dd/MM/yyyy"), Qt::AlignCenter));
        ui->tableWidget->setItem(row, 6, item(c.getDateFin().toString("dd/MM/yyyy"), Qt::AlignCenter));
        ui->tableWidget->setItem(row, 7, item(QString::number(c.getCapaciteMax()), Qt::AlignCenter));
    }

    if (ui->labelResultats)
        ui->labelResultats->setText(QString("  %1 cours trouvé(s)").arg(liste.size()));
    if (statusBar())
        statusBar()->showMessage(QString("Base connectée  •  %1 cours affiché(s)").arg(liste.size()));
}

void MainWindow::rafraichirTableau()
{
    if (!DatabaseManager::instance().isConnected()) {
        if (statusBar())
            statusBar()->showMessage("Erreur : Base de données non connectée.");
        return;
    }
    afficherCours(m_dao.afficherTous());
}

// ─────────────────────────────────────────────────────────────
//  FORMULAIRE (GETTERS / SETTERS)
// ─────────────────────────────────────────────────────────────

Cours MainWindow::construireCoursDepuisFormulaire() const
{
    return Cours(
        m_idSelectionne,
        ui->lineEditIntitule->text().trimmed(),
        ui->textEditDescription->toPlainText().trimmed(),
        ui->spinBoxDuree->value(),
        ui->comboBoxNiveau->currentText(),
        ui->dateEditDebut->date(),
        ui->dateEditFin->date(),
        ui->spinBoxCapacite->value()
    );
}

void MainWindow::remplirFormulaire(const Cours &c)
{
    ui->lineEditId->setText(QString::number(c.getId()));
    ui->lineEditIntitule->setText(c.getIntitule());
    ui->textEditDescription->setPlainText(c.getDescription());
    ui->spinBoxDuree->setValue(c.getDureeHeures());
    ui->comboBoxNiveau->setCurrentText(c.getNiveau());
    ui->dateEditDebut->setDate(c.getDateDebut());
    ui->dateEditFin->setDate(c.getDateFin());
    ui->spinBoxCapacite->setValue(c.getCapaciteMax());
    m_idSelectionne = c.getId();
}

void MainWindow::viderFormulaire()
{
    ui->lineEditId->clear();
    ui->lineEditIntitule->clear();
    ui->textEditDescription->clear();
    ui->spinBoxDuree->setValue(20);
    ui->comboBoxNiveau->setCurrentIndex(0);
    ui->dateEditDebut->setDate(QDate::currentDate());
    ui->dateEditFin->setDate(QDate::currentDate().addDays(30));
    ui->spinBoxCapacite->setValue(20);
    ui->labelPlacesRestantes->clear();
    m_idSelectionne = -1;
}

void MainWindow::onLigneSelectionnee(int row, int)
{
    if (row < 0 || row >= ui->tableWidget->rowCount()) return;
    QTableWidgetItem *item0 = ui->tableWidget->item(row, 0);
    if (!item0) return;

    int id = item0->text().toInt();
    Cours c = m_dao.afficherParId(id);
    if (c.getId() != -1) {
        remplirFormulaire(c);

        // Afficher les places disponibles pour le cours sélectionné
        int places = m_dao.placesDisponibles(id);
        ui->labelPlacesRestantes->setText(
            QString("Places disponibles : %1").arg(places));
    }
}

// ─────────────────────────────────────────────────────────────
//  ACTIONS CRUD (AJOUTER / MODIFIER / SUPPRIMER)
// ─────────────────────────────────────────────────────────────

void MainWindow::onAjouterClicked()
{
    Cours c = construireCoursDepuisFormulaire();
    QString erreur;
    if (!c.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }

    if (m_dao.ajouter(c)) {
        rafraichirTableau();
        viderFormulaire();
        QMessageBox::information(this, "Succès", "Le cours a été ajouté avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur d'ajout",
            QString("Échec de l'ajout du cours dans la base de données.\n\nDétail Oracle : %1")
                .arg(m_dao.dernierErreur()));
    }
}

void MainWindow::onModifierClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection", "Veuillez sélectionner un cours dans le tableau.");
        return;
    }

    Cours c = construireCoursDepuisFormulaire();
    QString erreur;
    if (!c.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }

    if (m_dao.modifier(c)) {
        rafraichirTableau();
        viderFormulaire();
        QMessageBox::information(this, "Succès", "Le cours a été modifié avec succès !");
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la modification du cours.\n\nDétail Oracle : %1")
                .arg(m_dao.dernierErreur()));
    }
}

void MainWindow::onSupprimerClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection", "Veuillez sélectionner un cours dans le tableau.");
        return;
    }

    auto rep = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce cours ?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (rep != QMessageBox::Yes) return;

    if (m_dao.supprimer(m_idSelectionne)) {
        rafraichirTableau();
        viderFormulaire();
        QMessageBox::information(this, "Succès", "Le cours a été supprimé !");
    } else {
        QMessageBox::critical(this, "Erreur",
            QString("Échec de la suppression du cours.\n\nDétail Oracle : %1")
                .arg(m_dao.dernierErreur()));
    }
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
//  ALERTES
// ─────────────────────────────────────────────────────────────
void MainWindow::onAlertesClicked()
{
    AlertesCoursDialog dialog(this);
    dialog.exec();
}

// ─────────────────────────────────────────────────────────────
//  CLASSEMENT
// ─────────────────────────────────────────────────────────────
void MainWindow::onClassementClicked()
{
    ClassementCoursDialog dialog(this);
    dialog.exec();
}

// ─────────────────────────────────────────────────────────────
//  EXPORT PDF
// ─────────────────────────────────────────────────────────────
void MainWindow::onExportPdfClicked()
{
    if (m_idSelectionne != -1) {
        // Un cours est sélectionné -> Générer la fiche PDF de ce cours
        Cours c = m_dao.afficherParId(m_idSelectionne);
        if (c.getId() == -1) {
            QMessageBox::warning(this, "Erreur", "Impossible de charger les données du cours.");
            return;
        }

        QString nomFichier = QString("Fiche_Cours_%1.pdf").arg(c.getIntitule().trimmed().replace(" ", "_"));
        QString chemin = QFileDialog::getSaveFileName(
            this,
            "Enregistrer la fiche du cours en PDF",
            QDir::homePath() + "/" + nomFichier,
            "Fichiers PDF (*.pdf)");

        if (chemin.isEmpty()) return;

        if (PdfGenerator::genererFicheCours(c, chemin)) {
            int rep = QMessageBox::information(
                this, "PDF généré",
                QString("Fiche du cours créée avec succès :\n%1\n\nVoulez-vous l'ouvrir ?").arg(chemin),
                QMessageBox::Yes | QMessageBox::No);

            if (rep == QMessageBox::Yes)
                QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
        } else {
            QMessageBox::critical(this, "Erreur PDF",
                "La génération du PDF a échoué.\nVérifiez que le dossier de destination est accessible.");
        }
    } else {
        // Aucun cours sélectionné -> Proposer d'exporter le catalogue complet
        QList<Cours> tous = m_dao.afficherTous();
        if (tous.isEmpty()) {
            QMessageBox::information(this, "Information", "Aucun cours disponible à exporter.");
            return;
        }

        int choix = QMessageBox::question(
            this, "Export PDF",
            "Aucun cours n'est sélectionné.\nVoulez-vous exporter le catalogue complet de tous les cours au format PDF ?",
            QMessageBox::Yes | QMessageBox::No);

        if (choix != QMessageBox::Yes) return;

        QString nomFichier = "Catalogue_Des_Cours.pdf";
        QString chemin = QFileDialog::getSaveFileName(
            this,
            "Enregistrer le catalogue des cours en PDF",
            QDir::homePath() + "/" + nomFichier,
            "Fichiers PDF (*.pdf)");

        if (chemin.isEmpty()) return;

        if (PdfGenerator::genererCatalogue(tous, chemin)) {
            int rep = QMessageBox::information(
                this, "PDF généré",
                QString("Catalogue PDF créé avec succès :\n%1\n\nVoulez-vous l'ouvrir ?").arg(chemin),
                QMessageBox::Yes | QMessageBox::No);

            if (rep == QMessageBox::Yes)
                QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
        } else {
            QMessageBox::critical(this, "Erreur PDF",
                "La génération du PDF a échoué.\nVérifiez que le dossier de destination est accessible.");
        }
    }
}
