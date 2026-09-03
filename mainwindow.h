#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cours.h"
#include "coursdao.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
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
    void onRechercheChanged();
    void onReinitClicked();
    void onStatistiquesClicked();
    void onExportPdfClicked();
    void onAlertesClicked();
    void onClassementClicked();

private:
    void rafraichirTableau();
    void afficherCours(const QList<Cours> &liste);
    Cours construireCoursDepuisFormulaire() const;
    void remplirFormulaire(const Cours &c);
    void viderFormulaire();

    Ui::MainWindow *ui;
    CoursDAO m_dao;
    int m_idSelectionne;
};
#endif // MAINWINDOW_H
