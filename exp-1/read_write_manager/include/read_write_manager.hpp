#ifndef READ_WRITE_MANAGER_HPP
#define READ_WRITE_MANAGER_HPP

#include <QtCore>
#include <QMutex>
#include <QThread>
#include <QVector>
#include <QPointer>
#include <QObject>

namespace los
{

class Reader: public QObject
{
Q_OBJECT

public:
    using QObject::QObject;
public slots:
    void read(quint32 reader_count,
              QMutex &r_mutex,
              QMutex &w_mutex,
              const QString &path);

signals:
    void result_ready(const QString &s);;
};

class Writer: public QObject
{
Q_OBJECT

public:
    using QObject::QObject;
public slots:
    void write(QMutex &w_mutex,
               const QString &path,
               const QString &content);

signals:
    void write_over();;

};

class ReadWriteManager: public QObject
{
Q_OBJECT

public:
    using QObject::QObject;
    ReadWriteManager(QObject *parent, const QString &filePath);
    ~ReadWriteManager() override;
    void add_readers(quint32 n);
    void add_writers(quint32 n);
    void run();

private:
    QMutex w_mutex, r_mutex;
    quint32 reader_count;

    QString file_path;

    QVector<QPointer<QThread>> reader_threads;
    QVector<QPointer<Reader>> reader_wokers;

    QVector<QPointer<QThread>> writer_threads;
    QVector<QPointer<Writer>> writer_wokers;

public slots:
    void get_one_reader_result(const QString &s);

signals:
    void all_threads_begin_read(quint32 reader_count,
                                QMutex &r_mutex,
                                QMutex &w_mutex,
                                const QString &path);

    void all_threads_begin_write(QMutex &w_mutex,
                                 const QString &path,
                                 const QString &content);
};

} // namespace los

#endif //READ_WRITE_MANAGER_HPP
