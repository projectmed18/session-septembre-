#include "cours.h"

Cours::Cours()
    : m_id(-1), m_dateDebut(QDate::currentDate()),
      m_dateFin(QDate::currentDate().addMonths(1)),
      m_dureeHeures(0), m_capaciteMax(0)
{}

Cours::Cours(int id, const QString &intitule, const QString &description,
             const QString &niveau, const QDate &dateDebut, const QDate &dateFin,
             int dureeHeures, int capaciteMax)
    : m_id(id), m_intitule(intitule), m_description(description),
      m_niveau(niveau), m_dateDebut(dateDebut), m_dateFin(dateFin),
      m_dureeHeures(dureeHeures), m_capaciteMax(capaciteMax)
{}

// --- Getters ---
int     Cours::getId()          const { return m_id; }
QString Cours::getIntitule()    const { return m_intitule; }
QString Cours::getDescription() const { return m_description; }
QString Cours::getNiveau()      const { return m_niveau; }
QDate   Cours::getDateDebut()   const { return m_dateDebut; }
QDate   Cours::getDateFin()     const { return m_dateFin; }
int     Cours::getDureeHeures() const { return m_dureeHeures; }
int     Cours::getCapaciteMax() const { return m_capaciteMax; }

// --- Setters ---
void Cours::setId(int id)                    { m_id = id; }
void Cours::setIntitule(const QString &v)    { m_intitule = v; }
void Cours::setDescription(const QString &v) { m_description = v; }
void Cours::setNiveau(const QString &v)      { m_niveau = v; }
void Cours::setDateDebut(const QDate &d)     { m_dateDebut = d; }
void Cours::setDateFin(const QDate &d)       { m_dateFin = d; }
void Cours::setDureeHeures(int h)            { m_dureeHeures = h; }
void Cours::setCapaciteMax(int cap)          { m_capaciteMax = cap; }

// --- Validation ---
bool Cours::estValide(QString &erreur) const
{
    if (m_intitule.trimmed().isEmpty()) {
        erreur = "L'intitulé du cours est obligatoire.";
        return false;
    }
    if (m_niveau.trimmed().isEmpty()) {
        erreur = "Le niveau est obligatoire.";
        return false;
    }
    if (!m_dateDebut.isValid()) {
        erreur = "La date de début est invalide.";
        return false;
    }
    if (!m_dateFin.isValid()) {
        erreur = "La date de fin est invalide.";
        return false;
    }
    if (m_dateFin < m_dateDebut) {
        erreur = "La date de fin doit être postérieure à la date de début.";
        return false;
    }
    if (m_dureeHeures <= 0) {
        erreur = "La durée doit être supérieure à 0 heure.";
        return false;
    }
    if (m_capaciteMax <= 0) {
        erreur = "La capacité maximale doit être supérieure à 0.";
        return false;
    }
    return true;
}
