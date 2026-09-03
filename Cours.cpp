#include "cours.h"

Cours::Cours()
    : m_id(-1), m_dureeHeures(0), m_dateDebut(QDate::currentDate()),
    m_dateFin(QDate::currentDate()), m_capaciteMax(0)
{
}

Cours::Cours(int id, const QString &intitule, const QString &description,
             int dureeHeures, const QString &niveau, const QDate &dateDebut,
             const QDate &dateFin, int capaciteMax,
             const QString &formateurReferent)
    : m_id(id), m_intitule(intitule), m_description(description),
    m_dureeHeures(dureeHeures), m_niveau(niveau), m_dateDebut(dateDebut),
    m_dateFin(dateFin), m_capaciteMax(capaciteMax),
    m_formateurReferent(formateurReferent)
{
}

// --- Getters ---
int Cours::getId() const { return m_id; }
QString Cours::getIntitule() const { return m_intitule; }
QString Cours::getDescription() const { return m_description; }
int Cours::getDureeHeures() const { return m_dureeHeures; }
QString Cours::getNiveau() const { return m_niveau; }
QDate Cours::getDateDebut() const { return m_dateDebut; }
QDate Cours::getDateFin() const { return m_dateFin; }
int Cours::getCapaciteMax() const { return m_capaciteMax; }
QString Cours::getFormateurReferent() const { return m_formateurReferent; }

// --- Setters ---
void Cours::setId(int id) { m_id = id; }
void Cours::setIntitule(const QString &intitule) { m_intitule = intitule; }
void Cours::setDescription(const QString &description) { m_description = description; }
void Cours::setDureeHeures(int duree) { m_dureeHeures = duree; }
void Cours::setNiveau(const QString &niveau) { m_niveau = niveau; }
void Cours::setDateDebut(const QDate &date) { m_dateDebut = date; }
void Cours::setDateFin(const QDate &date) { m_dateFin = date; }
void Cours::setCapaciteMax(int capacite) { m_capaciteMax = capacite; }
void Cours::setFormateurReferent(const QString &formateur) { m_formateurReferent = formateur; }

// --- Validation ---
bool Cours::estValide(QString &erreur) const
{
    if (m_intitule.trimmed().isEmpty()) {
        erreur = "L'intitulé du cours est obligatoire.";
        return false;
    }
    if (m_dureeHeures <= 0) {
        erreur = "La durée doit être supérieure à 0 heures.";
        return false;
    }
    if (m_niveau.trimmed().isEmpty()) {
        erreur = "Le niveau est obligatoire.";
        return false;
    }
    if (m_dateFin < m_dateDebut) {
        erreur = "La date de fin doit être postérieure à la date de début.";
        return false;
    }
    if (m_capaciteMax <= 0) {
        erreur = "La capacité maximale doit être supérieure à 0.";
        return false;
    }
    return true;
}
