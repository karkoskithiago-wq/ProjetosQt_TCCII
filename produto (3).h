#ifndef DATABASEMANAGER_COPY_H
#define DATABASEMANAGER_COPY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QList>
#include <QDebug>
#include <QString>

class DatabaseManager
{
public:
    explicit DatabaseManager(const QString &databasePath);
    ~DatabaseManager();

    bool createTables();
    bool isDbOpen() const { return m_db.isOpen(); }

    // LOGIN
    bool authenticateUser(const QString &username, const QString &password);

    // PRODUTOS – usados em produto.cpp
    QList<QVariantList> getProducts();
    bool insertProduct(const QString &description, double price, int stock);
    bool updateProduct(const QString &id, const QString &description, double price, int stock);
    bool deleteProduct(const QString &id);

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_COPY_H
