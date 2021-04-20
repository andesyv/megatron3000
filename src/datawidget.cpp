#include "datawidget.h"
#include "ui_datawidget.h"
#include "volume.h"
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <filesystem>
#include <fstream>


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

void DataWidget::loadCached() {
    std::ifstream in{"lastopened", std::fstream::in};
    if (!in) {
#ifndef NDEBUG
        qWarning() << "File \"lastopened\" is not possible to open.";
#endif
        return;
    }

    std::string path;
    in >> path;

    load(QString::fromStdString(path));
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
    if (filemodel->fileInfo(index).suffix()=="dat") {
        load(filePath);
    } else {
        //Not supported file type
        qInfo() << "Failed to load unsupported filetype at path " + filePath;
    }
}

void DataWidget::load(const QString& filePath) {
    auto volume = std::make_shared<Volume>();
    if(volume->loadData(filePath)) {
        qInfo() << "Successfully loaded file at " + filePath;

        const auto& fileIdentifier = std::filesystem::path{filePath.toStdString()}.stem().string();
        // If we managed to load the volume, pass it as a signal for anyone to catch
        loaded(volume, fileIdentifier);
        this->close();

        // cache what file were opened last:
        cacheLast(filePath);
    } else {
        qInfo() << "Failed to read file at " + filePath;
    }
}

void DataWidget::cacheLast(const QString& filePath) {
    std::ofstream out{"lastopened", std::fstream::out | std::fstream::trunc};
    if (!out) {
#ifndef NDEBUG
        qWarning() << "File \"lastopened\" is not possible to open.";
#endif
        return;
    }

    out << filePath.toStdString();
}