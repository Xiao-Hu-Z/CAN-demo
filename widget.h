#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <iostream>
#include <QLibrary>
#include <QDebug>
#include <QTimer>
#include "contorlcan.h"
#include "canthread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
public:
    int hex_str_to_int(unsigned char *ch);
    unsigned int hex_str_to_uint(char *ch_id);



public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_StartDevice_clicked();

    void on_CloseDevice_clicked();

    void on_TransmitData_clicked();

    void deal_my_string(QString str);
    void deal_my_string1(QString str);

private:
    Ui::Widget *ui;
    Thread *MyCANControlThread;

};

#endif // WIDGET_H
