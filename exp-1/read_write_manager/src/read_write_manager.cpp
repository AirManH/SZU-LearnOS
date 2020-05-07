#include <read_write_manager.hpp>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <szu-learnos-utils.hpp>
#include <utility>

namespace los
{

void Reader::read(quint32 reader_count, QMutex &r_mutex, QMutex &w_mutex, const QString &path)
{
    // here we lock rMutex for increasing reader_count
    r_mutex.lock();

    if (reader_count == 0) {
        // reader count == 0 means that
        // there may be writer working,
        // since we have to lock the w_mutex
        w_mutex.lock();
    }
    reader_count += 1;

    r_mutex.unlock();

    // actually read here
    QFile data(path);
    if (data.open(QFile::ReadOnly | QFile::Truncate)) {
        QTextStream in(&data);
        QString text;
        in >> text;
        qDebug() << text;
        // emit result_ready(text);
    }


    // here we lock r_mutex for decreasing reader_count
    r_mutex.lock();

    reader_count -= 1;
    if (reader_count == 0) {
        // now reader_count == 0 means that
        // after reading, there is no more readers,
        // so writers may get the lock.
        w_mutex.unlock();
    }

    r_mutex.unlock();
}

void Writer::write(QMutex &w_mutex, const QString &path, const QString &content)
{
    w_mutex.lock();

    QFile data(path);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        out << content;
    }

    qDebug() << "writing\n";

    w_mutex.unlock();
}

void ReadWriteManager::add_readers(quint32 n)
{
    for (auto i: range(n)) {
        auto p_worker = QPointer<Reader>(new Reader());
        auto p_thread = QPointer<QThread>(new QThread);
        p_worker->moveToThread(p_thread);
        connect(p_thread, &QThread::finished,
                p_worker, &Reader::deleteLater);
        connect(this, &ReadWriteManager::all_threads_begin_read,
                p_worker, &Reader::read);
        connect(p_worker, &Reader::result_ready, this,
                &ReadWriteManager::get_one_reader_result);
        p_thread->start();
    }
}

void ReadWriteManager::add_writers(quint32 n)
{
    for (auto i: range(n)) {
        auto p_worker = QPointer<Writer>(new Writer);
        auto p_thread = QPointer<QThread>(new QThread);
        p_worker->moveToThread(p_thread);
        connect(p_thread, &QThread::finished,
                p_worker, &Writer::deleteLater);
        connect(this, &ReadWriteManager::all_threads_begin_write,
                p_worker, &Writer::write);
        p_thread->start();
    }
}

ReadWriteManager::~ReadWriteManager()
{
    for (auto p: this->reader_threads) {
        p->quit();
        p->wait();
    }
    for (auto p: this->writer_threads) {
        p->quit();
        p->wait();
    }
}
void ReadWriteManager::run()
{
    emit all_threads_begin_read(this->reader_count, this->r_mutex, this->w_mutex, this->file_path);
    emit all_threads_begin_write(this->w_mutex, this->file_path, "aa");
}
void ReadWriteManager::get_one_reader_result(const QString &s)
{}

ReadWriteManager::ReadWriteManager(QObject *parent, const QString &filePath)
    : QObject(parent), file_path(filePath)
{}

}