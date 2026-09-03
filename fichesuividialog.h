#ifndef FICHESUIVIDIALOG_H
#define FICHESUIVIDIALOG_H

#include <QDialog>
#include "stagiaire.h"
#include "stagiairedao.h"

class FicheSuiviDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FicheSuiviDialog(const Stagiaire &stagiaire, QWidget *parent = nullptr);
};

#endif // FICHESUIVIDIALOG_H
