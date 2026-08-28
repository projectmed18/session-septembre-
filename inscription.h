#ifndef INSCRIPTION_H
#define INSCRIPTION_H

#include <QString>
#include <QDate>

class Inscription
{
public:
    Inscription();
    Inscription(int id, int idStagiaire, int idCours,
                const QDate &dateInscription,
                const QString &statut,
                // dénormalisé pour l'affichage
                const QString &nomStagiaire   = QString(),
                const QString &prenomStagiaire= QString(),
                const QString &intituleCours  = QString(),
                const QString &niveauCours    = QString());

    // Getters
    int     getId()               const;
    int     getIdStagiaire()      const;
    int     getIdCours()          const;
    QDate   getDateInscription()  const;
    QString getStatut()           const;
    QString getNomStagiaire()     const;
    QString getPrenomStagiaire()  const;
    QString getIntituleCours()    const;
    QString getNiveauCours()      const;

    // Setters
    void setId(int id);
    void setIdStagiaire(int id);
    void setIdCours(int id);
    void setDateInscription(const QDate &d);
    void setStatut(const QString &s);

    // Validation
    bool estValide(QString &erreur) const;

    // Statuts disponibles
    static QStringList statutsDisponibles();

private:
    int     m_id;
    int     m_idStagiaire;
    int     m_idCours;
    QDate   m_dateInscription;
    QString m_statut;

    // Données jointes (lecture seule, non stockées)
    QString m_nomStagiaire;
    QString m_prenomStagiaire;
    QString m_intituleCours;
    QString m_niveauCours;
};

#endif // INSCRIPTION_H
