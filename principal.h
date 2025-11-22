#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QDialog>
#include "produto.h"
#include "databasemanager.h"   // <-- nome certo

namespace Ui {
class Principal;
}

class Principal : public QDialog
{
    Q_OBJECT

public:
    explicit Principal(QWidget *parent = nullptr);
    ~Principal();

private slots:
    void on_btnProduto_clicked();
    void on_btnSair_clicked();

private:
    Ui::Principal *ui;
    DatabaseManager dbManager = DatabaseManager("vendas.db");
};

#endif // PRINCIPAL_H

