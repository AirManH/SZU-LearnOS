#include "setting_dialog.hpp"
#include <QDebug>
#include <QPushButton>
#include <QObject>

setting_dialog::setting_dialog(QDialog *parent)
    : QDialog(parent),
      reader_number(0),
      writer_number(0)
{
    ui.setupUi(this);
    connect(ui.cancel_pushButton, &QPushButton::clicked,
            this, &QDialog::close);

    connect(ui.ok_pushButton, &QPushButton::clicked,
            this, &setting_dialog::when_ok_button_click);

    // for reason to write qOverload<int> here
    // see https://stackoverflow.com/a/16795664
    connect(ui.reader_spinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &setting_dialog::when_set_reader_number);
    connect(ui.writer_spinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &setting_dialog::when_set_writer_number);
}

setting_dialog::~setting_dialog()
{}

void setting_dialog::when_set_reader_number(int n) {
    this->reader_number = n;
}

void setting_dialog::when_set_writer_number(int n) {
    this->writer_number = n;
}

void setting_dialog::when_ok_button_click(){
    emit confirm_numbers(writer_number, reader_number);
}

void setting_dialog::log(QString s){
    ui.logger_textBrowser->append(s);
}
void setting_dialog::clear_log()
{
    this->ui.logger_textBrowser->clear();
}
