#include <QTextStream>
#include <QFile>
#include <read_write_manager.hpp>
#include <QString>
#include <QStringList>

namespace los
{

void Reader::read()
{
    // here we lock rMutex for increasing reader_count
    debug("R-" + QString::number(id) + " try to lock R");
    r_mutex.lock();
    debug("R-" + QString::number(id) + " get lock R");

    if (reader_count == 0) {
        // reader count == 0 means that
        // there may be writer working,
        // since we have to lock the w_mutex
        debug("R-" + QString::number(id) + " try to lock W");
        w_mutex.lock();
        debug("R-" + QString::number(id) + " get lock W");
    }

    reader_count += 1;

    debug("R-" + QString::number(id) + " try to release R");
    r_mutex.unlock();
    debug("R-" + QString::number(id) + " release R");

    // actually read here
    QFile data(path);
    auto s_list = QStringList();
    if (data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&data);
        while (!in.atEnd()) {
            s_list.append(in.readLine());
        }
        QString content = s_list.join("\n");
        info("R-" + QString::number(id) + " READ: " + content);
        emit result_ready(content);
    }

    debug("R-" + QString::number(id) + " try to lock R");
    // here we lock r_mutex for decreasing reader_count
    r_mutex.lock();
    debug("R-" + QString::number(id) + " get lock R");

    reader_count -= 1;
    if (reader_count == 0) {
        // now reader_count == 0 means that
        // after reading, there is no more readers,
        // so writers may get the lock.
        debug("R-" + QString::number(id) + " try to release W");
        w_mutex.unlock();
        debug("R-" + QString::number(id) + " release W");
    }

    r_mutex.unlock();
}

void Writer::write()
{
    debug("R-" + QString::number(id) + " try to lock R");
    w_mutex.lock();
    debug("R-" + QString::number(id) + " get lock R");

    QFile data(path);
    if (data.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&data);
        out << content;
        info("W-" + QString::number(id) + " WROTE: " + content);
    }

    debug("R-" + QString::number(id) + " try to release R");
    w_mutex.unlock();
    debug("R-" + QString::number(id) + " release R");
}

Writer::Writer(QMutex &w_mutex,
               const QString &path,
               const QString &content,
               qint32 id)
    : w_mutex(w_mutex),
      path(path),
      content(content),
      id(id)
{}

void ReadWriteManager::add_readers(quint32 n)
{
    for (auto i: range(n)) {
        auto p_worker = QPointer<Reader>(
            new Reader(reader_count, r_mutex, w_mutex, file_path, i));
        auto p_thread = QPointer<QThread>(new QThread);

        this->reader_wokers.append(p_worker);
        this->reader_threads.append(p_thread);

        p_worker->moveToThread(p_thread);
        // auto delete worker after thread finish
        connect(p_thread, &QThread::finished,
                p_worker, &Reader::deleteLater);
        // ReadWriteManager controls worker
        connect(this, &ReadWriteManager::all_threads_begin_read,
                p_worker, &Reader::read);
        // receive result from worker
        connect(p_worker, &Reader::result_ready,
                this, &ReadWriteManager::get_one_reader_result);
        // redirect worker's to_debug(), to_info() to this to_debug(), to_info()
        connect(p_worker, &Reader::to_debug,
                this, &ReadWriteManager::to_debug);
        connect(p_worker, &Reader::to_info,
                this, &ReadWriteManager::to_info);
        p_thread->start();
    }
}

void ReadWriteManager::add_writers(quint32 n)
{
    for (auto i: range(n)) {

        auto content = QString::number(i);

        auto p_worker = QPointer<Writer>(
            new Writer(w_mutex, file_path, content, i));
        auto p_thread = QPointer<QThread>(new QThread);

        this->writer_threads.append(p_thread);
        this->writer_wokers.append(p_worker);

        p_worker->moveToThread(p_thread);
        // auto delete worker after thread finish
        connect(p_thread, &QThread::finished,
                p_worker, &Writer::deleteLater);
        // ReadWriteManager controls worker
        connect(this, &ReadWriteManager::all_threads_begin_write,
                p_worker, &Writer::write);
        // redirect worker's to_debug(), to_info() to this to_debug(), to_info()
        connect(p_worker, &Writer::to_debug,
                this, &ReadWriteManager::to_debug);
        connect(p_worker, &Writer::to_info,
                this, &ReadWriteManager::to_info);
        p_thread->start();
    }
}

void ReadWriteManager::reset()
{

    for (auto p: this->reader_threads) {
        p->quit();
        p->wait();
    }
    for (auto p: this->writer_threads) {
        p->quit();
        p->wait();
    }

    reader_count = 0;

    QFile data(this->file_path);
    if (data.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    }
}

void ReadWriteManager::set_writers_and_readers(quint32 w, quint32 r)
{
    this->reset();

    this->add_writers(w);
    this->add_readers(r);

    emit number_changed();
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
    emit all_threads_begin_read();
    emit all_threads_begin_write();
}

void ReadWriteManager::get_one_reader_result(const QString &s)
{
    // no use anymore here
    // qDebug() << "Read: " + s;
}

ReadWriteManager::ReadWriteManager(const QString &filePath)
    : file_path(filePath),
      reader_count(0),
      w_mutex(),
      r_mutex()
{
    QFile data(this->file_path);
    if (data.open(QIODevice::WriteOnly)) {
        QTextStream out(&data);
        out << QString("");
    }
}

Reader::Reader(quint32 &reader_count,
               QMutex &r_mutex,
               QMutex &w_mutex,
               QString path,
               qint32 id)
    : reader_count(reader_count),
      r_mutex(r_mutex),
      w_mutex(w_mutex),
      path(path),
      id(id)
{}

}
