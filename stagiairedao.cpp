#include "stagiairedao.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool StagiaireDAO::ajouter(const Stagiaire &s)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Stagiaire (id_stagiaire, nom, prenom, email, telephone, "
        "date_naissance, niveau, date_inscription) "
        "VALUES (seq_stagiaire.NEXTVAL, :nom, :prenom, :email, :telephone, "
        ":date_naissance, :niveau, :date_inscription)");
    query.bindValue(":nom", s.getNom());
    query.bindValue(":prenom", s.getPrenom());
    query.bindValue(":email", s.getEmail());
    query.bindValue(":telephone", s.getTelephone());
    query.bindValue(":date_naissance", s.getDateNaissance());
    query.bindValue(":niveau", s.getNiveau());
    query.bindValue(":date_inscription", s.getDateInscription());

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur ajout stagiaire :" << m_dernierErreur;
        return false;
    }
    return true;
}

bool StagiaireDAO::modifier(const Stagiaire &s)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "UPDATE Stagiaire SET nom = :nom, prenom = :prenom, email = :email, "
        "telephone = :telephone, date_naissance = :date_naissance, "
        "niveau = :niveau WHERE id_stagiaire = :id");
    query.bindValue(":nom", s.getNom());
    query.bindValue(":prenom", s.getPrenom());
    query.bindValue(":email", s.getEmail());
    query.bindValue(":telephone", s.getTelephone());
    query.bindValue(":date_naissance", s.getDateNaissance());
    query.bindValue(":niveau", s.getNiveau());
    query.bindValue(":id", s.getId());

    if (!query.exec()) {
        qWarning() << "Erreur modification stagiaire :" << query.lastError().text();
        return false;
    }
    return true;
}

bool StagiaireDAO::supprimer(int id)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare("DELETE FROM Stagiaire WHERE id_stagiaire = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Erreur suppression stagiaire :" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Stagiaire> StagiaireDAO::afficherTous()
{
    QList<Stagiaire> liste;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_stagiaire, nom, prenom, email, telephone, date_naissance, "
        "niveau, date_inscription FROM Stagiaire ORDER BY nom");

    if (!query.exec()) {
        qWarning() << "Erreur chargement stagiaires :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        liste.append(Stagiaire(
            query.value("id_stagiaire").toInt(),
            query.value("nom").toString(),
            query.value("prenom").toString(),
            query.value("email").toString(),
            query.value("telephone").toString(),
            query.value("date_naissance").toDate(),
            query.value("niveau").toString(),
            query.value("date_inscription").toDate()));
    }
    return liste;
}

Stagiaire StagiaireDAO::afficherParId(int id)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_stagiaire, nom, prenom, email, telephone, date_naissance, "
        "niveau, date_inscription FROM Stagiaire WHERE id_stagiaire = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Stagiaire(
            query.value("id_stagiaire").toInt(),
            query.value("nom").toString(),
            query.value("prenom").toString(),
            query.value("email").toString(),
            query.value("telephone").toString(),
            query.value("date_naissance").toDate(),
            query.value("niveau").toString(),
            query.value("date_inscription").toDate());
    }
    return Stagiaire();
}

QList<Stagiaire> StagiaireDAO::rechercherMulticritere(
    const QString &nom, const QString &niveau, int idCours,
    const QString &triColonne, bool triAscendant)
{
    QList<Stagiaire> liste;

    QString sql =
        "SELECT DISTINCT s.id_stagiaire, s.nom, s.prenom, s.email, "
        "s.telephone, s.date_naissance, s.niveau, s.date_inscription "
        "FROM Stagiaire s ";

    if (idCours != -1) {
        sql += "JOIN Inscription i ON i.id_stagiaire = s.id_stagiaire ";
    }

    sql += "WHERE 1=1 ";
    if (!nom.isEmpty())
        sql += "AND (LOWER(s.nom) LIKE :nom OR LOWER(s.prenom) LIKE :nom) ";
    if (!niveau.isEmpty())
        sql += "AND s.niveau = :niveau ";
    if (idCours != -1)
        sql += "AND i.id_cours = :idCours ";

    // Whitelist stricte des colonnes de tri (jamais de concaténation de
    // valeur utilisateur directement dans l'ORDER BY)
    QStringList colonnesAutorisees = {"nom", "date_inscription", "niveau"};
    QString colonne = colonnesAutorisees.contains(triColonne) ? triColonne : "nom";
    sql += "ORDER BY s." + colonne + (triAscendant ? " ASC" : " DESC");

    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);

    if (!nom.isEmpty())
        query.bindValue(":nom", "%" + nom.toLower() + "%");
    if (!niveau.isEmpty())
        query.bindValue(":niveau", niveau);
    if (idCours != -1)
        query.bindValue(":idCours", idCours);

    if (!query.exec()) {
        qWarning() << "Erreur recherche stagiaires :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        liste.append(Stagiaire(
            query.value("id_stagiaire").toInt(),
            query.value("nom").toString(),
            query.value("prenom").toString(),
            query.value("email").toString(),
            query.value("telephone").toString(),
            query.value("date_naissance").toDate(),
            query.value("niveau").toString(),
            query.value("date_inscription").toDate()));
    }
    return liste;
}

bool StagiaireDAO::emailExiste(const QString &email, int idAExclure)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT COUNT(*) FROM Stagiaire WHERE email = :email "
        "AND id_stagiaire != :id");
    query.bindValue(":email", email);
    query.bindValue(":id", idAExclure);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

QList<StagiaireDAO::LigneSuivi> StagiaireDAO::suiviStagiaire(int idStagiaire)
{
    QList<LigneSuivi> liste;
    QSqlQuery query(DatabaseManager::instance().db());

    // Jointure Inscription → Cours pour récupérer tous les cours du stagiaire
    query.prepare(
        "SELECT c.intitule, c.niveau, c.date_debut, c.date_fin, "
        "       c.duree_heures, i.statut "
        "FROM Inscription i "
        "JOIN Cours c ON c.id_cours = i.id_cours "
        "WHERE i.id_stagiaire = :id "
        "ORDER BY c.date_debut");
    query.bindValue(":id", idStagiaire);

    if (!query.exec()) {
        qWarning() << "Erreur suivi stagiaire :" << query.lastError().text();
        return liste;
    }

    while (query.next()) {
        LigneSuivi ligne;
        ligne.intituleCours = query.value("intitule").toString();
        ligne.niveau        = query.value("niveau").toString();
        ligne.dateDebut     = query.value("date_debut").toDate();
        ligne.dateFin       = query.value("date_fin").toDate();
        ligne.dureeHeures   = query.value("duree_heures").toInt();
        ligne.statut        = query.value("statut").toString();
        liste.append(ligne);
    }
    return liste;
}
