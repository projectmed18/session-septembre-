#include "stagiaire.h"
#include <QRegularExpression>

Stagiaire::Stagiaire()
    : m_id(-1), m_dateNaissance(QDate::currentDate()),
    m_dateInscription(QDate::currentDate())
{
}

Stagiaire::Stagiaire(int id, const QString &nom, const QString &prenom,
                     const QString &email, const QString &telephone,
                     const QDate &dateNaissance, const QString &niveau,
                     const QDate &dateInscription)
    : m_id(id), m_nom(nom), m_prenom(prenom), m_email(email),
    m_telephone(telephone), m_dateNaissance(dateNaissance),
    m_niveau(niveau), m_dateInscription(dateInscription)
{
}

// --- Getters ---
int Stagiaire::getId() const { return m_id; }
QString Stagiaire::getNom() const { return m_nom; }
QString Stagiaire::getPrenom() const { return m_prenom; }
QString Stagiaire::getEmail() const { return m_email; }
QString Stagiaire::getTelephone() const { return m_telephone; }
QDate Stagiaire::getDateNaissance() const { return m_dateNaissance; }
QString Stagiaire::getNiveau() const { return m_niveau; }
QDate Stagiaire::getDateInscription() const { return m_dateInscription; }

// --- Setters ---
void Stagiaire::setId(int id) { m_id = id; }
void Stagiaire::setNom(const QString &nom) { m_nom = nom; }
void Stagiaire::setPrenom(const QString &prenom) { m_prenom = prenom; }
void Stagiaire::setEmail(const QString &email) { m_email = email; }
void Stagiaire::setTelephone(const QString &telephone) { m_telephone = telephone; }
void Stagiaire::setDateNaissance(const QDate &date) { m_dateNaissance = date; }
void Stagiaire::setNiveau(const QString &niveau) { m_niveau = niveau; }
void Stagiaire::setDateInscription(const QDate &date) { m_dateInscription = date; }

// --- Validation ---
bool Stagiaire::estValide(QString &erreur) const
{
    if (m_nom.trimmed().isEmpty()) {
        erreur = "Le nom est obligatoire.";
        return false;
    }
    if (m_prenom.trimmed().isEmpty()) {
        erreur = "Le prénom est obligatoire.";
        return false;
    }

    // Contrôle format email
    static const QRegularExpression regexEmail(
        R"(^[\w\.-]+@[\w\.-]+\.\w+$)");
    if (!regexEmail.match(m_email).hasMatch()) {
        erreur = "L'adresse email n'est pas valide.";
        return false;
    }

    // Contrôle format téléphone (8 chiffres, format tunisien par ex.)
    static const QRegularExpression regexTel(R"(^\d{8}$)");
    if (!m_telephone.isEmpty() && !regexTel.match(m_telephone).hasMatch()) {
        erreur = "Le numéro de téléphone doit contenir 8 chiffres.";
        return false;
    }

    // Contrôle cohérence date de naissance (pas dans le futur, âge raisonnable)
    if (m_dateNaissance > QDate::currentDate().addYears(-10)) {
        erreur = "Le stagiaire doit avoir au moins 10 ans.";
        return false;
    }

    if (m_niveau.trimmed().isEmpty()) {
        erreur = "Le niveau est obligatoire.";
        return false;
    }

    return true;
}
