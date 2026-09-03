#ifndef STAGIAIRE_H
#define STAGIAIRE_H

#include <QString>
#include <QDate>

class Stagiaire
{
public:
    Stagiaire();
    Stagiaire(int id, const QString &nom, const QString &prenom,
              const QString &email, const QString &telephone,
              const QDate &dateNaissance, const QString &niveau,
              const QDate &dateInscription = QDate::currentDate());

    // Getters
    int getId() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getEmail() const;
    QString getTelephone() const;
    QDate getDateNaissance() const;
    QString getNiveau() const;
    QDate getDateInscription() const;

    // Setters
    void setId(int id);
    void setNom(const QString &nom);
    void setPrenom(const QString &prenom);
    void setEmail(const QString &email);
    void setTelephone(const QString &telephone);
    void setDateNaissance(const QDate &date);
    void setNiveau(const QString &niveau);
    void setDateInscription(const QDate &date);

    // Validation de base (contrôles côté C++)
    bool estValide(QString &erreur) const;

private:
    int m_id;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_telephone;
    QDate m_dateNaissance;
    QString m_niveau;
    QDate m_dateInscription;
};

#endif // STAGIAIRE_H
