#include <read_write_manager.hpp>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <szu-learnos-utils.hpp>
#include <utility>


namespace los
{

void Reader::read()
{
    qDebug() << "[R] " << QThread::currentThreadId() << QString(": try lock R");
    // here we lock rMutex for increasing reader_count
    r_mutex.lock();
    qDebug() << "[R] " << QThread::currentThreadId() << QString(": get lock R");

    if (reader_count == 0) {
        // reader count == 0 means that
        // there may be writer working,
        // since we have to lock the w_mutex

        qDebug() << "[R] " << QThread::currentThreadId() << QString(": try lock W");
        w_mutex.lock();
        qDebug() << "[R] " << QThread::currentThreadId() << QString(": get lock W");
    }
    reader_count += 1;

    qDebug() << "[R] " << QThread::currentThreadId() << QString(": try unlock R");
    r_mutex.unlock();
    qDebug() << "[R] " << QThread::currentThreadId() << QString(": unlock R");

    // actually read here
    QFile data(path);
    auto s_list = QStringList();
    if (data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&data);
        while (!in.atEnd()) {
            s_list.append(in.readLine());
        }
        emit result_ready(s_list.join("|"));
    }

    qDebug() << "[R] " << QThread::currentThreadId() << QString(": try lock R");
    // here we lock r_mutex for decreasing reader_count
    r_mutex.lock();
    qDebug() << "[R] " << QThread::currentThreadId() << QString(": get lock R");

    reader_count -= 1;
    if (reader_count == 0) {
        // now reader_count == 0 means that
        // after reading, there is no more readers,
        // so writers may get the lock.
        qDebug() << "[R] " << QThread::currentThreadId() << QString(": try unlock W");
        w_mutex.unlock();
        qDebug() << "[R] " << QThread::currentThreadId() << QString(": unlock W");
    }

    r_mutex.unlock();
}

void Writer::write()
{
    qDebug() << "[W] " << QThread::currentThreadId() << QString(": try lock W");
    w_mutex.lock();
    qDebug() << "[W] " << QThread::currentThreadId() << QString(": get lock W");

    QFile data(path);
    if (data.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&data);
        out << content;
    }

    qDebug() << "[W] " << QString("Writed: '%1'\n").arg(content);

    qDebug() << "[W] " << QThread::currentThreadId() << QString(": try unlock W");
    w_mutex.unlock();
    qDebug() << "[W] " << QThread::currentThreadId() << QString(": unlock W");
}

Writer::Writer(QMutex &w_mutex, const QString &path, const QString &content)
    : w_mutex(w_mutex),
      path(path),
      content(content)
{}

void ReadWriteManager::add_readers(quint32 n)
{
    for (auto i: range(n)) {
        auto p_worker = QPointer<Reader>(
            new Reader(reader_count, r_mutex, w_mutex, file_path));
        auto p_thread = QPointer<QThread>(new QThread);

        this->reader_wokers.append(p_worker);
        this->reader_threads.append(p_thread);

        p_worker->moveToThread(p_thread);
        connect(p_thread, &QThread::finished,
                p_worker, &Reader::deleteLater);
        connect(this, &ReadWriteManager::all_threads_begin_read,
                p_worker, &Reader::read);
        connect(p_worker, &Reader::result_ready,
                this, &ReadWriteManager::get_one_reader_result);
        p_thread->start();
    }
}

void ReadWriteManager::add_writers(quint32 n)
{
    for (auto i: range(n)) {

        auto content = QString("writer: ") + QString::number(i) + QString("\n");

        auto p_worker = QPointer<Writer>(
            new Writer(w_mutex, file_path, content));
        auto p_thread = QPointer<QThread>(new QThread);

        this->writer_threads.append(p_thread);
        this->writer_wokers.append(p_worker);

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
    emit all_threads_begin_read();
    emit all_threads_begin_write();
}
void ReadWriteManager::get_one_reader_result(const QString &s)
{
    qDebug() << "Read: " + s;
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

Reader::Reader(quint32 &reader_count, QMutex &r_mutex, QMutex &w_mutex, QString path)
    : reader_count(reader_count),
      r_mutex(r_mutex),
      w_mutex(w_mutex),
      path(path)
{}

}