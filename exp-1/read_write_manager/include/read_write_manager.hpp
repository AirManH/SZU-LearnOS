#ifndef READ_WRITE_MANAGER_HPP
#define READ_WRITE_MANAGER_HPP


#include <QMutex>
#include <QThread>
#include <QVector>
#include <QPointer>
#include <QObject>

namespace los
{

class Reader: public QThread
{
Q_OBJECT

    void run() override;
public:
    using QThread::QThread;
    Reader(QObject *parent, quint32 &readerCount, QMutex &rMutex, QMutex &wMutex, const QString &path);

private:
    quint32 &reader_count;
    QMutex &r_mutex, &w_mutex;
    const QString &path;
//signals:
//    void result_ready(QString s);
};

class Writer: public QThread
{
Q_OBJECT

    void run() override;
public:
    using QThread::QThread;
    Writer(QObject *parent, QMutex &wMutex, const QString &path, const QString &content);

private:
    QMutex &w_mutex;
    const QString &path, &content;
};

class ReadWriteManager: public QObject
{
Q_OBJECT

public:
    using QObject::QObject;
    explicit ReadWriteManager(QString filePath);
    ~ReadWriteManager() override;
    void add_readers(quint32 n);
    void add_writers(quint32 n);

private:
    QMutex w_mutex, r_mutex;
    quint32 reader_count;
    QVector<QPointer<los::Reader>> reader_threads;
    QVector<QPointer<los::Writer>> writer_threads;
    QString file_path;
};

} // namespace los

#endif //READ_WRITE_MANAGER_HPP
