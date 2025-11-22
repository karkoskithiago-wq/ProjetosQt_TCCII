#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QList>
#include <QVariant>

class DatabaseManager
{
public:
    // Construtor: recebe o caminho do arquivo .db
    explicit DatabaseManager(const QString &databasePath);
    ~DatabaseManager();

    bool isDbOpen() const { return m_db.isOpen(); }

    // Criação das tabelas
    bool createTables();

    // -------- LOGIN --------
    bool authenticateUser(const QString &username,
                          const QString &password);

    // -------- PRODUTOS --------
    bool addProduct(const QString &codigo,
                    const QString &descricao,
                    double valor,
                    int estoque,
                    int estoqueMinimo);

    // Retorna todos os produtos como lista de linhas (QVariantList)
    QList<QVariantList> getAllProducts();

    // Exclui um produto pelo código
    bool deleteProduct(const QString &codigo);

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
