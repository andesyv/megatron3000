#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include <QWidget>
#include <memory>

class Renderer;

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
    std::unique_ptr<Ui::Viewport3D> ui;
    std::unique_ptr<Renderer> mRenderer;
};

#endif // VIEWPORT3D_H
