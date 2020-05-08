#include <read_write_manager.hpp>
#include "setting_dialog.hpp"
#include <QObject>
#include <QtWidgets/QApplication>

void foo(quint32 w, quint32 r){
    qDebug() << w << " " << r << Qt::endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    setting_dialog dialog;
    auto* rwm = new los::ReadWriteManager(QString("./a.txt"));

    QObject::connect(&dialog, &setting_dialog::confirm_numbers,
                     rwm, &los::ReadWriteManager::set_writers_and_readers);

    QObject::connect(rwm, &los::ReadWriteManager::number_changed,
                     rwm, &los::ReadWriteManager::run);

    QObject::connect(&dialog, &setting_dialog::confirm_numbers,
                     &dialog, &setting_dialog::clear_log);

    QObject::connect(rwm, &los::ReadWriteManager::to_info,
                     &dialog, &setting_dialog::log);

    dialog.show();



    return a.exec();
}
