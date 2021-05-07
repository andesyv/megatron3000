#include "datawidget.h"
#include "ui_datawidget.h"
#include "volume.h"
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <filesystem>
#include <fstream>
#include <QOpenGLContext>
#include <QOffscreenSurface>


DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);

    //Directory model
    QString rootPath = "C:/";
    dirmodel = new QFileSystemModel;
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(rootPath);

    ui->treeView->setModel(dirmodel);

    //File model
    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filemodel->setRootPath(rootPath);

    ui->listView->setModel(filemodel);

    // Create opengl context for thread:
    mThreadContext = std::make_unique<QOpenGLContext>();
    mThreadContext->setShareContext(QOpenGLContext::globalShareContext());
    if (!mThreadContext->create())
        throw std::runtime_error{"Failed to create context for Volume!"};

    // Create surface for thread;
    mThreadSurface = std::make_unique<QOffscreenSurface>();
    mThreadSurface->setFormat(QSurfaceFormat::defaultFormat());
    mThreadSurface->create();

    connect(&mWatcher, SIGNAL(finished()), this, SLOT(finishLoading()));
}

void DataWidget::loadCached() {
    std::ifstream in{"lastopened", std::fstream::in};
    if (!in) {
#ifndef NDEBUG
        qWarning() << "File \"lastopened\" is not possible to open.";
#endif
        return;
    }

    std::string path;
    in >> path;

    load(QString::fromStdString(path));
}

DataWidget::~DataWidget()
{
    delete ui;
}


void DataWidget::on_treeView_clicked(const QModelIndex &index)
{
    QString dirPath = dirmodel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(filemodel->setRootPath(dirPath));
}

void DataWidget::on_listView_doubleClicked(const QModelIndex &index)
{
    // This is the absolute file path to the file double clicked
    QString filePath = filemodel->fileInfo(index).absoluteFilePath();
    if (filemodel->fileInfo(index).suffix()=="dat") {
        load(filePath);
    } else {
        //Not supported file type
        qInfo() << "Failed to load unsupported filetype at path " + filePath;
    }
}

void DataWidget::finishLoading() {
    const auto& volume = mFuture.result();
    const auto& filePath = mRunner->mPath;

    if (volume) {
        qInfo() << "Successfully loaded file at " + filePath;

        const auto& fileIdentifier = std::filesystem::path{filePath.toStdString()}.stem().string();
        // If we managed to load the volume, pass it as a signal for anyone to catch
        loaded(volume, fileIdentifier);

        // cache what file were opened last:
        cacheLast(filePath);
    } else if (!mGlobalCancelled) {
        qInfo() << "Failed to read file at " + filePath;
    }

    mRunner.release();
    // Yoink the thread back so we can safely clean up
    mThreadContext->moveToThread(thread());
    mThreadSurface->moveToThread(thread());

    finished();

    if (volume)
        close();
}

void DataWidget::closeEvent(QCloseEvent* event) {
    // If we're cancelling, block all close events:
    if (mGlobalCancelled) {
        event->ignore();

    // If we have a job running, cancel that first and then close window:
    } else if (mRunner) {
        mGlobalCancelled = true;
        connect(this, &DataWidget::finished, this, [&](){
#ifndef NDEBUG
            qDebug() << "Cancelled data loading";
#endif
            mGlobalCancelled = false;
            close();
        });
        mRunner->mCancelled = true;
    } else {
        event->accept();
    }
}

void DataWidget::load(const QString& filePath) {
    mGlobalCancelled = false;
    auto oldRunner = std::move(mRunner);
    const auto oldFuture = mFuture;

    // Remove thread affinity (so the runner can "take" it)
    // Only remove thread affinity if this is the owner, otherwise let the runner do it
    if (mThreadContext->thread() == thread()) {
        mThreadContext->moveToThread(nullptr);
        mThreadSurface->moveToThread(nullptr);
    }

    // Create new runner:
    mRunner = std::make_unique<DataReaderRunner>(filePath, mThreadContext.get(), mThreadSurface.get());

    // Move cleanup responsibility of old runner to new runner
    mRunner->mPrevRunner = std::move(oldRunner);
    mRunner->mPrevFuture = oldFuture;
    
    // Get the new future:
    mFuture = mRunner->future();
    mWatcher.setFuture(mFuture);

    if (mRunner->mPrevRunner)
        mRunner->mPrevRunner->mCancelled = true;

    // Start new thread (add it to queue)
    QThreadPool::globalInstance()->start(mRunner.get());
}

void DataWidget::cacheLast(const QString& filePath) {
    std::ofstream out{"lastopened", std::fstream::out | std::fstream::trunc};
    if (!out) {
#ifndef NDEBUG
        qWarning() << "File \"lastopened\" is not possible to open.";
#endif
        return;
    }

    out << filePath.toStdString();
}





/* 
 * QFutureInterface is an udocumented class for interacting with
 * QFutures from Qt5, replaced by QPromise in Qt6. In order
 * for it to be able to send signals across threads, it needs
 * to be declared as a metatype.
 * See https://stackoverflow.com/questions/59197694/qt-how-to-create-a-qfuture-from-a-thread
 */
Q_DECLARE_METATYPE( QFutureInterface<DataReaderRunner::RetType> );

DataReaderRunner::DataReaderRunner(const QString& filePath, QOpenGLContext* context, QOffscreenSurface* surface)
    : mPath{filePath}, mContext{context}, mSurface{surface} {
    // Need to call this line once, hence why it's static.
    static const auto typeId{qRegisterMetaType<QFutureInterface<DataReaderRunner::RetType>>()};

    setAutoDelete(false);
    // Initiate future:
    mFutureInterface.reportStarted();
}

void DataReaderRunner::run() {
    // If a previous runner is still running,
    // wait for that one before starting to cleanup after it.
    if (mPrevRunner) {
        // Catch up to other thread by just waiting
        mPrevFuture.waitForFinished();
        // Delete old thread
        mPrevRunner.reset();
    }

    // "Steal" the context and the surface
    mContext->moveToThread(QThread::currentThread());
    mSurface->moveToThread(QThread::currentThread());

    auto volume = std::make_shared<Volume>(mContext, mSurface);

    const auto bLoadStatus = volume->loadData(mPath, mCancelled);
    if (bLoadStatus) {
        // Move volume ownership to main thread
        volume->moveToThread(QApplication::instance()->thread());
    } else {
        // Manually clear resources before returning ownership of context
        volume.reset();
    }

    // Remove thread for context and surface (for cleanup)
    mContext->moveToThread(nullptr);
    mSurface->moveToThread(nullptr);
    mContext = nullptr;
    mSurface = nullptr;

    mFutureInterface.reportResult(volume);
    mFutureInterface.reportFinished();
}

DataReaderRunner::FType DataReaderRunner::future() {
    return mFutureInterface.future();
}