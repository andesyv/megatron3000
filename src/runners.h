#ifndef RUNNERS_H
#define RUNNERS_H

/**
 * @brief Helper worker classes (runners) for multithreading 
 */

#include <QRunnable>
#include <QFuture>
#include <QFutureWatcher>
#include <memory>
#include "volume.h"
#include "renderutils.h"

/**
 * @brief Helper class to wrap the histogram generation in a separate thread
 * This class is used by the global QThreadPool, which puts these runners in
 * a queue for them to wait their turn in order for a thread to become avilable
 * for them. When a thread is available, the HistogramRunners generates
 * a histogram, which after finished is inserted into the widget.
 * 
 * HistogramRunners can be cancelled mid-generation by setting mCancelled = true,
 * which will early return the connected future. 
 */
class HistogramRunner : public QRunnable {
public:
    using RetType = QVector<qreal>;
    using FType = QFuture<QVector<qreal>>;

    HistogramRunner(
        unsigned int binCount = 256,
        HistogramSource source = HistogramSource::All,
        std::shared_ptr<Volume> volume = {},
        std::vector<QVector4D> tfValues = {},
        const Slicing::Plane& plane = {}
    );
    ~HistogramRunner() {}

    void run() override;
    QFuture<QVector<qreal>> future();

    bool mCancelled{false};
    const unsigned int mBinCount{30};
    const HistogramSource mSource{HistogramSource::All};
    std::unique_ptr<HistogramRunner> mPrevRunner;
    QFuture<QVector<qreal>> mPrevFuture;
    Slicing::Plane mPlane;

private:
    // https://stackoverflow.com/questions/59197694/qt-how-to-create-a-qfuture-from-a-thread
    QFutureInterface<QVector<qreal>> mFutureInterface;
    std::shared_ptr<Volume> mVolume;
    std::vector<QVector4D> mTfValues;
};





class QOpenGLContext;
class QOffscreenSurface;

/**
 * @brief Helper class to wrap data loading in a separate thread
 * This class is used by the global QThreadPool, which puts these runners in
 * a queue for them to wait their turn in order for a thread to become avilable
 * for them.
 * 
 * DataReaderRunner can be cancelled mid-generation by setting mCancelled = true,
 * which will early return the connected future. 
 */
class DataReaderRunner : public QRunnable {
public:
    using RetType = std::shared_ptr<Volume>;
    using FType = QFuture<RetType>;

    DataReaderRunner(
        const QString& filePath,
        QOpenGLContext* context,
        QOffscreenSurface* surface
    );
    ~DataReaderRunner() {}


    void run() override;
    FType future();
    const QString mPath;

    bool mCancelled{false};
    std::unique_ptr<DataReaderRunner> mPrevRunner;
    FType mPrevFuture;

private:
    // https://stackoverflow.com/questions/59197694/qt-how-to-create-a-qfuture-from-a-thread
    QFutureInterface<RetType> mFutureInterface;

    QOpenGLContext* mContext{nullptr};
    QOffscreenSurface* mSurface{nullptr};
};

#endif // RUNNERS_H