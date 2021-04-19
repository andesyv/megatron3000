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
    /**
     * @brief Construct a new IMenu object
     * 
     * @param widget Widget to attach interface to. Usually "this"
     * @param bAutoLoad Whether to automatically load a volume whenever one becaomes available. Triggers volumeSwitched()
     */
    IMenu(QWidget* widget, bool bAutoLoad = true);

    QMenuBar* mMenuBar{nullptr};
    QMenu* mViewMenu{nullptr};
    QMenu* mDataMenu{nullptr};

    auto getVolume() const { return mVolume; }
    
    virtual ~IMenu() = 0;

    void load();

protected:
    std::shared_ptr<Volume> mVolume;

    /**
     * @brief Virtual function that can be overriden to run code on volume selection
     * @note Does not run in constructor, only on executive calls
     */
    virtual void volumeSwitched() = 0;

private:
    MainWindow* mMenuMainWindow;
    QWidget* mWidget;
    bool mAutoLoad;

    void updateDataMenu();

    bool setInitData();
};

#endif // MENUINTERFACE_H