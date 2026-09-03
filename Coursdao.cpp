#include "coursdao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool CoursDAO::ajouter(const Cours &c)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Cours (id_cours, intitule, description, duree_heures, "
        "niveau, date_debut, date_fin, capacite_max) "
        "VALUES (seq_cours.NEXTVAL, :intitule, :description, :duree, "
        ":niveau, :date_debut, :date_fin, :capacite)");
    query.bindValue(":intitule", c.getIntitule());
    query.bindValue(":description", c.getDescription());
    query.bindValue(":duree", c.getDureeHeures());
    query.bindValue(":niveau", c.getNiveau());
    query.bindValue(":date_debut", c.getDateDebut());
    query.bindValue(":date_fin", c.getDateFin());
    query.bindValue(":capacite", c.getCapaciteMax());

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
        "duree_heures = :duree, niveau = :niveau, date_debut = :date_debut, "
        "date_fin = :date_fin, capacite_max = :capacite "
        "WHERE id_cours = :id");
    query.bindValue(":intitule", c.getIntitule());
    query.bindValue(":description", c.getDescription());
    query.bindValue(":duree", c.getDureeHeures());
    query.bindValue(":niveau", c.getNiveau());
    query.bindValue(":date_debut", c.getDateDebut());
    query.bindValue(":date_fin", c.getDateFin());
    query.bindValue(":capacite", c.getCapaciteMax());
    query.bindValue(":id", c.getId());

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
        "SELECT id_cours, intitule, description, duree_heures, niveau, "
        "date_debut, date_fin, capacite_max FROM Cours ORDER BY date_debut");

    if (!query.exec()) {
        qWarning() << "Erreur chargement cours :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        liste.append(Cours(
            query.value("id_cours").toInt(),
            query.value("intitule").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("niveau").toString(),
            query.value("date_debut").toDate(),
            query.value("date_fin").toDate(),
            query.value("capacite_max").toInt()));
    }
    return liste;
}

Cours CoursDAO::afficherParId(int id)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_cours, intitule, description, duree_heures, niveau, "
        "date_debut, date_fin, capacite_max FROM Cours WHERE id_cours = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Cours(
            query.value("id_cours").toInt(),
            query.value("intitule").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("niveau").toString(),
            query.value("date_debut").toDate(),
            query.value("date_fin").toDate(),
            query.value("capacite_max").toInt());
    }
    return Cours();
}

QList<Cours> CoursDAO::rechercherMulticritere(
    const QString &intitule, const QString &niveau,
    const QDate &dateDebutMin, const QDate &dateDebutMax,
    const QString &triColonne, bool triAscendant)
{
    QList<Cours> liste;

    QString sql =
        "SELECT id_cours, intitule, description, duree_heures, niveau, "
        "date_debut, date_fin, capacite_max FROM Cours WHERE 1=1 ";

    if (!intitule.isEmpty())
        sql += "AND LOWER(intitule) LIKE :intitule ";
    if (!niveau.isEmpty())
        sql += "AND niveau = :niveau ";
    if (dateDebutMin.isValid())
        sql += "AND date_debut >= :dateMin ";
    if (dateDebutMax.isValid())
        sql += "AND date_debut <= :dateMax ";

    // Whitelist stricte des colonnes de tri (jamais de concaténation de
    // valeur utilisateur directement dans l'ORDER BY)
    QStringList colonnesAutorisees = {"date_debut", "duree_heures", "intitule"};
    QString colonne = colonnesAutorisees.contains(triColonne) ? triColonne : "date_debut";
    sql += "ORDER BY " + colonne + (triAscendant ? " ASC" : " DESC");

    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);

    if (!intitule.isEmpty())
        query.bindValue(":intitule", "%" + intitule.toLower() + "%");
    if (!niveau.isEmpty())
        query.bindValue(":niveau", niveau);
    if (dateDebutMin.isValid())
        query.bindValue(":dateMin", dateDebutMin);
    if (dateDebutMax.isValid())
        query.bindValue(":dateMax", dateDebutMax);

    if (!query.exec()) {
        qWarning() << "Erreur recherche cours :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        liste.append(Cours(
            query.value("id_cours").toInt(),
            query.value("intitule").toString(),
            query.value("description").toString(),
            query.value("duree_heures").toInt(),
            query.value("niveau").toString(),
            query.value("date_debut").toDate(),
            query.value("date_fin").toDate(),
            query.value("capacite_max").toInt()));
    }
    return liste;
}

