#ifndef STAGIAIREDAO_H
#define STAGIAIREDAO_H

#include <QList>
#include <QDate>
#include <QString>
#include "stagiaire.h"

class StagiaireDAO
{
public:
    StagiaireDAO() = default;

    // CRUD de base
    bool ajouter(const Stagiaire &s);
    bool modifier(const Stagiaire &s);
    bool supprimer(int id);

    QList<Stagiaire> afficherTous();
    Stagiaire        afficherParId(int id);

    // Métier obligatoire : recherche / tri multicritères
    QList<Stagiaire> rechercherMulticritere(
        const QString &nom       = QString(),
        const QString &niveau    = QString(),
        int            idCours   = -1,
        const QString &triColonne   = "nom",
        bool           triAscendant = true);

    // Métier au choix : fiche de suivi individuel
    struct LigneSuivi {
        QString intituleCours;
        QString niveau;
        QDate   dateDebut;
        QDate   dateFin;
        QString statut;
        int     dureeHeures;
    };
    QList<LigneSuivi> suiviStagiaire(int idStagiaire);

    // Contrôle unicité email
    bool emailExiste(const QString &email, int idAExclure = -1);

    // Dernier message d'erreur SQL
    QString dernierErreur() const { return m_dernierErreur; }

private:
    QString m_dernierErreur;
};

#endif // STAGIAIREDAO_H
