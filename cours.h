#ifndef COURS_H
#define COURS_H

#include <QString>
#include <QDate>

class Cours
{
public:
    Cours();
    Cours(int id, const QString &intitule, const QString &description,
          const QString &niveau, const QDate &dateDebut, const QDate &dateFin,
          int dureeHeures, int capaciteMax);

    // Getters
    int     getId()          const;
    QString getIntitule()    const;
    QString getDescription() const;
    QString getNiveau()      const;
    QDate   getDateDebut()   const;
    QDate   getDateFin()     const;
    int     getDureeHeures() const;
    int     getCapaciteMax() const;

    // Setters
    void setId(int id);
    void setIntitule(const QString &v);
    void setDescription(const QString &v);
    void setNiveau(const QString &v);
    void setDateDebut(const QDate &d);
    void setDateFin(const QDate &d);
    void setDureeHeures(int h);
    void setCapaciteMax(int cap);

    // Validation
    bool estValide(QString &erreur) const;

private:
    int     m_id;
    QString m_intitule;
    QString m_description;
    QString m_niveau;
    QDate   m_dateDebut;
    QDate   m_dateFin;
    int     m_dureeHeures;
    int     m_capaciteMax;
};

#endif // COURS_H
