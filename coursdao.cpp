#include "coursdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ─── Helper : lire un Cours depuis une QSqlQuery positionnée ─────────────────
static Cours coursDepuisQuery(const QSqlQuery &q)
{
    return Cours(
        q.value("id_cours").toInt(),
        q.value("intitule").toString(),
        q.value("description").toString(),
        q.value("niveau").toString(),
        q.value("date_debut").toDate(),
        q.value("date_fin").toDate(),
        q.value("duree_heures").toInt(),
        q.value("capacite_max").toInt());
}

// ─── CRUD ─────────────────────────────────────────────────────────────────────

bool CoursDAO::ajouter(const Cours &c)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Cours (id_cours, intitule, description, niveau, "
        "date_debut, date_fin, duree_heures, capacite_max) "
        "VALUES (seq_cours.NEXTVAL, :intitule, :description, :niveau, "
        ":date_debut, :date_fin, :duree_heures, :capacite_max)");
    query.bindValue(":intitule",     c.getIntitule());
    query.bindValue(":description",  c.getDescription());
    query.bindValue(":niveau",       c.getNiveau());
    query.bindValue(":date_debut",   c.getDateDebut());
    query.bindValue(":date_fin",     c.getDateFin());
    query.bindValue(":duree_heures", c.getDureeHeures());
    query.bindValue(":capacite_max", c.getCapaciteMax());

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur ajout cours :" << m_dernierErreur;
        return false;
    }
    return true;
}

bool CoursDAO::modifier(const Cours &c)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "UPDATE Cours SET intitule = :intitule, description = :description, "
        "niveau = :niveau, date_debut = :date_debut, date_fin = :date_fin, "
        "duree_heures = :duree_heures, capacite_max = :capacite_max "
        "WHERE id_cours = :id");
    query.bindValue(":intitule",     c.getIntitule());
    query.bindValue(":description",  c.getDescription());
    query.bindValue(":niveau",       c.getNiveau());
    query.bindValue(":date_debut",   c.getDateDebut());
    query.bindValue(":date_fin",     c.getDateFin());
    query.bindValue(":duree_heures", c.getDureeHeures());
    query.bindValue(":capacite_max", c.getCapaciteMax());
    query.bindValue(":id",           c.getId());

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur modification cours :" << m_dernierErreur;
        return false;
    }
    return true;
}

bool CoursDAO::supprimer(int id)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare("DELETE FROM Cours WHERE id_cours = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur suppression cours :" << m_dernierErreur;
        return false;
    }
    return true;
}

QList<Cours> CoursDAO::afficherTous()
{
    QList<Cours> liste;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_cours, intitule, description, niveau, date_debut, date_fin, "
        "duree_heures, capacite_max FROM Cours ORDER BY intitule");

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur chargement cours :" << m_dernierErreur;
        return liste;
    }
    while (query.next())
        liste.append(coursDepuisQuery(query));
    return liste;
}

Cours CoursDAO::afficherParId(int id)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_cours, intitule, description, niveau, date_debut, date_fin, "
        "duree_heures, capacite_max FROM Cours WHERE id_cours = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next())
        return coursDepuisQuery(query);
    return Cours();
}

QList<Cours> CoursDAO::rechercherMulticritere(
    const QString &intitule, const QString &niveau,
    const QString &triColonne, bool triAscendant)
{
    QList<Cours> liste;

    QString sql =
        "SELECT id_cours, intitule, description, niveau, date_debut, date_fin, "
        "duree_heures, capacite_max FROM Cours WHERE 1=1 ";

    if (!intitule.isEmpty())
        sql += "AND (LOWER(intitule) LIKE :intitule "
               "     OR LOWER(description) LIKE :intitule) ";
    if (!niveau.isEmpty())
        sql += "AND niveau = :niveau ";

    QStringList colonnesAutorisees = {"intitule", "niveau", "date_debut", "duree_heures"};
    QString col = colonnesAutorisees.contains(triColonne) ? triColonne : "intitule";
    sql += "ORDER BY " + col + (triAscendant ? " ASC" : " DESC");

    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);
    if (!intitule.isEmpty())
        query.bindValue(":intitule", "%" + intitule.toLower() + "%");
    if (!niveau.isEmpty())
        query.bindValue(":niveau", niveau);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur recherche cours :" << m_dernierErreur;
        return liste;
    }
    while (query.next())
        liste.append(coursDepuisQuery(query));
    return liste;
}

