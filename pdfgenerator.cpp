#include "pdfgenerator.h"

#include <QPdfWriter>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QRect>
#include <QDate>
#include <QDebug>
#include <QPageSize>
#include <QPageLayout>
#include <QMargins>

bool PdfGenerator::genererFicheCours(const Cours &c, const QString &cheminFichier)
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

    const int W = p.device()->width();
    const int H = p.device()->height();

    auto couleur = [](const QString &hex) { return QColor(hex); };

    // FOND
    p.fillRect(0, 0, W, H, couleur("#1e1e2e"));

    // BANDE D'EN-TÊTE
    int headerH = H / 7;
    p.fillRect(0, 0, W, headerH, couleur("#89b4fa"));

    // Titre principal
    p.setPen(couleur("#1e1e2e"));
    QFont fontTitre("Segoe UI", 26, QFont::Bold);
    p.setFont(fontTitre);
    p.drawText(QRect(0, 20, W, headerH - 20),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "CENTRE DE FORMATION");

    // Sous-titre
    QFont fontSous("Segoe UI", 14);
    p.setFont(fontSous);
    p.drawText(QRect(0, headerH / 2 + 10, W, headerH / 2),
               Qt::AlignHCenter | Qt::AlignTop,
               "Catalogue & Syllabi des Cours");

    // TITRE DU DOCUMENT
    int y = headerH + 50;

    p.setPen(couleur("#cdd6f4"));
    QFont fontDoc("Segoe UI", 20, QFont::Bold);
    p.setFont(fontDoc);
    p.drawText(QRect(0, y, W, 70),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "FICHE DESCRIPTIVE DU COURS");

    // Ligne décorative sous le titre
    y += 80;
    p.setPen(QPen(couleur("#89b4fa"), 3));
    p.drawLine(W / 4, y, 3 * W / 4, y);
    y += 30;

    // CARTE DU COURS
    int cardX = W / 10;
    int cardW = W - 2 * cardX;
    int cardH = 380;
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(couleur("#181825")));
    p.drawRoundedRect(cardX, y, cardW, cardH, 12, 12);

    // Bordure gauche colorée
    p.setBrush(QBrush(couleur("#89b4fa")));
    p.drawRoundedRect(cardX, y, 10, cardH, 4, 4);

    auto dessinerLigne = [&](const QString &label, const QString &valeur, int &yPos) {
        QFont fLabel("Segoe UI", 10);
        fLabel.setItalic(true);
        p.setFont(fLabel);
        p.setPen(couleur("#6c7086"));
        p.drawText(QRect(cardX + 30, yPos, cardW / 3, 36),
                   Qt::AlignVCenter | Qt::AlignLeft, label);

        QFont fVal("Segoe UI", 11, QFont::Bold);
        p.setFont(fVal);
        p.setPen(couleur("#cdd6f4"));
        p.drawText(QRect(cardX + 30 + cardW / 3, yPos, cardW * 2 / 3 - 40, 36),
                   Qt::AlignVCenter | Qt::AlignLeft, valeur);

        yPos += 38;
        p.setPen(QPen(couleur("#313244"), 1));
        p.drawLine(cardX + 20, yPos - 2, cardX + cardW - 20, yPos - 2);
    };

    int cy = y + 20;
    dessinerLigne("ID du Cours",         QString::number(c.getId()),                  cy);
    dessinerLigne("Intitulé",           c.getIntitule(),                             cy);
    dessinerLigne("Description",        c.getDescription().isEmpty() ? "—" : c.getDescription(), cy);
    dessinerLigne("Durée",              QString::number(c.getDureeHeures()) + " heures", cy);
    dessinerLigne("Niveau",             c.getNiveau(),                               cy);
    dessinerLigne("Date de Début",      c.getDateDebut().toString("dd/MM/yyyy"),     cy);
    dessinerLigne("Date de Fin",        c.getDateFin().toString("dd/MM/yyyy"),       cy);
    dessinerLigne("Capacité Maximale",  QString::number(c.getCapaciteMax()) + " personnes", cy);

    y += cardH + 40;

    // BADGE NIVEAU
    QColor badgeCouleur;
    QString niv = c.getNiveau();
    if      (niv == "Débutant" || niv == "Debutant")           badgeCouleur = QColor("#a6e3a1");
    else if (niv == "Intermédiaire" || niv == "Intermediaire") badgeCouleur = QColor("#f9e2af");
    else                                                       badgeCouleur = QColor("#f38ba8");

    int badgeW = 240, badgeH = 48;
    int badgeX = (W - badgeW) / 2;
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(badgeCouleur));
    p.drawRoundedRect(badgeX, y, badgeW, badgeH, 24, 24);

    p.setPen(couleur("#1e1e2e"));
    QFont fBadge("Segoe UI", 13, QFont::Bold);
    p.setFont(fBadge);
    p.drawText(QRect(badgeX, y, badgeW, badgeH),
               Qt::AlignCenter, "Niveau : " + c.getNiveau());

    y += badgeH + 50;

    // TEXTE DESCRIPTIF
    p.setPen(couleur("#a6adc8"));
    QFont fTexte("Segoe UI", 10);
    p.setFont(fTexte);

    QString texte = QString(
        "Ce cours s'inscrit dans le programme officiel de formation du Centre de Formation.\n"
        "Pour toute inscription ou demande de renseignements complémentaires,\n"
        "veuillez contacter la direction des études.");

    p.drawText(QRect(W / 8, y, W * 3 / 4, 120),
               Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, texte);

    // PIED DE PAGE
    p.setPen(QPen(couleur("#313244"), 2));
    p.drawLine(W / 10, H - 120, W * 9 / 10, H - 120);

    p.setPen(couleur("#6c7086"));
    QFont fPied("Segoe UI", 9);
    p.setFont(fPied);
    p.drawText(QRect(W / 10, H - 100, W / 2, 40),
               Qt::AlignVCenter | Qt::AlignLeft,
               "Émis le : " + QDate::currentDate().toString("dd/MM/yyyy"));

    p.drawText(QRect(W / 2, H - 110, W * 2 / 5, 20),
               Qt::AlignRight | Qt::AlignTop, "Signature & Cachet :");

    p.setPen(QPen(couleur("#45475a"), 1));
    p.drawLine(W / 2 + 20, H - 70, W * 9 / 10, H - 70);

    p.end();
    return true;
}

