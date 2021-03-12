#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

#include <QMenuBar>

/**
 * @brief Small interface class for common stuff shared between multiple widgets with menus
 * 
 */
class IMenu {
public:
    IMenu(QWidget* widget);

    QMenuBar* mMenuBar{nullptr};
    QMenu* mViewMenu{nullptr};
    
    virtual ~IMenu() = 0;
};

#endif // MENUINTERFACE_H