#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include "volume.h"


namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataWidget(Volume *vol, QWidget *parent = nullptr);
    ~DataWidget();

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Volume *m_volume;
    Ui::DataWidget *ui;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;
};

#endif // DATAWIDGET_H
