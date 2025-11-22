#ifndef PRODUTO_H
#define PRODUTO_H

#include <QDialog>
#include <QString>
#include "databasemanager.h"

namespace Ui {
class Produto;
}

class Produto : public QDialog
{
    Q_OBJECT

public:
    explicit Produto(DatabaseManager *dbManager,
                     QWidget *parent = nullptr);
    ~Produto();

private slots:
    void on_btnNovo_clicked();
    void on_btnSalvar_clicked();
    void on_btnCancelar_clicked();
    void on_btnDeletar_clicked();
    void on_tabelaProdutos_cellClicked(int row, int column);

private:
    Ui::Produto *ui;
    DatabaseManager *db;
    QString currentProductId;

    void populateTable();
    void clearFields();
    void setFieldStatus(bool status);
};

#endif // PRODUTO_H
