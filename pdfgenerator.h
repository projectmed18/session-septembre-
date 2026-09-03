#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <QString>
#include <QList>
#include "Cours.h"

class PdfGenerator
{
public:
    // Génère une Fiche de Cours PDF pour un cours sélectionné
    static bool genererFicheCours(const Cours &c, const QString &cheminFichier);

    // Génère un catalogue complet de tous les cours au format PDF
    static bool genererCatalogue(const QList<Cours> &liste, const QString &cheminFichier);
};

#endif // PDFGENERATOR_H
