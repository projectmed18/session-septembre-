#include "inscription.h"

Inscription::Inscription()
    : m_id(-1), m_idStagiaire(-1), m_idCours(-1),
      m_dateInscription(QDate::currentDate()),
      m_statut("En cours")
{}

Inscription::Inscription(int id, int idStagiaire, int idCours,
                         const QDate &dateInscription,
                         const QString &statut,
                         const QString &nomStagiaire,
                         const QString &prenomStagiaire,
                         const QString &intituleCours,
                         const QString &niveauCours)
    : m_id(id), m_idStagiaire(idStagiaire), m_idCours(idCours),
      m_dateInscription(dateInscription), m_statut(statut),
      m_nomStagiaire(nomStagiaire), m_prenomStagiaire(prenomStagiaire),
      m_intituleCours(intituleCours), m_niveauCours(niveauCours)
{}

// --- Getters ---
int     Inscription::getId()              const { return m_id; }
int     Inscription::getIdStagiaire()     const { return m_idStagiaire; }
int     Inscription::getIdCours()         const { return m_idCours; }
QDate   Inscription::getDateInscription() const { return m_dateInscription; }
QString Inscription::getStatut()          const { return m_statut; }
QString Inscription::getNomStagiaire()    const { return m_nomStagiaire; }
QString Inscription::getPrenomStagiaire() const { return m_prenomStagiaire; }
QString Inscription::getIntituleCours()   const { return m_intituleCours; }
QString Inscription::getNiveauCours()     const { return m_niveauCours; }

// --- Setters ---
void Inscription::setId(int id)                    { m_id = id; }
void Inscription::setIdStagiaire(int id)           { m_idStagiaire = id; }
void Inscription::setIdCours(int id)               { m_idCours = id; }
void Inscription::setDateInscription(const QDate &d){ m_dateInscription = d; }
void Inscription::setStatut(const QString &s)      { m_statut = s; }

// --- Statuts ---
QStringList Inscription::statutsDisponibles()
{
    // Seuls deux statuts gérables manuellement :
    // "En cours" = inscrit et actif
    // "Annule"   = retiré, libère la place
    // "Termine"  = géré automatiquement (date de fin dépassée)
    return {"En cours", "Annule"};
}

// --- Validation ---
bool Inscription::estValide(QString &erreur) const
{
    if (m_idStagiaire <= 0) {
        erreur = "Veuillez sélectionner un stagiaire.";
        return false;
    }
    if (m_idCours <= 0) {
        erreur = "Veuillez sélectionner un cours.";
        return false;
    }
    if (!m_dateInscription.isValid()) {
        erreur = "La date d'inscription est invalide.";
        return false;
    }
    if (!statutsDisponibles().contains(m_statut)) {
        erreur = "Le statut est invalide.";
        return false;
    }
    return true;
}