// ─── Métiers ──────────────────────────────────────────────────────────────────

QList<CoursDAO::InfoCapacite> CoursDAO::capaciteTousCours()
{
    QList<InfoCapacite> liste;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.id_cours, c.intitule, c.niveau, c.date_debut, c.date_fin, "
        "       c.capacite_max, COUNT(i.id_inscription) AS nb_inscrits "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "                       AND i.statut != 'Annule' "
        "GROUP BY c.id_cours, c.intitule, c.niveau, c.date_debut, "
        "         c.date_fin, c.capacite_max "
        "ORDER BY c.date_debut");

    if (!query.exec()) {
        qWarning() << "Erreur capacite cours :" << query.lastError().text();
        return liste;
    }
    while (query.next()) {
        InfoCapacite info;
        info.idCours         = query.value("id_cours").toInt();
        info.intitule        = query.value("intitule").toString();
        info.niveau          = query.value("niveau").toString();
        info.dateDebut       = query.value("date_debut").toDate();
        info.dateFin         = query.value("date_fin").toDate();
        info.capaciteMax     = query.value("capacite_max").toInt();
        info.nbInscrits      = query.value("nb_inscrits").toInt();
        info.placesRestantes = info.capaciteMax - info.nbInscrits;
        liste.append(info);
    }
    return liste;
}

int CoursDAO::placesRestantes(int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.capacite_max - COUNT(i.id_inscription) AS places "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "                       AND i.statut != 'Annule' "
        "WHERE c.id_cours = :id "
        "GROUP BY c.id_cours, c.capacite_max");
    query.bindValue(":id", idCours);

    if (query.exec() && query.next())
        return query.value("places").toInt();
    return 0;
}

bool CoursDAO::placesDisponibles(int idCours)
{
    return placesRestantes(idCours) > 0;
}

bool CoursDAO::inscrireStagiaire(int idStagiaire, int idCours)
{
    if (!placesDisponibles(idCours)) {
        m_dernierErreur = QString("Cours ID %1 complet.").arg(idCours);
        return false;
    }
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Inscription (id_inscription, id_stagiaire, id_cours, statut) "
        "VALUES (seq_inscription.NEXTVAL, :idStagiaire, :idCours, 'En cours')");
    query.bindValue(":idStagiaire", idStagiaire);
    query.bindValue(":idCours",     idCours);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur inscription :" << m_dernierErreur;
        return false;
    }
    return true;
}

QList<CoursDAO::InfoClassement> CoursDAO::classementCours()
{
    QList<InfoClassement> liste;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.id_cours, c.intitule, c.niveau, c.capacite_max, "
        "       COUNT(i.id_inscription) AS nb_inscrits "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "                       AND i.statut != 'Annule' "
        "GROUP BY c.id_cours, c.intitule, c.niveau, c.capacite_max "
        "ORDER BY nb_inscrits DESC");

    if (!query.exec()) {
        qWarning() << "Erreur classement cours :" << query.lastError().text();
        return liste;
    }
    int rang = 1;
    while (query.next()) {
        InfoClassement info;
        info.rang        = rang++;
        info.idCours     = query.value("id_cours").toInt();
        info.intitule    = query.value("intitule").toString();
        info.niveau      = query.value("niveau").toString();
        info.capaciteMax = query.value("capacite_max").toInt();
        info.nbInscrits  = query.value("nb_inscrits").toInt();
        info.tauxRemplissage = (info.capaciteMax > 0)
            ? (info.nbInscrits * 100.0 / info.capaciteMax) : 0.0;
        liste.append(info);
    }
    return liste;
}
