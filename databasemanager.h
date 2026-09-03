#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

/**
 * Singleton gérant la connexion unique à la base de données Oracle/QODBC.
 * Utilisation : DatabaseManager::instance().db()
 */
class DatabaseManager
{
public:
    // Retourne l'instance unique
    static DatabaseManager &instance();

    // Ouvre la connexion (à appeler une seule fois au démarrage, dans main.cpp)
    bool connect(const QString &connectionString,
                 const QString &user,
                 const QString &password);

    // Retourne l'objet QSqlDatabase actif (par référence — critique pour QSqlQuery)
    QSqlDatabase& db();

    // Vérifie si la connexion est ouverte
    bool isConnected() const;

    // Retourne le dernier message d'erreur
    QString getLastError() const;

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;

    // Non copiable
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
