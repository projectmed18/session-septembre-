#include "classementcoursdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QTableWidgetItem>

ClassementCoursDialog::ClassementCoursDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Classement — Cours les plus demandés");
    resize(680, 460);

    // ── Style global (Catppuccin Mocha) ──────────────────────────────────
    setStyleSheet(
        "QDialog { background-color: #1e1e2e; }"
        "QLabel  { color: #cdd6f4; font: 9pt 'Segoe UI'; background: transparent; }"
        "QTableWidget {"
        "  background-color: #181825; color: #cdd6f4;"
        "  gridline-color: #313244; border: 1px solid #313244;"
        "  border-radius: 8px; font: 9pt 'Segoe UI'; outline: none;"
        "}"
        "QTableWidget::item { padding: 6px 10px; border-bottom: 1px solid #313244; }"
        "QTableWidget::item:selected { background-color: #313244; color: #89b4fa; }"
        "QHeaderView::section {"
        "  background-color: #181825; color: #89b4fa;"
        "  font: bold 9pt 'Segoe UI'; padding: 8px 10px;"
        "  border: none; border-bottom: 2px solid #89b4fa;"
        "}"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // ── Titre ─────────────────────────────────────────────────────────────
    auto *titre = new QLabel("Top 10 des cours par nombre d'inscrits actifs", this);
    titre->setStyleSheet("font: bold 11pt 'Segoe UI'; color: #cba6f7;");
    layout->addWidget(titre);

    // ── Légende couleurs ──────────────────────────────────────────────────
    auto *legendeLayout = new QHBoxLayout();
    auto makeLegende = [](const QString &couleur, const QString &texte) -> QLabel* {
        auto *l = new QLabel(QString("<b style='color:%1'>■</b> %2").arg(couleur, texte));
        l->setStyleSheet("font: 8pt 'Segoe UI'; color: #6c7086; background: transparent;");
        return l;
    };
    legendeLayout->addWidget(makeLegende("#f38ba8", "≥ 80% (très demandé)"));
    legendeLayout->addWidget(makeLegende("#f9e2af", "≥ 40%"));
    legendeLayout->addWidget(makeLegende("#a6e3a1", "< 40%"));
    legendeLayout->addStretch();
    layout->addLayout(legendeLayout);

    // ── Tableau ───────────────────────────────────────────────────────────
    m_tableau = new QTableWidget(this);
    m_tableau->setColumnCount(5);
    m_tableau->setHorizontalHeaderLabels(
        {"#", "Intitulé", "Inscrits", "Capacité", "Taux de remplissage"});
    m_tableau->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableau->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableau->verticalHeader()->setVisible(false);
    m_tableau->horizontalHeader()->setStretchLastSection(true);
    m_tableau->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableau->setAlternatingRowColors(true);
    QPalette p = m_tableau->palette();
    p.setColor(QPalette::AlternateBase, QColor("#252535"));
    p.setColor(QPalette::Base,          QColor("#181825"));
    m_tableau->setPalette(p);

    layout->addWidget(m_tableau);

    chargerClassement();
}

void ClassementCoursDialog::chargerClassement()
{
    m_tableau->setRowCount(0);

    const QList<CoursDAO::CoursClassement> liste = m_dao.coursLesPlusDemandes(10);

    if (liste.isEmpty()) {
        m_tableau->insertRow(0);
        auto *item = new QTableWidgetItem("Aucun cours inscrit trouvé.");
        item->setForeground(QColor("#6c7086"));
        m_tableau->setItem(0, 1, item);
        return;
    }

    int rang = 1;
    for (const auto &cc : liste) {
        int row = m_tableau->rowCount();
        m_tableau->insertRow(row);
        m_tableau->setRowHeight(row, 36);

        // Choisir la couleur selon le taux
        QColor couleur;
        if (cc.tauxRemplissage >= 80.0)
            couleur = QColor("#f38ba8"); // rouge
        else if (cc.tauxRemplissage >= 40.0)
            couleur = QColor("#f9e2af"); // orange
        else
            couleur = QColor("#a6e3a1"); // vert

        auto makeItem = [](const QString &txt,
                           Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft)
        {
            auto *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

        // Colonne # (rang)
        auto *rangItem = makeItem(QString::number(rang++), Qt::AlignCenter);
        rangItem->setForeground(QColor("#6c7086"));
        m_tableau->setItem(row, 0, rangItem);

        // Intitulé
        m_tableau->setItem(row, 1, makeItem(cc.intitule));

        // Inscrits
        auto *inscritsItem = makeItem(QString::number(cc.nbInscrits), Qt::AlignCenter);
        inscritsItem->setForeground(couleur);
        m_tableau->setItem(row, 2, inscritsItem);

        // Capacité
        m_tableau->setItem(row, 3,
            makeItem(QString::number(cc.capaciteMax), Qt::AlignCenter));

        // Taux de remplissage (coloré)
        auto *tauxItem = makeItem(
            QString("%1 %").arg(cc.tauxRemplissage, 0, 'f', 1), Qt::AlignCenter);
        tauxItem->setForeground(couleur);
        m_tableau->setItem(row, 4, tauxItem);
    }

    // Ajuster la largeur des colonnes fixes
    m_tableau->setColumnWidth(0, 40);   // #
    m_tableau->setColumnWidth(2, 80);   // Inscrits
    m_tableau->setColumnWidth(3, 80);   // Capacité
    m_tableau->setColumnWidth(4, 140);  // Taux
}
