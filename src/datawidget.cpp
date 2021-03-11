#include "datawidget.h"
#include "ui_datawidget.h"


DataWidget::DataWidget(Volume *vol, QWidget *parent) :
    QWidget(parent),
    m_volume(vol),
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

void DataWidget::on_listView_doubleClicked(const QModelIndex &index)
{
    // This is the absolute file path to the file double clicked
    QString filePath = filemodel->fileInfo(index).absoluteFilePath();

    if(filemodel->fileInfo(index).suffix()=="dat") {
        qInfo() << "Successfully loaded file at " + filePath;
        m_volume->loadData(filePath);
        this->close();
    } else {
        //Not supported file type
        qInfo() << "Failed to load unsupported filetype at path " + filePath;
    }
}
