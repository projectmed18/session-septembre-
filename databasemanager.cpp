#include "databasemanager.h"
#include <QSqlError>
#include <QDebug>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager s_instance;
    return s_instance;
}

bool DatabaseManager::connect(const QString &connectionString,
                               const QString &user,
                               const QString &password)
{
    m_db = QSqlDatabase::addDatabase("QODBC", "gestion_stagiaire_conn");
    m_db.setDatabaseName(connectionString);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        qCritical() << "Erreur de connexion à la base de données :"
                    << m_db.lastError().text();
        return false;
    }

    qDebug() << "Connexion à la base de données réussie.";
    return true;
}

QSqlDatabase& DatabaseManager::db()
{
    return m_db;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}
