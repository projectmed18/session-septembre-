#ifndef INSCRIPTIONWIDGET_H
#define INSCRIPTIONWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include "inscriptiondao.h"
#include "stagiairedao.h"
#include "coursdao.h"
#include "inscription.h"

class InscriptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InscriptionWidget(QWidget *parent = nullptr);
    ~InscriptionWidget() = default;

protected:
    // Rechargement automatique à chaque affichage du widget
    void showEvent(QShowEvent *event) override;

private slots:
    void onInscrireClicked();
    void onModifierStatutClicked();
    void onDesinscrireClicked();
    void onLigneSelectionnee(int row, int col);
    void onRechercheChanged();
    void onReinitClicked();
    void onCoursSelectionChanged(int index);   // affiche places restantes

private:
    void rafraichirTableau(bool rechargerCombos = false);
    void afficherInscriptions(const QList<Inscription> &liste);
    void chargerComboStagiaires();
    void chargerCombosCours();
    void viderFormulaire();

    // ── Panneau gauche ─────────────────────────────────────────────
    QComboBox   *comboStagiaire;
    QComboBox   *comboCours;
    QLabel      *labelPlaces;       // places restantes en temps réel
    QDateEdit   *dateEditInscription;
    QComboBox   *comboStatut;
    QPushButton *btnInscrire;
    QPushButton *btnModifierStatut;
    QPushButton *btnDesinscrire;

    // ── Panneau droit ──────────────────────────────────────────────
    QLineEdit    *lineEditRecherche;
    QComboBox    *comboFiltreStatut;
    QComboBox    *comboTri;
    QPushButton  *btnReinit;
    QLabel       *labelResultats;
    QTableWidget *tableWidget;

    // ── Stats en haut du panneau droit ────────────────────────────
    QLabel *labelStatTotal;
    QLabel *labelStatEnCours;
    QLabel *labelStatTermine;
    QLabel *labelStatAnnule;

    InscriptionDAO m_dao;
    StagiaireDAO   m_dayStagiaire;
    CoursDAO       m_daoCours;
    int            m_idSelectionne;
};

#endif // INSCRIPTIONWIDGET_H
