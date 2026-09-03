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
    if (QSqlDatabase::contains("gestion_cours_conn")) {
        m_db = QSqlDatabase::database("gestion_cours_conn");
    } else {
        m_db = QSqlDatabase::addDatabase("QODBC", "gestion_cours_conn");
    }

    // Stratégie 1 : Essai avec le DSN configuré
    m_db.setDatabaseName(connectionString);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (m_db.open()) {
        qDebug() << "Connexion à la base de données réussie via DSN :" << connectionString;
        m_lastError.clear();
        return true;
    }

    m_lastError = m_db.lastError().text();
    qWarning() << "Échec connexion via DSN" << connectionString << ":" << m_lastError;
    m_db.close();

    // Stratégie 2 : Connexion DSN-less avec 'Oracle in XE'
    QString dsnLess1 = QString("Driver={Oracle in XE};DBQ=XE;Uid=%1;Pwd=%2;").arg(user, password);
    m_db.setDatabaseName(dsnLess1);

    if (m_db.open()) {
        qDebug() << "Connexion à la base de données réussie via Driver Oracle in XE.";
        m_lastError.clear();
        return true;
    }

    m_lastError = m_db.lastError().text();
    qWarning() << "Échec connexion DSN-less Oracle in XE :" << m_lastError;
    m_db.close();

    // Stratégie 3 : Connexion DSN-less avec 'Oracle dans OraDB21Home1'
    QString dsnLess2 = QString("Driver={Oracle dans OraDB21Home1};DBQ=XE;Uid=%1;Pwd=%2;").arg(user, password);
    m_db.setDatabaseName(dsnLess2);

    if (m_db.open()) {
        qDebug() << "Connexion à la base de données réussie via Driver OraDB21Home1.";
        m_lastError.clear();
        return true;
    }

    m_lastError = m_db.lastError().text();
    qCritical() << "Toutes les tentatives de connexion ont échoué :" << m_lastError;
    return false;
}

QSqlDatabase& DatabaseManager::db()
{
    return m_db;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QString DatabaseManager::getLastError() const
{
    return m_lastError;
}
