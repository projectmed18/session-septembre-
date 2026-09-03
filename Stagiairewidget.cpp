#include "Stagiairewidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>
#include <QAbstractItemView>

StagiaireWidget::StagiaireWidget(QWidget *parent)
    : QWidget(parent), m_idSelectionne(-1)
{
    // --- Widgets formulaire ---
    editNom       = new QLineEdit(this);
    editPrenom    = new QLineEdit(this);
    editEmail     = new QLineEdit(this);
    editTelephone = new QLineEdit(this);

    dateNaissance = new QDateEdit(QDate(2000, 1, 1), this);
    dateNaissance->setCalendarPopup(true);
    dateNaissance->setDisplayFormat("dd/MM/yyyy");

    comboNiveau = new QComboBox(this);
    comboNiveau->addItems({"Debutant", "Intermediaire", "Avance"});

    btnAjouter   = new QPushButton("+ Ajouter",  this);
    btnModifier  = new QPushButton("Modifier",   this);
    btnSupprimer = new QPushButton("Supprimer",  this);

    // --- Formulaire (layout gauche) ---
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Nom :",            editNom);
    formLayout->addRow("Prénom :",         editPrenom);
    formLayout->addRow("Email :",          editEmail);
    formLayout->addRow("Téléphone :",      editTelephone);
    formLayout->addRow("Date naissance :", dateNaissance);
    formLayout->addRow("Niveau :",         comboNiveau);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(formLayout);
    leftLayout->addSpacing(16);
    leftLayout->addWidget(btnAjouter);
    leftLayout->addWidget(btnModifier);
    leftLayout->addWidget(btnSupprimer);
    leftLayout->addStretch();

    // --- Tableau (layout droit) ---
    tableView = new QTableView(this);
    m_model   = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels(
        {"ID", "Nom", "Prénom", "Email", "Téléphone", "D.nais", "Niveau", "D.insc"});

    tableView->setModel(m_model);
    tableView->setColumnHidden(0, true);   // ID caché mais présent dans le modèle
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->horizontalHeader()->setStretchLastSection(true);

    // --- Layout principal ---
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(tableView, 3);
    setLayout(mainLayout);

    // --- Connexions signaux/slots ---
    connect(btnAjouter,   &QPushButton::clicked, this, &StagiaireWidget::onAjouterClicked);
    connect(btnModifier,  &QPushButton::clicked, this, &StagiaireWidget::onModifierClicked);
    connect(btnSupprimer, &QPushButton::clicked, this, &StagiaireWidget::onSupprimerClicked);
    connect(tableView,    &QTableView::clicked,  this, &StagiaireWidget::onLigneTableauSelectionnee);

    rafraichirTableau();
}

// --- Récupère les valeurs du formulaire dans un objet Stagiaire ---
Stagiaire StagiaireWidget::construireStagiaireDepuisFormulaire() const
{
    return Stagiaire(
        m_idSelectionne,
        editNom->text().trimmed(),
        editPrenom->text().trimmed(),
        editEmail->text().trimmed(),
        editTelephone->text().trimmed(),
        dateNaissance->date(),
        comboNiveau->currentText());
}

// --- Pré-remplit le formulaire à partir d'une ligne sélectionnée ---
void StagiaireWidget::remplirFormulaire(const Stagiaire &s)
{
    editNom->setText(s.getNom());
    editPrenom->setText(s.getPrenom());
    editEmail->setText(s.getEmail());
    editTelephone->setText(s.getTelephone());
    dateNaissance->setDate(s.getDateNaissance());
    comboNiveau->setCurrentText(s.getNiveau());
    m_idSelectionne = s.getId();
}

void StagiaireWidget::viderFormulaire()
{
    editNom->clear();
    editPrenom->clear();
    editEmail->clear();
    editTelephone->clear();
    dateNaissance->setDate(QDate(2000, 1, 1));
    comboNiveau->setCurrentIndex(0);
    m_idSelectionne = -1;
}

// --- Clic sur une ligne du tableau ---
void StagiaireWidget::onLigneTableauSelectionnee(const QModelIndex &index)
{
    int ligne = index.row();
    Stagiaire s(
        m_model->item(ligne, 0)->text().toInt(),
        m_model->item(ligne, 1)->text(),
        m_model->item(ligne, 2)->text(),
        m_model->item(ligne, 3)->text(),
        m_model->item(ligne, 4)->text(),
        QDate::fromString(m_model->item(ligne, 5)->text(), "dd/MM/yyyy"),
        m_model->item(ligne, 6)->text(),
        QDate::fromString(m_model->item(ligne, 7)->text(), "dd/MM/yyyy"));
    remplirFormulaire(s);
}

void StagiaireWidget::onAjouterClicked()
{
    Stagiaire s = construireStagiaireDepuisFormulaire();

    QString erreur;
    if (!s.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }
    if (m_dao.emailExiste(s.getEmail())) {
        QMessageBox::warning(this, "Email déjà utilisé",
                             "Un stagiaire avec cet email existe déjà.");
        return;
    }

    if (m_dao.ajouter(s)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout en base.");
    }
}

void StagiaireWidget::onModifierClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
                                 "Sélectionnez un stagiaire dans le tableau.");
        return;
    }

    Stagiaire s = construireStagiaireDepuisFormulaire();
    QString erreur;
    if (!s.estValide(erreur)) {
        QMessageBox::warning(this, "Saisie invalide", erreur);
        return;
    }
    if (m_dao.emailExiste(s.getEmail(), m_idSelectionne)) {
        QMessageBox::warning(this, "Email déjà utilisé",
                             "Un autre stagiaire utilise déjà cet email.");
        return;
    }

    if (m_dao.modifier(s)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification.");
    }
}

void StagiaireWidget::onSupprimerClicked()
{
    if (m_idSelectionne == -1) {
        QMessageBox::information(this, "Aucune sélection",
                                 "Sélectionnez un stagiaire dans le tableau.");
        return;
    }

    auto reponse = QMessageBox::question(
        this, "Confirmation", "Supprimer ce stagiaire ?",
        QMessageBox::Yes | QMessageBox::No);
    if (reponse != QMessageBox::Yes) return;

    if (m_dao.supprimer(m_idSelectionne)) {
        rafraichirTableau();
        viderFormulaire();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression.");
    }
}

void StagiaireWidget::rafraichirTableau()
{
    m_model->removeRows(0, m_model->rowCount());

    const QList<Stagiaire> stagiaires = m_dao.afficherTous();
    for (const Stagiaire &s : stagiaires) {
        QList<QStandardItem *> ligne;
        ligne << new QStandardItem(QString::number(s.getId()))
              << new QStandardItem(s.getNom())
              << new QStandardItem(s.getPrenom())
              << new QStandardItem(s.getEmail())
              << new QStandardItem(s.getTelephone())
              << new QStandardItem(s.getDateNaissance().toString("dd/MM/yyyy"))
              << new QStandardItem(s.getNiveau())
              << new QStandardItem(s.getDateInscription().toString("dd/MM/yyyy"));
        m_model->appendRow(ligne);
    }
}
