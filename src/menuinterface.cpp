#include "menuinterface.h"
#include "mainwindow.h"
#include "datawidget.h"
#include <QMenu>
#include <QMenuBar>

IMenu::IMenu(QWidget* widget, bool bAutoLoad) : mWidget{widget}, mAutoLoad{bAutoLoad} {
    // Follow outer chain to find main window:
    auto mw = widget->window();
    while (mw != nullptr && !mw->isWindow())
        mw = mw->window();
    
    mMenuMainWindow = dynamic_cast<MainWindow*>(mw);
    

    mMenuBar = new QMenuBar{widget};
    mMenuBar->setSizePolicy(QSizePolicy{QSizePolicy::Fixed, QSizePolicy::Fixed});
    mViewMenu = mMenuBar->addMenu("View");

    if (mMenuMainWindow == nullptr)
        return;

    mDataMenu = mMenuBar->addMenu("Data");
    // Note: Weird way to wrap signals and slots around not using qobjects.
    widget->connect(mDataMenu, &QMenu::aboutToShow, widget, [=](){ this->updateDataMenu(); });
    // updateDataMenu();

    const auto& volumes = mMenuMainWindow->volumesList();
    if (!volumes.empty()) {
        mVolume = volumes.front().second;
    // If auto-loading, take the first and best one:
    } else if (bAutoLoad) {
        QMetaObject::Connection autoLoadConnection;
        autoLoadConnection = widget->connect(mMenuMainWindow, &MainWindow::loaded, widget, [=](const std::shared_ptr<Volume>& volume){
            // If we're done with auto-loading, remove this binding and quit
            if (!mAutoLoad) {
                widget->disconnect(autoLoadConnection);
                return;
            }

            if (!mVolume) {
                mVolume = volume;
                volumeSwitched();
            }
        });
    }
}

void IMenu::volumeSwitched() {}

void IMenu::load() {
    // If we're using this load button as the first button, switch to global loading.
    if (mMenuMainWindow->volumesList().empty()) {
        mMenuMainWindow->load();
    } else {
        // If manually loading a new volume, turn off automatic loading.
        mAutoLoad = false;
        const auto dataWidget = mMenuMainWindow->loadData();
        mWidget->connect(dataWidget, &DataWidget::loaded, mWidget, [&](std::shared_ptr<Volume> volume){
            mVolume = volume;
            volumeSwitched();
        });
    }
}

void IMenu::updateDataMenu() {
    mDataMenu->clear();

    const auto openAction = mDataMenu->addAction("Open");
    mWidget->connect(openAction, &QAction::triggered, mWidget, [=](){ this->load(); });
    
    mDataMenu->addSeparator();

    const auto volumes = mMenuMainWindow->volumesList();
    std::vector<QAction*> volumeButtons;
    volumeButtons.reserve(volumes.size());

    for (const auto& [name, vol] : volumes) {
        auto action = mDataMenu->addAction(name.c_str());
        action->setCheckable(true);
        mWidget->connect(action, &QAction::triggered, mWidget, [=, vol = vol](bool bState){
            // If attempting to uncheck checked button, just don't do anything:
            if (!bState) {
                action->setChecked(true);
                return;
            }

            // Uncheck all other buttons
            for (auto& button : volumeButtons)
                if (button != action)
                    button->setChecked(false);
            
            // Update volume to new volume
            mVolume = vol;

            volumeSwitched();
        });


        // Update selection of current volume
        if (vol == mVolume)
            action->setChecked(true);
        
        volumeButtons.push_back(action);
    }
}

bool IMenu::setInitData() {
    // Set first volume to be default, if we have any
    const auto& volumes = mMenuMainWindow->volumesList();
    if (!volumes.empty()) {
        mVolume = volumes.front().second;
        return true;
    }

    return false;
}

IMenu::~IMenu() {}