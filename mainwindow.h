#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stagiairedao.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onLigneSelectionnee(int row, int column);
    void onFicheSuiviDemandee(int row, int column); // double-clic → fiche suivi
    void onRechercheChanged();
    void onReinitClicked();
    void onCapaciteClicked();
    void onStatistiquesClicked();
    void onFicheSuiviClicked();     // bouton Fiche de Suivi
    void onExportPdfClicked();

private:
    void rafraichirTableau();
    void afficherStagiaires(const QList<Stagiaire> &liste);
    void viderFormulaire();
    Stagiaire construireStagiaireDepuisFormulaire() const;
    void remplirFormulaire(const Stagiaire &s);

    Ui::MainWindow *ui;
    StagiaireDAO    m_dao;
    int             m_idSelectionne;
};

#endif // MAINWINDOW_H
