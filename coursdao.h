#ifndef COURSDAO_H
#define COURSDAO_H

#include <QString>
#include <QList>
#include <QDate>
#include "cours.h"

class CoursDAO
{
public:
    CoursDAO() = default;

    // ── CRUD ──────────────────────────────────────────────────────
    bool ajouter(const Cours &c);
    bool modifier(const Cours &c);
    bool supprimer(int id);

    QList<Cours> afficherTous();
    Cours        afficherParId(int id);

    QList<Cours> rechercherMulticritere(
        const QString &intitule     = QString(),
        const QString &niveau       = QString(),
        const QString &triColonne   = "intitule",
        bool           triAscendant = true);

    // ── Capacité ──────────────────────────────────────────────────
    struct InfoCapacite {
        int     idCours;
        QString intitule;
        QString niveau;
        QDate   dateDebut;
        QDate   dateFin;
        int     capaciteMax;
        int     nbInscrits;
        int     placesRestantes;
    };

    QList<InfoCapacite> capaciteTousCours();
    bool placesDisponibles(int idCours);
    int  placesRestantes(int idCours);
    bool inscrireStagiaire(int idStagiaire, int idCours);

    // ── Classement ────────────────────────────────────────────────
    struct InfoClassement {
        int     rang;
        int     idCours;
        QString intitule;
        QString niveau;
        int     capaciteMax;
        int     nbInscrits;
        double  tauxRemplissage;   // %
    };

    QList<InfoClassement> classementCours();

    // Dernier message d'erreur SQL
    QString dernierErreur() const { return m_dernierErreur; }

private:
    QString m_dernierErreur;
};

#endif // COURSDAO_H
