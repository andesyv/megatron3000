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
    // ui classes don't get garbage collected,
    // so here it's actually smart to use a smart pointer
    std::unique_ptr<Ui::Viewport2D> ui;
    Renderer* mRenderer;
};

#endif // VIEWPORT2D_H
