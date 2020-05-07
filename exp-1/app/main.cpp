#include <read_write_manager.hpp>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPushButton button("hello", 0);
    button.resize(100, 30);
    button.show();

    auto* rwm = new los::ReadWriteManager(QString("./a.txt"));
    rwm->add_readers(5);
    rwm->add_writers(5);
    rwm->run();



    return a.exec();
}