#ifndef COURSWIDGET_H
#define COURSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include "coursdao.h"
#include "cours.h"

class CoursWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoursWidget(QWidget *parent = nullptr);
    ~CoursWidget() = default;

private slots:
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onLigneSelectionnee(int row, int col);
    void onRechercheChanged();
    void onReinitClicked();
    void onAlertesClicked();
    void onClassementClicked();
    void onStatistiquesClicked();
    void onExportPdfClicked();

private:
    void rafraichirTableau();
    void afficherCours(const QList<Cours> &liste);
    void viderFormulaire();
    Cours construireCoursDepuisFormulaire() const;
    void  remplirFormulaire(const Cours &c);

    // ── Panneau gauche — formulaire ────────────────────────────────
    QLineEdit    *lineEditId;
    QLineEdit    *lineEditIntitule;
    QPlainTextEdit *plainEditDescription;
    QSpinBox     *spinDuree;
    QComboBox    *comboNiveau;
    QDateEdit    *dateEditDebut;
    QDateEdit    *dateEditFin;
    QSpinBox     *spinCapacite;
    QPushButton  *btnAjouter;
    QPushButton  *btnModifier;
    QPushButton  *btnSupprimer;

    // ── Panneau droit — recherche + tableau ────────────────────────
    QLineEdit    *lineEditRecherche;
    QComboBox    *comboFiltreNiveau;
    QComboBox    *comboTri;
    QPushButton  *btnReinit;
    QLabel       *labelResultats;
    QTableWidget *tableWidget;

    // ── Boutons métier du bas ──────────────────────────────────────
    QPushButton  *btnAlertes;
    QPushButton  *btnClassement;
    QPushButton  *btnStatistiques;
    QPushButton  *btnExportPdf;

    CoursDAO m_dao;
    int      m_idSelectionne;
};

#endif // COURSWIDGET_H
