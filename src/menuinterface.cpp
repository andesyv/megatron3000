#include "menuinterface.h"

IMenu::IMenu(QWidget* widget) {
    mMenuBar = new QMenuBar{widget};
    mMenuBar->setSizePolicy(QSizePolicy{QSizePolicy::Fixed, QSizePolicy::Fixed});
    mViewMenu = mMenuBar->addMenu("View");
}

IMenu::~IMenu() {}