int CoursDAO::nombreInscrits(int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT COUNT(*) FROM Inscription "
        "WHERE id_cours = :id AND statut != 'Annule'");
    query.bindValue(":id", idCours);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int CoursDAO::placesDisponibles(int idCours)
{
    Cours c = afficherParId(idCours);
    if (c.getId() == -1) return 0;
    return c.getCapaciteMax() - nombreInscrits(idCours);
}

QList<CoursDAO::CoursClassement> CoursDAO::coursLesPlusDemandes(int limite)
{
    QList<CoursClassement> liste;

    // Sous-requête pour compter les inscrits actifs par cours,
    // trié par nb_inscrits décroissant, limité aux N premiers (Oracle 12c+)
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.id_cours, c.intitule, c.capacite_max, "
        "  NVL((SELECT COUNT(*) FROM Inscription i "
        "       WHERE i.id_cours = c.id_cours AND i.statut != 'Annule'), 0) "
        "  AS nb_inscrits "
        "FROM Cours c "
        "ORDER BY nb_inscrits DESC "
        "FETCH FIRST :limite ROWS ONLY");
    query.bindValue(":limite", limite);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur classement cours :" << m_dernierErreur;
        return liste;
    }

    while (query.next()) {
        CoursClassement cc;
        cc.idCours      = query.value("id_cours").toInt();
        cc.intitule     = query.value("intitule").toString();
        cc.capaciteMax  = query.value("capacite_max").toInt();
        cc.nbInscrits   = query.value("nb_inscrits").toInt();
        cc.tauxRemplissage = (cc.capaciteMax > 0)
            ? (cc.nbInscrits * 100.0 / cc.capaciteMax)
            : 0.0;
        liste.append(cc);
    }
    return liste;
}

QList<CoursDAO::AlerteCours> CoursDAO::coursAvecAlerte(int seuilBientotComplet,
                                                       int seuilSousRempli)
{
    QList<AlerteCours> alertes;

    // Une seule requête avec sous-requête pour le compte d'inscrits actifs,
    // plutôt que d'appeler nombreInscrits() en boucle (évite le N+1).
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT c.id_cours, c.intitule, c.capacite_max, "
        "  (c.capacite_max - NVL((SELECT COUNT(*) FROM Inscription i "
        "      WHERE i.id_cours = c.id_cours AND i.statut != 'Annule'), 0)) "
        "  AS places_restantes "
        "FROM Cours c "
        "WHERE c.date_fin >= SYSDATE " // n'alerte que sur les cours pas encore terminés
        );

    if (!query.exec()) {
        qWarning() << "Erreur chargement alertes cours :" << query.lastError().text();
        return alertes;
    }

    while (query.next()) {
        int placesRestantes = query.value("places_restantes").toInt();
        int capaciteMax = query.value("capacite_max").toInt();

        QString type;
        if (placesRestantes <= 0) {
            type = "COMPLET";
        } else if (placesRestantes <= seuilBientotComplet) {
            type = "BIENTOT_COMPLET";
        } else if (capaciteMax > 0 &&
                   (capaciteMax - placesRestantes) <= seuilSousRempli) {
            // peu d'inscrits par rapport à la capacité -> sous-rempli
            type = "SOUS_REMPLI";
        } else {
            continue; // pas d'alerte pour ce cours
        }

        AlerteCours a;
        a.idCours = query.value("id_cours").toInt();
        a.intitule = query.value("intitule").toString();
        a.placesRestantes = placesRestantes;
        a.capaciteMax = capaciteMax;
        a.typeAlerte = type;
        alertes.append(a);
    }
    return alertes;
}
