#ifndef STAGIAIREWIDGET_H
#define STAGIAIREWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include "stagiairedao.h"
#include "stagiaire.h"

class StagiaireWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StagiaireWidget(QWidget *parent = nullptr);
    ~StagiaireWidget() = default;

private slots:
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onLigneSelectionnee(int row, int column);
    void onFicheSuiviDemandee(int row, int column);
    void onRechercheChanged();
    void onReinitClicked();
    void onCapaciteClicked();
    void onStatistiquesClicked();
    void onFicheSuiviClicked();
    void onExportPdfClicked();

private:
    void rafraichirTableau();
    void afficherStagiaires(const QList<Stagiaire> &liste);
    void viderFormulaire();
    Stagiaire construireStagiaireDepuisFormulaire() const;
    void remplirFormulaire(const Stagiaire &s);

    // Panneau gauche — formulaire
    QLineEdit   *lineEditId;
    QLineEdit   *lineEdit;       // Nom
    QLineEdit   *lineEdit_2;     // Prénom
    QLineEdit   *lineEdit_3;     // Email
    QLineEdit   *lineEdit_4;     // Téléphone
    QDateEdit   *dateEdit;
    QComboBox   *comboBox;       // Niveau
    QPushButton *pushButton;     // Ajouter
    QPushButton *pushButton_2;   // Modifier
    QPushButton *pushButton_3;   // Supprimer

    // Panneau droit — recherche + tableau
    QLineEdit    *lineEditRecherche;
    QComboBox    *comboFiltreNiveau;
    QComboBox    *comboTri;
    QPushButton  *btnReinit;
    QLabel       *labelResultats;
    QTableWidget *tableWidget;

    // Boutons du bas
    QPushButton *btnCapacite;
    QPushButton *btnStatistiques;
    QPushButton *btnFicheSuivi;
    QPushButton *btnExportPdf;

    StagiaireDAO m_dao;
    int          m_idSelectionne;
};

#endif // STAGIAIREWIDGET_H
