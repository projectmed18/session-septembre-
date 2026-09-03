#include "pdfgenerator.h"

#include <QPdfWriter>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QRect>
#include <QDate>
#include <QDebug>

// ─── Constantes de mise en page (unités : points PDF 1/72 pouce) ──────────────
static const int PAGE_W  = 595 * 10;   // A4 largeur  (en unités QPdfWriter 1/10 mm → on travaille en logicalDpi)
static const int PAGE_H  = 842 * 10;   // A4 hauteur

bool PdfGenerator::genererAttestation(const Stagiaire &s, const QString &cheminFichier)
{
    QPdfWriter writer(cheminFichier);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    writer.setResolution(150);

    QPainter p;
    if (!p.begin(&writer)) {
        qWarning() << "PdfGenerator : impossible d'ouvrir" << cheminFichier;
        return false;
    }

    // ── dimensions réelles de la zone de dessin ──────────────────────────────
    const int W = p.device()->width();
    const int H = p.device()->height();

    auto couleur = [](const QString &hex) { return QColor(hex); };

    // ════════════════════════════════════════════════════════════════
    //  FOND
    // ════════════════════════════════════════════════════════════════
    p.fillRect(0, 0, W, H, couleur("#1e1e2e"));

    // ════════════════════════════════════════════════════════════════
    //  BANDE D'EN-TÊTE
    // ════════════════════════════════════════════════════════════════
    int headerH = H / 7;
    p.fillRect(0, 0, W, headerH, couleur("#89b4fa"));

    // Titre principal
    p.setPen(couleur("#1e1e2e"));
    QFont fontTitre("Segoe UI", 28, QFont::Bold);
    p.setFont(fontTitre);
    p.drawText(QRect(0, 20, W, headerH - 20),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "CENTRE DE FORMATION");

    // Sous-titre
    QFont fontSous("Segoe UI", 14);
    p.setFont(fontSous);
    p.drawText(QRect(0, headerH / 2 + 10, W, headerH / 2),
               Qt::AlignHCenter | Qt::AlignTop,
               "Session de Repassage — Septembre 2026");

    // ════════════════════════════════════════════════════════════════
    //  TITRE DU DOCUMENT
    // ════════════════════════════════════════════════════════════════
    int y = headerH + 60;

    p.setPen(couleur("#cdd6f4"));
    QFont fontDoc("Segoe UI", 22, QFont::Bold);
    p.setFont(fontDoc);
    p.drawText(QRect(0, y, W, 80),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "ATTESTATION D'INSCRIPTION");

    // Ligne décorative sous le titre
    y += 90;
    p.setPen(QPen(couleur("#89b4fa"), 3));
    p.drawLine(W / 4, y, 3 * W / 4, y);
    y += 30;

    // ════════════════════════════════════════════════════════════════
    //  CARTE STAGIAIRE
    // ════════════════════════════════════════════════════════════════
    // Fond de la carte
    int cardX = W / 10;
    int cardW = W - 2 * cardX;
    int cardH = 340;
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(couleur("#181825")));
    p.drawRoundedRect(cardX, y, cardW, cardH, 12, 12);

    // Bordure gauche colorée
    p.setBrush(QBrush(couleur("#89b4fa")));
    p.drawRoundedRect(cardX, y, 10, cardH, 4, 4);

    // Contenu de la carte
    auto dessinerLigne = [&](const QString &label, const QString &valeur, int &yPos) {
        // Label
        QFont fLabel("Segoe UI", 10);
        fLabel.setItalic(true);
        p.setFont(fLabel);
        p.setPen(couleur("#6c7086"));
        p.drawText(QRect(cardX + 30, yPos, cardW / 3, 36),
                   Qt::AlignVCenter | Qt::AlignLeft, label);

        // Valeur
        QFont fVal("Segoe UI", 11, QFont::Bold);
        p.setFont(fVal);
        p.setPen(couleur("#cdd6f4"));
        p.drawText(QRect(cardX + 30 + cardW / 3, yPos, cardW * 2 / 3 - 40, 36),
                   Qt::AlignVCenter | Qt::AlignLeft, valeur);

        // Séparateur
        yPos += 38;
        p.setPen(QPen(couleur("#313244"), 1));
        p.drawLine(cardX + 20, yPos - 2, cardX + cardW - 20, yPos - 2);
    };

    int cy = y + 20;
    dessinerLigne("ID Stagiaire",      QString::number(s.getId()),                  cy);
    dessinerLigne("Nom complet",       s.getNom() + " " + s.getPrenom(),            cy);
    dessinerLigne("Email",             s.getEmail(),                                cy);
    dessinerLigne("Téléphone",         s.getTelephone().isEmpty()
                                           ? "—" : s.getTelephone(),               cy);
    dessinerLigne("Date de naissance", s.getDateNaissance().toString("dd/MM/yyyy"), cy);
    dessinerLigne("Niveau",            s.getNiveau(),                               cy);
    dessinerLigne("Date d'inscription",s.getDateInscription().toString("dd/MM/yyyy"), cy);

    y += cardH + 50;

    // ════════════════════════════════════════════════════════════════
    //  BADGE NIVEAU
    // ════════════════════════════════════════════════════════════════
    QColor badgeCouleur;
    if      (s.getNiveau() == "Debutant")      badgeCouleur = QColor("#a6e3a1");
    else if (s.getNiveau() == "Intermediaire") badgeCouleur = QColor("#f9e2af");
    else                                        badgeCouleur = QColor("#f38ba8");

    int badgeW = 200, badgeH = 48;
    int badgeX = (W - badgeW) / 2;
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(badgeCouleur));
    p.drawRoundedRect(badgeX, y, badgeW, badgeH, 24, 24);

    p.setPen(couleur("#1e1e2e"));
    QFont fBadge("Segoe UI", 13, QFont::Bold);
    p.setFont(fBadge);
    p.drawText(QRect(badgeX, y, badgeW, badgeH),
               Qt::AlignCenter, "Niveau : " + s.getNiveau());

    y += badgeH + 60;

    // ════════════════════════════════════════════════════════════════
    //  TEXTE OFFICIEL
    // ════════════════════════════════════════════════════════════════
    p.setPen(couleur("#a6adc8"));
    QFont fTexte("Segoe UI", 10);
    p.setFont(fTexte);

    QString texte = QString(
        "Le Centre de Formation certifie que %1 %2 est dûment inscrit(e)\n"
        "à notre programme de formation de niveau %3,\n"
        "à compter du %4.\n\n"
        "La présente attestation est délivrée pour servir et valoir ce que de droit.")
        .arg(s.getNom(), s.getPrenom(), s.getNiveau(),
             s.getDateInscription().toString("dd MMMM yyyy"));

    p.drawText(QRect(W / 8, y, W * 3 / 4, 180),
               Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, texte);

    y += 200;

    // ════════════════════════════════════════════════════════════════
    //  PIED DE PAGE — date d'émission + signature
    // ════════════════════════════════════════════════════════════════
    // Ligne séparatrice
    p.setPen(QPen(couleur("#313244"), 2));
    p.drawLine(W / 10, H - 120, W * 9 / 10, H - 120);

    // Date d'émission
    p.setPen(couleur("#6c7086"));
    QFont fPied("Segoe UI", 9);
    p.setFont(fPied);
    p.drawText(QRect(W / 10, H - 100, W / 2, 40),
               Qt::AlignVCenter | Qt::AlignLeft,
               "Émis le : " + QDate::currentDate().toString("dd/MM/yyyy"));

    // Zone signature
    p.drawText(QRect(W / 2, H - 110, W * 2 / 5, 20),
               Qt::AlignRight | Qt::AlignTop, "Signature & Cachet :");

    p.setPen(QPen(couleur("#45475a"), 1));
    p.drawLine(W / 2 + 20, H - 70, W * 9 / 10, H - 70);

    p.end();
    return true;
}
