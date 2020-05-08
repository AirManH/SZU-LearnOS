#ifndef READ_WRITE_MANAGER_HPP
#define READ_WRITE_MANAGER_HPP

#include <QtCore>
#include <QMutex>
#include <QThread>
#include <QVector>
#include <QPointer>
#include <QObject>
#include <szu-learnos-utils.hpp>

namespace los
{

class Reader: public QObject, public Loggable
{
Q_OBJECT

public:
    Reader(quint32 &reader_count,
           QMutex &r_mutex,
           QMutex &w_mutex,
           QString path,
           qint32 id = 0);
public slots:
    void read();
private:
    quint32 &reader_count;
    QMutex &r_mutex, &w_mutex;
    QString path;
    qint32 id;


signals:
    void result_ready(const QString &s);
    void to_debug(QString s) override;
    void to_info(QString s) override;
};

class Writer: public QObject, public Loggable
{
Q_OBJECT

public:
    Writer(QMutex &w_mutex,
           const QString &path,
           const QString &content,
           qint32 id = 0);
public slots:
    void write();
private:
    QMutex &w_mutex;
    QString path;
    QString content;
    qint32 id;
signals:
    void write_over();
    void to_debug(QString s) override;
    void to_info(QString s) override;

};

class ReadWriteManager: public QObject, public Loggable
{
Q_OBJECT

public:

    ReadWriteManager(const QString &filePath);
    ~ReadWriteManager() override;

private:
    QMutex w_mutex, r_mutex;
    quint32 reader_count;

    QString file_path;

    QVector<QPointer<QThread>> reader_threads;
    QVector<QPointer<Reader>> reader_wokers;

    QVector<QPointer<QThread>> writer_threads;
    QVector<QPointer<Writer>> writer_wokers;

    void add_readers(quint32 n);
    void add_writers(quint32 n);

    void reset();

public slots:
    void set_writers_and_readers(quint32 w, quint32 r);
    void run();
    void get_one_reader_result(const QString &s);

signals:
    void all_threads_begin_read();
    void all_threads_begin_write();
    void number_changed();

    void to_debug(QString s) override;
    void to_info(QString s) override;
};

} // namespace los

#endif //READ_WRITE_MANAGER_HPP
