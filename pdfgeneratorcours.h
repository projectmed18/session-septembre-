#ifndef PDFGENERATORCOURS_H
#define PDFGENERATORCOURS_H

#include <QString>
#include <QList>
#include "coursdao.h"

class PdfGeneratorCours
{
public:
    // Génère un rapport PDF de tous les cours avec leur taux de remplissage
    static bool genererRapport(const QList<CoursDAO::InfoCapacite> &cours,
                               const QString &cheminFichier);
};

#endif // PDFGENERATORCOURS_H
