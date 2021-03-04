#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include <QWidget>

namespace Ui {
class Viewport3D;
}

class Viewport3D : public QWidget
{
    Q_OBJECT

public:
    explicit Viewport3D(QWidget *parent = nullptr);
    ~Viewport3D();

private:
    Ui::Viewport3D *ui;
};

#endif // VIEWPORT3D_H
