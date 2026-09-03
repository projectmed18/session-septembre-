#ifndef COURSDAO_H
#define COURSDAO_H

#include <QString>
#include <QList>
#include <QDate>

class CoursDAO
{
public:
    CoursDAO() = default;

    // Structure représentant la capacité d'un cours
    struct InfoCapacite {
        int     idCours;
        QString intitule;
        QString niveau;
        QDate   dateDebut;
        QDate   dateFin;
        int     capaciteMax;
        int     nbInscrits;
        int     placesRestantes;  // capaciteMax - nbInscrits
    };

    // Retourne la capacité de tous les cours avec nombre d'inscrits
    QList<InfoCapacite> capaciteTousCours();

    // Vérifie si un cours spécifique a encore de la place
    // Retourne true si inscription possible
    bool placesDisponibles(int idCours);

    // Retourne le nombre de places restantes pour un cours
    int placesRestantes(int idCours);

    // Tente d'inscrire un stagiaire à un cours ; bloque si le cours est plein
    bool inscrireStagiaire(int idStagiaire, int idCours);

    // Message d'erreur SQL ou métier
    QString dernierErreur() const { return m_dernierErreur; }

private:
    QString m_dernierErreur;
};

#endif // COURSDAO_H
