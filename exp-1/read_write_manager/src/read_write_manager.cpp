#include <read_write_manager.hpp>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <szu-learnos-utils.hpp>
#include <utility>

void los::Reader::run()
{
    // here we lock r_mutex for increasing reader_count
    this->r_mutex.lock();

    if (this->reader_count == 0) {
        // reader count == 0 means that
        // there may be writer working,
        // since we have to lock the w_mutex
        this->w_mutex.lock();
    }
    reader_count += 1;

    this->r_mutex.unlock();

    // actually read here
    QFile data(this->path);
    if (data.open(QFile::ReadOnly | QFile::Truncate)) {
        QTextStream in(&data);
        QString text;
        in >> text;
        qDebug() << text;
        // emit result_ready(text);
    }


    // here we lock r_mutex for decreasing reader_count
    this->r_mutex.lock();

    reader_count -= 1;
    if (this->reader_count == 0) {
        // now reader_count == 0 means that
        // after reading, there is no more readers,
        // so writers may get the lock.
        this->w_mutex.unlock();
    }

    this->r_mutex.unlock();
}

los::Reader::Reader(QObject *parent, quint32 &readerCount, QMutex &rMutex, QMutex &wMutex, const QString &path)
    : QThread(parent), reader_count(readerCount), r_mutex(rMutex), w_mutex(wMutex), path(path)
{}


void los::Writer::run()
{
    this->w_mutex.lock();

    QFile data(this->path);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        out << this->content;
    }

    qDebug() << "writing\n";

    this->w_mutex.unlock();
}

los::Writer::Writer(QObject *parent, QMutex &wMutex, const QString &path, const QString &content)
    : QThread(parent), w_mutex(wMutex), path(path), content(content)
{}

los::ReadWriteManager::ReadWriteManager(QString filePath)
    : file_path(std::move(filePath)), w_mutex(), r_mutex(), reader_count(0)
{}

void los::ReadWriteManager::add_readers(quint32 n)
{
        foreach(quint64 i, los::range(n)) {
            auto *reader = new los::Reader(this, this->reader_count, this->r_mutex, this->w_mutex, this->file_path);
            QPointer<los::Reader> p_reader(reader);
            this->reader_threads.append(p_reader);
            p_reader->start();
        }
}

void los::ReadWriteManager::add_writers(quint32 n)
{
        foreach(quint64 i, los::range(n)) {
            auto content = QString("Writer: %1\n").arg(this->writer_threads.size());
            auto *writer = new los::Writer(this, this->w_mutex, this->file_path, content);
            QPointer<los::Writer> p_writer(writer);
            this->writer_threads.append(p_writer);
            p_writer->start();
        }
}

los::ReadWriteManager::~ReadWriteManager()
{
    foreach (auto p, this->reader_threads) {
        p->wait(10);
    }
    foreach (auto p, this->writer_threads) {
        p->wait(10);
    }
}
