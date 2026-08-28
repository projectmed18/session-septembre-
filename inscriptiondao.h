#ifndef INSCRIPTIONDAO_H
#define INSCRIPTIONDAO_H

#include <QString>
#include <QList>
#include <QPair>
#include "inscription.h"

class InscriptionDAO
{
public:
    InscriptionDAO() = default;

    // ── CRUD ──────────────────────────────────────────────────────
    bool ajouter(const Inscription &i);
    bool modifierStatut(int idInscription, const QString &nouveauStatut);
    bool supprimer(int idInscription);

    // ── Lectures ──────────────────────────────────────────────────
    // Toutes les inscriptions (jointure Stagiaire + Cours)
    QList<Inscription> afficherTous();

    // Inscriptions d'un stagiaire donné
    QList<Inscription> parStagiaire(int idStagiaire);

    // Inscriptions d'un cours donné
    QList<Inscription> parCours(int idCours);

    // Recherche multicritère
    QList<Inscription> rechercherMulticritere(
        const QString &nomStagiaire   = QString(),
        const QString &intituleCours  = QString(),
        const QString &statut         = QString(),
        const QString &triColonne     = "date_inscription",
        bool           triAscendant   = false);

    // ── Vérifications ─────────────────────────────────────────────
    // Vrai si ce stagiaire est déjà inscrit à ce cours
    bool existeDeja(int idStagiaire, int idCours);

    // Nombre d'inscrits pour un cours
    int nombreInscrits(int idCours);

    // Places restantes pour un cours
    int placesRestantes(int idCours);

    // ── Stats pour le widget ───────────────────────────────────────
    // Retourne { idCours, nbInscrits } pour tous les cours
    QList<QPair<int,int>> statsParCours();

    // Dernier message d'erreur SQL
    QString dernierErreur() const { return m_dernierErreur; }

private:
    QList<Inscription> execQuery(const QString &sql,
                                 const QMap<QString,QVariant> &params = {});
    QString m_dernierErreur;
};

#endif // INSCRIPTIONDAO_H
