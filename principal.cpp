#include "principal.h"
#include "ui_principal.h"

Principal::Principal(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Principal)
{
    ui->setupUi(this);
}

Principal::~Principal()
{
    delete ui;
}

void Principal::on_btnProduto_clicked()
{
    Produto telaProduto(&dbManager);   // <--- CORRETO!!!
    telaProduto.exec();                // Abre MODAL
}

void Principal::on_btnSair_clicked()
{
    this->close();          // Fecha a tela principal
}
