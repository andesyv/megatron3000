#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>


namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataWidget(QWidget *parent = nullptr);
    ~DataWidget();

private slots:
    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::DataWidget *ui;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;
};

#endif // DATAWIDGET_H
