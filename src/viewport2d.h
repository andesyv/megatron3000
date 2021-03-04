#ifndef VIEWPORT2D_H
#define VIEWPORT2D_H

#include <QWidget>
#include <memory>

class Renderer;
namespace Ui {
class Viewport2D;
}

class Viewport2D : public QWidget
{
    Q_OBJECT

public:
    explicit Viewport2D(QWidget *parent = nullptr);
    ~Viewport2D();

private:
    Ui::Viewport2D *ui;
    std::unique_ptr<Renderer> mRenderer;
};

#endif // VIEWPORT2D_H
