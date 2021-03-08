#include "datawidget.h"
#include "ui_datawidget.h"

DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);

    //Directory model
    QString rootPath = "C:/";
    dirmodel = new QFileSystemModel;
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(rootPath);

    ui->treeView->setModel(dirmodel);

    //File model
    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filemodel->setRootPath(rootPath);

    ui->listView->setModel(filemodel);

}

DataWidget::~DataWidget()
{
    delete ui;
}


void DataWidget::on_treeView_clicked(const QModelIndex &index)
{
    QString dirPath = dirmodel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(filemodel->setRootPath(dirPath));
}
