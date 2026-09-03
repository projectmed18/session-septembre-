#include "coursdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// Retourne tous les cours avec leur taux de remplissage
QList<CoursDAO::InfoCapacite> CoursDAO::capaciteTousCours()
{
    QList<InfoCapacite> liste;
    QSqlQuery query(DatabaseManager::instance().db());

    // Jointure LEFT JOIN pour compter les inscrits par cours
    query.prepare(
        "SELECT c.id_cours, c.intitule, c.niveau, c.date_debut, c.date_fin, "
        "       c.capacite_max, "
        "       COUNT(i.id_inscription) AS nb_inscrits "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "GROUP BY c.id_cours, c.intitule, c.niveau, c.date_debut, "
        "         c.date_fin, c.capacite_max "
        "ORDER BY c.date_debut");

    if (!query.exec()) {
        qWarning() << "Erreur capacite cours :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        InfoCapacite info;
        info.idCours          = query.value("id_cours").toInt();
        info.intitule         = query.value("intitule").toString();
        info.niveau           = query.value("niveau").toString();
        info.dateDebut        = query.value("date_debut").toDate();
        info.dateFin          = query.value("date_fin").toDate();
        info.capaciteMax      = query.value("capacite_max").toInt();
        info.nbInscrits       = query.value("nb_inscrits").toInt();
        info.placesRestantes  = info.capaciteMax - info.nbInscrits;
        liste.append(info);
    }
    return liste;
}

// Vérifie si un cours a encore des places disponibles
bool CoursDAO::placesDisponibles(int idCours)
{
    return placesRestantes(idCours) > 0;
}

// Retourne le nombre de places restantes pour un cours donné
int CoursDAO::placesRestantes(int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.capacite_max - COUNT(i.id_inscription) AS places "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "WHERE c.id_cours = :id "
        "GROUP BY c.id_cours, c.capacite_max");
    query.bindValue(":id", idCours);

    if (query.exec() && query.next())
        return query.value("places").toInt();

    return 0;
}

// Inscrire un stagiaire à un cours tout en bloquant l'inscription si le cours est complet
bool CoursDAO::inscrireStagiaire(int idStagiaire, int idCours)
{
    if (!placesDisponibles(idCours)) {
        m_dernierErreur = QString("Inscription impossible : le cours (ID %1) est complet (0 place restante).").arg(idCours);
        qWarning() << m_dernierErreur;
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Inscription (id_inscription, id_stagiaire, id_cours, statut) "
        "VALUES (seq_inscription.NEXTVAL, :idStagiaire, :idCours, 'En cours')");
    query.bindValue(":idStagiaire", idStagiaire);
    query.bindValue(":idCours", idCours);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur lors de l'inscription :" << m_dernierErreur;
        return false;
    }

    return true;
}
