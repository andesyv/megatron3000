#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

#include <QObject>
#include <memory>

class MainWindow;
class Volume;
class QMenuBar;
class QMenu;
class QWidget;

/**
 * @brief Small interface class for common stuff shared between multiple widgets with menus + volume management
 * 
 */
class IMenu {
public:
    IMenu(QWidget* widget);

    QMenuBar* mMenuBar{nullptr};
    QMenu* mViewMenu{nullptr};
    QMenu* mDataMenu{nullptr};

    virtual void volumeSwitched();

    auto getVolume() const { return mVolume; }
    
    virtual ~IMenu() = 0;

    void load();

protected:
    std::shared_ptr<Volume> mVolume;

private:
    MainWindow* mMenuMainWindow;
    QWidget* mWidget;

    void updateDataMenu();

    bool setInitData();
};

#endif // MENUINTERFACE_H