#include <read_write_manager.hpp>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "ok\n";

    auto* rwm = new los::ReadWriteManager(QCoreApplication::instance(), QString("./a.txt"));
    rwm->add_readers(5);
    rwm->add_writers(5);
    rwm->run();

    return a.exec();
}