#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>

class Volume;
class QFileSystemModel;

namespace Ui {
class DataWidget;
}

class DataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataWidget(QWidget *parent = nullptr);

    void loadCached();

    ~DataWidget();

signals:
    void loaded(std::shared_ptr<Volume> volume, const std::string& identifier = {});

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::DataWidget *ui;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;

    void load(const QString& filePath);

    void cacheLast(const QString& filePath);
};

#endif // DATAWIDGET_H
