#include "inscriptiondao.h"
#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QVariant>
#include <QDebug>

// ─── Helper : exécute un SELECT et retourne la liste d'Inscription ────────────
QList<Inscription> InscriptionDAO::execQuery(const QString &sql,
                                              const QMap<QString,QVariant> &params)
{
    QList<Inscription> liste;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(sql);
    for (auto it = params.cbegin(); it != params.cend(); ++it)
        query.bindValue(it.key(), it.value());

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "InscriptionDAO SQL error:" << m_dernierErreur;
        return liste;
    }
    while (query.next()) {
        liste.append(Inscription(
            query.value("id_inscription").toInt(),
            query.value("id_stagiaire").toInt(),
            query.value("id_cours").toInt(),
            query.value("date_inscription").toDate(),
            query.value("statut").toString(),
            query.value("nom_stagiaire").toString(),
            query.value("prenom_stagiaire").toString(),
            query.value("intitule_cours").toString(),
            query.value("niveau_cours").toString()));
    }
    return liste;
}

// ─── Base SELECT avec JOIN ────────────────────────────────────────────────────
static const QString BASE_SELECT =
    "SELECT i.id_inscription, i.id_stagiaire, i.id_cours, "
    "       i.date_inscription, i.statut, "
    "       s.nom AS nom_stagiaire, s.prenom AS prenom_stagiaire, "
    "       c.intitule AS intitule_cours, c.niveau AS niveau_cours "
    "FROM Inscription i "
    "JOIN Stagiaire s ON s.id_stagiaire = i.id_stagiaire "
    "JOIN Cours     c ON c.id_cours     = i.id_cours ";

// ─── CRUD ─────────────────────────────────────────────────────────────────────

bool InscriptionDAO::ajouter(const Inscription &insc)
{
    // Vérifier les places disponibles avant insertion
    if (placesRestantes(insc.getIdCours()) <= 0) {
        m_dernierErreur = "Ce cours est complet, impossible d'inscrire le stagiaire.";
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "INSERT INTO Inscription "
        "(id_inscription, id_stagiaire, id_cours, date_inscription, statut) "
        "VALUES (seq_inscription.NEXTVAL, :idStagiaire, :idCours, "
        ":dateInscription, :statut)");
    query.bindValue(":idStagiaire",     insc.getIdStagiaire());
    query.bindValue(":idCours",         insc.getIdCours());
    query.bindValue(":dateInscription", insc.getDateInscription());
    query.bindValue(":statut",          insc.getStatut());

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur ajout inscription:" << m_dernierErreur;
        return false;
    }
    return true;
}

bool InscriptionDAO::modifierStatut(int idInscription, const QString &nouveauStatut)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "UPDATE Inscription SET statut = :statut "
        "WHERE id_inscription = :id");
    query.bindValue(":statut", nouveauStatut);
    query.bindValue(":id",     idInscription);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur modification statut:" << m_dernierErreur;
        return false;
    }
    return true;
}

bool InscriptionDAO::supprimer(int idInscription)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare("DELETE FROM Inscription WHERE id_inscription = :id");
    query.bindValue(":id", idInscription);

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        qWarning() << "Erreur suppression inscription:" << m_dernierErreur;
        return false;
    }
    return true;
}

// ─── Lectures ─────────────────────────────────────────────────────────────────

QList<Inscription> InscriptionDAO::afficherTous()
{
    return execQuery(BASE_SELECT + "ORDER BY i.date_inscription DESC");
}

QList<Inscription> InscriptionDAO::parStagiaire(int idStagiaire)
{
    return execQuery(
        BASE_SELECT + "WHERE i.id_stagiaire = :idStagiaire "
                      "ORDER BY i.date_inscription DESC",
        {{":idStagiaire", idStagiaire}});
}

QList<Inscription> InscriptionDAO::parCours(int idCours)
{
    return execQuery(
        BASE_SELECT + "WHERE i.id_cours = :idCours "
                      "ORDER BY s.nom ASC",
        {{":idCours", idCours}});
}

QList<Inscription> InscriptionDAO::rechercherMulticritere(
    const QString &nomStagiaire, const QString &intituleCours,
    const QString &statut,
    const QString &triColonne, bool triAscendant)
{
    QString sql = BASE_SELECT + "WHERE 1=1 ";
    QMap<QString,QVariant> params;

    if (!nomStagiaire.isEmpty()) {
        sql += "AND (LOWER(s.nom) LIKE :nom OR LOWER(s.prenom) LIKE :nom) ";
        params[":nom"] = "%" + nomStagiaire.toLower() + "%";
    }
    if (!intituleCours.isEmpty()) {
        sql += "AND LOWER(c.intitule) LIKE :intitule ";
        params[":intitule"] = "%" + intituleCours.toLower() + "%";
    }
    if (!statut.isEmpty()) {
        sql += "AND i.statut = :statut ";
        params[":statut"] = statut;
    }

    // Colonnes de tri autorisées
    QStringList colsOk = {"date_inscription", "nom_stagiaire",
                          "intitule_cours",   "statut"};
    QString col = colsOk.contains(triColonne) ? triColonne : "date_inscription";

    // Oracle ne permet pas ORDER BY sur un alias de SELECT dans tous les cas ;
    // on mappe vers la vraie colonne
    QMap<QString,QString> colMap = {
        {"date_inscription", "i.date_inscription"},
        {"nom_stagiaire",    "s.nom"},
        {"intitule_cours",   "c.intitule"},
        {"statut",           "i.statut"}
    };
    sql += "ORDER BY " + colMap.value(col, "i.date_inscription")
         + (triAscendant ? " ASC" : " DESC");

    return execQuery(sql, params);
}

// ─── Vérifications ────────────────────────────────────────────────────────────

bool InscriptionDAO::existeDeja(int idStagiaire, int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT COUNT(*) FROM Inscription "
        "WHERE id_stagiaire = :idStagiaire AND id_cours = :idCours");
    query.bindValue(":idStagiaire", idStagiaire);
    query.bindValue(":idCours",     idCours);

    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

int InscriptionDAO::nombreInscrits(int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT COUNT(*) FROM Inscription WHERE id_cours = :idCours");
    query.bindValue(":idCours", idCours);

    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

int InscriptionDAO::placesRestantes(int idCours)
{
    QSqlQuery query(DatabaseManager::instance().db());
    // On ne compte que les inscriptions actives (En cours ou Termine)
    // Un statut Annule libère la place
    query.prepare(
        "SELECT c.capacite_max - COUNT(i.id_inscription) AS places "
        "FROM Cours c "
        "LEFT JOIN Inscription i ON i.id_cours = c.id_cours "
        "                       AND i.statut != 'Annule' "
        "WHERE c.id_cours = :idCours "
        "GROUP BY c.id_cours, c.capacite_max");
    query.bindValue(":idCours", idCours);

    if (query.exec() && query.next())
        return query.value("places").toInt();
    return 0;
}

// ─── Stats ────────────────────────────────────────────────────────────────────

QList<QPair<int,int>> InscriptionDAO::statsParCours()
{
    QList<QPair<int,int>> result;
    QSqlQuery query(DatabaseManager::instance().db());
    query.prepare(
        "SELECT id_cours, COUNT(*) AS nb "
        "FROM Inscription GROUP BY id_cours ORDER BY id_cours");

    if (!query.exec()) {
        m_dernierErreur = query.lastError().text();
        return result;
    }
    while (query.next())
        result.append({query.value("id_cours").toInt(),
                       query.value("nb").toInt()});
    return result;
}
