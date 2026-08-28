#include "pdfgeneratorcours.h"

#include <QPdfWriter>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QRect>
#include <QDate>
#include <QDebug>

bool PdfGeneratorCours::genererRapport(
    const QList<CoursDAO::InfoCapacite> &cours,
    const QString &cheminFichier)
{
    QPdfWriter writer(cheminFichier);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    writer.setResolution(150);

    QPainter p;
    if (!p.begin(&writer)) {
        qWarning() << "PdfGeneratorCours : impossible d'ouvrir" << cheminFichier;
        return false;
    }

    const int W = p.device()->width();
    const int H = p.device()->height();

    auto col = [](const QString &hex) { return QColor(hex); };

    // ── Fond ─────────────────────────────────────────────────────
    p.fillRect(0, 0, W, H, col("#1e1e2e"));

    // ── En-tête ───────────────────────────────────────────────────
    const int HEADER_H = H / 8;
    p.fillRect(0, 0, W, HEADER_H, col("#cba6f7"));

    p.setPen(col("#1e1e2e"));
    p.setFont(QFont("Segoe UI", 22, QFont::Bold));
    p.drawText(QRect(0, 10, W, HEADER_H / 2),
               Qt::AlignHCenter | Qt::AlignVCenter, "CENTRE DE FORMATION");

    p.setFont(QFont("Segoe UI", 12));
    p.drawText(QRect(0, HEADER_H / 2, W, HEADER_H / 2),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "Rapport des Cours — " + QDate::currentDate().toString("dd/MM/yyyy"));

    // ── Titre section ─────────────────────────────────────────────
    int y = HEADER_H + 40;
    p.setPen(col("#cdd6f4"));
    p.setFont(QFont("Segoe UI", 16, QFont::Bold));
    p.drawText(QRect(40, y, W - 80, 50),
               Qt::AlignLeft | Qt::AlignVCenter, "Liste des cours");

    y += 50;
    p.setPen(QPen(col("#cba6f7"), 2));
    p.drawLine(40, y, W - 40, y);
    y += 20;

    // ── Résumé stats ──────────────────────────────────────────────
    int total = cours.size();
    int complet = 0, dispo = 0;
    int totalInscrits = 0;
    for (const auto &c : cours) {
        if (c.placesRestantes <= 0) complet++;
        else dispo++;
        totalInscrits += c.nbInscrits;
    }

    // 3 cartes stat
    const int cardW = (W - 80 - 40) / 3;
    const int cardH = 80;
    struct { QString val; QString lbl; QString hex; } cards[] = {
        {QString::number(total),         "Total cours",    "#89b4fa"},
        {QString::number(totalInscrits), "Total inscrits", "#a6e3a1"},
        {QString::number(complet),       "Cours complets", "#f38ba8"}
    };

    for (int i = 0; i < 3; i++) {
        int cx = 40 + i * (cardW + 20);
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(col("#181825")));
        p.drawRoundedRect(cx, y, cardW, cardH, 8, 8);
        p.setBrush(QBrush(col(cards[i].hex)));
        p.drawRoundedRect(cx, y, 8, cardH, 4, 4);

        p.setPen(col(cards[i].hex));
        p.setFont(QFont("Segoe UI", 18, QFont::Bold));
        p.drawText(QRect(cx + 20, y, cardW - 20, cardH / 2 + 10),
                   Qt::AlignLeft | Qt::AlignVCenter, cards[i].val);
        p.setPen(col("#6c7086"));
        p.setFont(QFont("Segoe UI", 9));
        p.drawText(QRect(cx + 20, y + cardH / 2, cardW - 20, cardH / 2),
                   Qt::AlignLeft | Qt::AlignVCenter, cards[i].lbl);
    }
    y += cardH + 30;

    // ── En-tête du tableau ────────────────────────────────────────
    const int COL_W[] = {
        int(W * 0.28), int(W * 0.14), int(W * 0.13),
        int(W * 0.13), int(W * 0.11), int(W * 0.21)
    };
    const QString COLS[] = {"Intitulé", "Niveau", "Date début", "Date fin",
                             "Capacité", "Remplissage"};
    const int ROW_H = 38;

    p.fillRect(40, y, W - 80, ROW_H, col("#313244"));
    p.setPen(col("#cba6f7"));
    p.setFont(QFont("Segoe UI", 9, QFont::Bold));
    int x = 40;
    for (int c = 0; c < 6; c++) {
        p.drawText(QRect(x + 6, y, COL_W[c] - 6, ROW_H),
                   Qt::AlignVCenter | Qt::AlignLeft, COLS[c]);
        x += COL_W[c];
    }
    y += ROW_H;

    // ── Lignes du tableau ─────────────────────────────────────────
    p.setFont(QFont("Segoe UI", 8));
    bool alt = false;
    for (const auto &c : cours) {
        if (y + ROW_H > H - 60) {
            writer.newPage();
            p.fillRect(0, 0, W, H, col("#1e1e2e"));
            y = 40;
        }

        p.fillRect(40, y, W - 80, ROW_H,
                   alt ? col("#252535") : col("#181825"));
        alt = !alt;

        // Bordure gauche colorée selon état
        QColor lineCol = (c.placesRestantes <= 0) ? col("#f38ba8")
            : (c.capaciteMax > 0 && c.placesRestantes <= c.capaciteMax * 0.2)
              ? col("#fab387") : col("#a6e3a1");
        p.fillRect(40, y, 4, ROW_H, lineCol);

        p.setPen(col("#cdd6f4"));
        x = 40;
        QStringList vals = {
            c.intitule,
            c.niveau,
            c.dateDebut.toString("dd/MM/yy"),
            c.dateFin.toString("dd/MM/yy"),
            QString::number(c.capaciteMax)
        };
        for (int i = 0; i < 5; i++) {
            p.drawText(QRect(x + 8, y, COL_W[i] - 8, ROW_H),
                       Qt::AlignVCenter | Qt::AlignLeft, vals[i]);
            x += COL_W[i];
        }

        // Taux remplissage avec mini barre
        int pct = (c.capaciteMax > 0)
            ? qMin(100, c.nbInscrits * 100 / c.capaciteMax) : 0;
        int barW = COL_W[5] - 60;
        int barH = 10;
        int barX = x + 8;
        int barY = y + (ROW_H - barH) / 2;

        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(col("#313244")));
        p.drawRoundedRect(barX, barY, barW, barH, 4, 4);
        if (pct > 0) {
            p.setBrush(QBrush(lineCol));
            p.drawRoundedRect(barX, barY, barW * pct / 100, barH, 4, 4);
        }

        p.setPen(col("#cdd6f4"));
        p.setFont(QFont("Segoe UI", 8, QFont::Bold));
        p.drawText(QRect(barX + barW + 6, y, 40, ROW_H),
                   Qt::AlignVCenter | Qt::AlignLeft,
                   QString("%1%").arg(pct));
        p.setFont(QFont("Segoe UI", 8));

        y += ROW_H;
    }

    // ── Pied de page ──────────────────────────────────────────────
    p.setPen(QPen(col("#313244"), 1));
    p.drawLine(40, H - 50, W - 40, H - 50);
    p.setPen(col("#6c7086"));
    p.setFont(QFont("Segoe UI", 8));
    p.drawText(QRect(40, H - 40, W - 80, 30),
               Qt::AlignVCenter | Qt::AlignLeft,
               "Généré le " + QDate::currentDate().toString("dd/MM/yyyy") +
               " — Centre de Formation");
    p.drawText(QRect(40, H - 40, W - 80, 30),
               Qt::AlignVCenter | Qt::AlignRight,
               QString("Total : %1 cours | %2 inscrits")
                   .arg(total).arg(totalInscrits));

    p.end();
    return true;
}
