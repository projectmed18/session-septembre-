#ifndef STAGIAIREWIDGET_H
#define STAGIAIREWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include "stagiairedao.h"

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
    void onLigneTableauSelectionnee(const QModelIndex &index);

private:
    void rafraichirTableau();
    void viderFormulaire();
    Stagiaire construireStagiaireDepuisFormulaire() const;
    void remplirFormulaire(const Stagiaire &s);

    // Widgets formulaire
    QLineEdit        *editNom;
    QLineEdit        *editPrenom;
    QLineEdit        *editEmail;
    QLineEdit        *editTelephone;
    QDateEdit        *dateNaissance;
    QComboBox        *comboNiveau;
    QPushButton      *btnAjouter;
    QPushButton      *btnModifier;
    QPushButton      *btnSupprimer;

    // Tableau
    QTableView       *tableView;
    QStandardItemModel *m_model;

    StagiaireDAO      m_dao;
    int               m_idSelectionne; // -1 si aucune sélection
};

#endif // STAGIAIREWIDGET_H