bool PdfGenerator::genererCatalogue(const QList<Cours> &liste, const QString &cheminFichier)
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

    const int W = p.device()->width();
    const int H = p.device()->height();
    auto couleur = [](const QString &hex) { return QColor(hex); };

    // FOND
    p.fillRect(0, 0, W, H, couleur("#1e1e2e"));

    // EN-TÊTE
    int headerH = H / 8;
    p.fillRect(0, 0, W, headerH, couleur("#89b4fa"));

    p.setPen(couleur("#1e1e2e"));
    QFont fontTitre("Segoe UI", 24, QFont::Bold);
    p.setFont(fontTitre);
    p.drawText(QRect(0, 15, W, headerH - 15),
               Qt::AlignHCenter | Qt::AlignVCenter,
               "CENTRE DE FORMATION — CATALOGUE DES COURS");

    int y = headerH + 40;

    // TABLEAU DES COURS
    int tableX = W / 20;
    int tableW = W - 2 * tableX;

    // En-tête du tableau
    p.fillRect(tableX, y, tableW, 40, couleur("#181825"));
    p.setPen(couleur("#89b4fa"));
    QFont fontHeaderTable("Segoe UI", 10, QFont::Bold);
    p.setFont(fontHeaderTable);

    int col1 = tableX + 10;
    int col2 = tableX + 180;
    int col3 = tableX + 320;
    int col4 = tableX + 440;
    int col5 = tableX + 580;

    p.drawText(QRect(col1, y, 160, 40), Qt::AlignVCenter | Qt::AlignLeft, "Intitulé");
    p.drawText(QRect(col2, y, 130, 40), Qt::AlignVCenter | Qt::AlignLeft, "Niveau");
    p.drawText(QRect(col3, y, 110, 40), Qt::AlignVCenter | Qt::AlignCenter, "Durée");
    p.drawText(QRect(col4, y, 130, 40), Qt::AlignVCenter | Qt::AlignCenter, "Période");
    p.drawText(QRect(col5, y, 100, 40), Qt::AlignVCenter | Qt::AlignCenter, "Capacité");

    y += 45;

    QFont fontRow("Segoe UI", 9);
    p.setFont(fontRow);

    for (int i = 0; i < liste.size(); ++i) {
        const Cours &c = liste[i];

        if (y + 40 > H - 100) {
            writer.newPage();
            p.fillRect(0, 0, W, H, couleur("#1e1e2e"));
            y = 40;
        }

        QColor rowBg = (i % 2 == 0) ? couleur("#252535") : couleur("#181825");
        p.fillRect(tableX, y, tableW, 36, rowBg);

        p.setPen(couleur("#cdd6f4"));
        QString intituleCourt = c.getIntitule();
        if (intituleCourt.length() > 22) intituleCourt = intituleCourt.left(20) + "...";

        p.drawText(QRect(col1, y, 160, 36), Qt::AlignVCenter | Qt::AlignLeft, intituleCourt);
        p.drawText(QRect(col2, y, 130, 36), Qt::AlignVCenter | Qt::AlignLeft, c.getNiveau());
        p.drawText(QRect(col3, y, 110, 36), Qt::AlignVCenter | Qt::AlignCenter, QString::number(c.getDureeHeures()) + " h");
        p.drawText(QRect(col4, y, 130, 36), Qt::AlignVCenter | Qt::AlignCenter,
                   c.getDateDebut().toString("dd/MM") + " - " + c.getDateFin().toString("dd/MM/yy"));
        p.drawText(QRect(col5, y, 100, 36), Qt::AlignVCenter | Qt::AlignCenter, QString::number(c.getCapaciteMax()));

        y += 40;
    }

    // PIED DE PAGE
    p.setPen(QPen(couleur("#313244"), 2));
    p.drawLine(W / 10, H - 80, W * 9 / 10, H - 80);

    p.setPen(couleur("#6c7086"));
    QFont fPied("Segoe UI", 9);
    p.setFont(fPied);
    p.drawText(QRect(W / 10, H - 70, W / 2, 30),
               Qt::AlignVCenter | Qt::AlignLeft,
               QString("Émis le : %1  •  Total : %2 cours").arg(QDate::currentDate().toString("dd/MM/yyyy")).arg(liste.size()));

    p.end();
    return true;
}
