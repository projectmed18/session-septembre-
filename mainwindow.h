#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Stagiairewidget.h"
#include "CoursWidget.h"
#include "InscriptionWidget.h"

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
    void onStagiairesClicked();
    void onCoursClicked();
    void onInscriptionsClicked();

private:
    void setHeader(const QString &titre, const QString &sousTitre,
                   const QString &couleur);

    Ui::MainWindow     *ui;
    StagiaireWidget    *m_stagiaireWidget;
    CoursWidget        *m_coursWidget;
    InscriptionWidget  *m_inscriptionWidget;
};

#endif // MAINWINDOW_H
