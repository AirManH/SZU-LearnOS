#ifndef SETTING_DIALOG_H
#define SETTING_DIALOG_H

#include <QDialog>
#include <ui_setting_dialog.h>

class setting_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit setting_dialog(QDialog *parent = nullptr);
    ~setting_dialog();

private:
    Ui::setting_dialog ui;
    quint32 reader_number;
    quint32 writer_number;

private slots:
    void when_set_reader_number(int n);
    void when_set_writer_number(int n);
    void when_ok_button_click();

public slots:
    void log(QString s);
    void clear_log();

signals:
    void confirm_numbers(quint32 w, quint32 r);
};

#endif // SETTING_DIALOG_H
