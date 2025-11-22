#include "databasemanager.h"

DatabaseManager::DatabaseManager(const QString &databasePath)
{
    // Cria conexão padrão com SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(databasePath);

    if (!m_db.open()) {
        qCritical() << "Erro ao abrir o banco de dados:"
                    << m_db.lastError().text();
        return;
    }

    // Cria tabelas, se ainda não existirem
    createTables();
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
}

bool DatabaseManager::createTables()
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);

    // -------- TABELA DE USUÁRIOS (LOGIN) --------
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            " id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " username TEXT UNIQUE NOT NULL,"
            " password TEXT NOT NULL)"
            )) {
        qCritical() << "Erro ao criar tabela users:"
                    << query.lastError().text();
        return false;
    }

    // Garante usuário padrão admin / 1234
    QSqlQuery countQuery(m_db);
    if (countQuery.exec("SELECT COUNT(*) FROM users") &&
        countQuery.next() &&
        countQuery.value(0).toInt() == 0)
    {
        QSqlQuery insertUser(m_db);
        insertUser.prepare(
            "INSERT INTO users (username, password)"
            " VALUES ('admin', '1234')");
        if (!insertUser.exec()) {
            qWarning() << "Falha ao inserir usuário padrão:"
                       << insertUser.lastError().text();
        }
    }

    // -------- TABELA DE PRODUTOS --------
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS produtos ("
            " codigo TEXT PRIMARY KEY,"
            " descricao TEXT NOT NULL,"
            " valor REAL NOT NULL,"
            " estoque INTEGER NOT NULL,"
            " estoque_minimo INTEGER NOT NULL)"
            )) {
        qCritical() << "Erro ao criar tabela produtos:"
                    << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::authenticateUser(const QString &username,
                                       const QString &password)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);
    query.prepare("SELECT password FROM users WHERE username = :u");
    query.bindValue(":u", username);

    if (!query.exec()) {
        qWarning() << "Erro em authenticateUser:"
                   << query.lastError().text();
        return false;
    }

    if (query.next()) {
        const QString storedPass = query.value(0).toString();
        return (storedPass == password);
    }

    return false;
}

bool DatabaseManager::addProduct(const QString &codigo,
                                 const QString &descricao,
                                 double valor,
                                 int estoque,
                                 int estoqueMinimo)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO produtos (codigo, descricao, valor, estoque, estoque_minimo) "
        "VALUES (:c, :d, :v, :e, :em)");

    query.bindValue(":c",  codigo);
    query.bindValue(":d",  descricao);
    query.bindValue(":v",  valor);
    query.bindValue(":e",  estoque);
    query.bindValue(":em", estoqueMinimo);

    if (!query.exec()) {
        qWarning() << "Erro em addProduct:"
                   << query.lastError().text();
        return false;
    }

    return true;
}

QList<QVariantList> DatabaseManager::getAllProducts()
{
    QList<QVariantList> list;

    if (!m_db.isOpen())
        return list;

    QSqlQuery query(m_db);
    if (!query.exec(
            "SELECT codigo, descricao, valor, estoque, estoque_minimo "
            "FROM produtos ORDER BY descricao")) {
        qWarning() << "Erro em getAllProducts:"
                   << query.lastError().text();
        return list;
    }

    while (query.next()) {
        QVariantList row;
        row << query.value(0)  // codigo
            << query.value(1)  // descricao
            << query.value(2)  // valor
            << query.value(3)  // estoque
            << query.value(4); // estoque_minimo
        list << row;
    }

    return list;
}

bool DatabaseManager::deleteProduct(const QString &codigo)
{
    if (!m_db.isOpen())
        return false;

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM produtos WHERE codigo = :c");
    query.bindValue(":c", codigo);

    if (!query.exec()) {
        qWarning() << "Erro em deleteProduct:"
                   << query.lastError().text();
        return false;
    }

    // query.numRowsAffected() pode ser -1 em alguns drivers,
    // então consideramos sucesso se o exec() deu certo.
    return true;
}
