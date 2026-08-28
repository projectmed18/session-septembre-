#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <QString>
#include "stagiaire.h"

class PdfGenerator
{
public:
    // Génère une attestation d'inscription PDF pour un stagiaire
    // Retourne true si le fichier a été créé avec succès
    static bool genererAttestation(const Stagiaire &s, const QString &cheminFichier);
};

#endif // PDFGENERATOR_H
