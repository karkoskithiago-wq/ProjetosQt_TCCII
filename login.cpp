#include "login.h"
#include "ui_login.h"
#include "principal.h"
#include "databasemanager.h"
#include <QMessageBox>

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btnEntrar_clicked()
{
    QString usuario = ui->txtUsuario->text();
    QString senha   = ui->txtSenha->text();

    DatabaseManager dbManager("vendas.db");

    if (dbManager.authenticateUser(usuario, senha)) {
        this->hide();              // Esconde a tela de login
        Principal principal;       // Cria a tela principal
        principal.exec();          // Abre a tela principal (modal)
        this->close();             // Fecha o login ao sair da principal
    } else {
        QMessageBox::warning(this,
                             "Erro de Login",
                             "Usuário ou senha inválidos.");
    }
}
