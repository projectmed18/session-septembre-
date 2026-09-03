#ifndef COURS_H
#define COURS_H

#include <QString>
#include <QDate>

class Cours
{
public:
    Cours();
    Cours(int id, const QString &intitule, const QString &description,
          int dureeHeures, const QString &niveau, const QDate &dateDebut,
          const QDate &dateFin, int capaciteMax,
          const QString &formateurReferent = QString());

    // Getters
    int getId() const;
    QString getIntitule() const;
    QString getDescription() const;
    int getDureeHeures() const;
    QString getNiveau() const;
    QDate getDateDebut() const;
    QDate getDateFin() const;
    int getCapaciteMax() const;
    QString getFormateurReferent() const;

    // Setters
    void setId(int id);
    void setIntitule(const QString &intitule);
    void setDescription(const QString &description);
    void setDureeHeures(int duree);
    void setNiveau(const QString &niveau);
    void setDateDebut(const QDate &date);
    void setDateFin(const QDate &date);
    void setCapaciteMax(int capacite);
    void setFormateurReferent(const QString &formateur);

    // Validation de base (contrôles côté C++)
    bool estValide(QString &erreur) const;

private:
    int m_id;
    QString m_intitule;
    QString m_description;
    int m_dureeHeures;
    QString m_niveau;
    QDate m_dateDebut;
    QDate m_dateFin;
    int m_capaciteMax;
    QString m_formateurReferent; // champ texte simple (pas de module Formateurs traité)
};

#endif // COURS_H
