#ifndef CAPACITEDIALOG_H
#define CAPACITEDIALOG_H

#include <QDialog>
#include "coursdao.h"

#include <QTableWidget>
#include <QLabel>
#include <QList>
#include "coursdao.h"

class CapaciteDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CapaciteDialog(QWidget *parent = nullptr);

private slots:
    void onInscrireClicked();
    void chargerDonnees();

private:
    QTableWidget *m_table;
    QLabel       *m_labelLegendeHeader;
    QLabel       *m_valTotal;
    QLabel       *m_valDispo;
    QLabel       *m_valPlein;
    CoursDAO      m_dao;
    QList<CoursDAO::InfoCapacite> m_cours;
};

#endif // CAPACITEDIALOG_H
