#include "produto.h"
#include "ui_produto.h"

#include <QMessageBox>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>

Produto::Produto(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Produto)
    , db(dbManager)
{
    ui->setupUi(this);
    setWindowTitle("Cadastro de Produtos");

    currentProductId.clear();

    // Configura a tabela: 5 colunas
    ui->tabelaProdutos->setColumnCount(5);
    QStringList headers;
    headers << "Código"
            << "Descrição"
            << "Valor (R$)"
            << "Estoque"
            << "Estoque mínimo";
    ui->tabelaProdutos->setHorizontalHeaderLabels(headers);

    setFieldStatus(false);
    populateTable();
}

Produto::~Produto()
{
    delete ui;
}

// ---------------------------------------------------------------------
// Preenche a tabela com o que está no banco
// ---------------------------------------------------------------------
void Produto::populateTable()
{
    ui->tabelaProdutos->setRowCount(0);

    if (!db || !db->isDbOpen())
        return;

    const QList<QVariantList> produtos = db->getAllProducts();
    int row = 0;

    for (const QVariantList &p : produtos) {
        if (p.size() < 5)
            continue;

        ui->tabelaProdutos->insertRow(row);

        const QString codigo        = p.at(0).toString();
        const QString descricao     = p.at(1).toString();
        const double  valor         = p.at(2).toDouble();
        const int     estoque       = p.at(3).toInt();
        const int     estoqueMinimo = p.at(4).toInt();

        auto *itemCodigo  = new QTableWidgetItem(codigo);
        auto *itemDesc    = new QTableWidgetItem(descricao);
        auto *itemValor   = new QTableWidgetItem(
            QString("R$ %1").arg(valor, 0, 'f', 2));
        auto *itemEst     = new QTableWidgetItem(QString::number(estoque));
        auto *itemEstMin  = new QTableWidgetItem(QString::number(estoqueMinimo));

        ui->tabelaProdutos->setItem(row, 0, itemCodigo);
        ui->tabelaProdutos->setItem(row, 1, itemDesc);
        ui->tabelaProdutos->setItem(row, 2, itemValor);
        ui->tabelaProdutos->setItem(row, 3, itemEst);
        ui->tabelaProdutos->setItem(row, 4, itemEstMin);

        // Destaque amarelo se estoque <= estoque mínimo
        if (estoque <= estoqueMinimo && estoqueMinimo > 0) {
            QBrush brush(QColor(255, 255, 0));
            itemCodigo->setBackground(brush);
            itemDesc->setBackground(brush);
            itemValor->setBackground(brush);
            itemEst->setBackground(brush);
            itemEstMin->setBackground(brush);
        }

        ++row;
    }
}

// ---------------------------------------------------------------------
// Utilidades de UI
// ---------------------------------------------------------------------
void Produto::clearFields()
{
    ui->txtCodigo->clear();
    ui->txtDescricao->clear();
    ui->txtValor->clear();
    ui->txtEstoque->clear();
    ui->txtEstoqueMin->clear();
    currentProductId.clear();
}

void Produto::setFieldStatus(bool status)
{
    ui->txtDescricao->setEnabled(status);
    ui->txtValor->setEnabled(status);
    ui->txtEstoque->setEnabled(status);
    ui->txtEstoqueMin->setEnabled(status);

    // Código não é editado manualmente
    ui->txtCodigo->setEnabled(false);

    ui->btnSalvar->setEnabled(status);
    ui->btnCancelar->setEnabled(status);

    ui->btnNovo->setEnabled(!status);
    ui->btnDeletar->setEnabled(!status);
}

// ---------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------
void Produto::on_btnNovo_clicked()
{
    clearFields();
    setFieldStatus(true);
}

void Produto::on_btnSalvar_clicked()
{
    if (!db || !db->isDbOpen()) {
        QMessageBox::critical(this, "Erro",
                              "Banco de dados não disponível.");
        return;
    }

    QString descricao      = ui->txtDescricao->text().trimmed();
    QString valorStr       = ui->txtValor->text().trimmed();
    QString estoqueStr     = ui->txtEstoque->text().trimmed();
    QString estoqueMinStr  = ui->txtEstoqueMin->text().trimmed();

    if (descricao.isEmpty() ||
        valorStr.isEmpty() ||
        estoqueStr.isEmpty() ||
        estoqueMinStr.isEmpty())
    {
        QMessageBox::warning(this, "Atenção",
                             "Preencha todos os campos antes de salvar.");
        return;
    }

    bool okValor = false;
    bool okEst   = false;
    bool okEstMin= false;

    double valor  = valorStr.replace(",", ".").toDouble(&okValor);
    int estoque   = estoqueStr.toInt(&okEst);
    int estMinimo = estoqueMinStr.toInt(&okEstMin);

    if (!okValor || !okEst || !okEstMin ||
        valor <= 0 || estoque < 0 || estMinimo < 0)
    {
        QMessageBox::warning(this, "Atenção",
                             "Verifique os valores numéricos informados.");
        return;
    }

    // Gera código automaticamente (timestamp simples)
    const QString codigo = QString::number(QDateTime::currentSecsSinceEpoch());

    bool sucesso = db->addProduct(codigo, descricao, valor,
                                  estoque, estMinimo);

    if (sucesso) {
        QMessageBox::information(this, "Sucesso",
                                 "Produto salvo com sucesso!");
        clearFields();
        setFieldStatus(false);
        populateTable();
    } else {
        QMessageBox::critical(this, "Erro",
                              "Não foi possível salvar o produto.\n"
                              "Talvez o código já exista.");
    }
}

void Produto::on_btnCancelar_clicked()
{
    clearFields();
    setFieldStatus(false);
}

void Produto::on_btnDeletar_clicked()
{
    if (!db || !db->isDbOpen())
        return;

    if (currentProductId.isEmpty()) {
        QMessageBox::warning(this, "Atenção",
                             "Selecione um produto na tabela para deletar.");
        return;
    }

    auto resp = QMessageBox::question(
        this,
        "Confirmar exclusão",
        "Tem certeza que deseja DELETAR este produto?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (resp == QMessageBox::No)
        return;

    if (db->deleteProduct(currentProductId)) {
        QMessageBox::information(this, "Sucesso",
                                 "Produto deletado com sucesso!");
        clearFields();
        setFieldStatus(false);
        populateTable();
    } else {
        QMessageBox::critical(this, "Erro",
                              "Não foi possível deletar o produto.");
    }
}

void Produto::on_tabelaProdutos_cellClicked(int row, int column)
{
    Q_UNUSED(column);

    auto *itemCodigo  = ui->tabelaProdutos->item(row, 0);
    auto *itemDesc    = ui->tabelaProdutos->item(row, 1);
    auto *itemValor   = ui->tabelaProdutos->item(row, 2);
    auto *itemEst     = ui->tabelaProdutos->item(row, 3);
    auto *itemEstMin  = ui->tabelaProdutos->item(row, 4);

    if (!itemCodigo || !itemDesc || !itemValor ||
        !itemEst || !itemEstMin)
        return;

    currentProductId = itemCodigo->text();

    ui->txtCodigo->setText(itemCodigo->text());
    ui->txtDescricao->setText(itemDesc->text());

    QString valorLimpo = itemValor->text();
    valorLimpo.remove("R$ ");
    ui->txtValor->setText(valorLimpo);

    ui->txtEstoque->setText(itemEst->text());
    ui->txtEstoqueMin->setText(itemEstMin->text());

    setFieldStatus(true);
    // Mantém o botão Deletar ativo
    ui->btnDeletar->setEnabled(true);
}
