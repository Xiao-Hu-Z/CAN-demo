#include "widget.h"
#include "ui_widget.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QIODevice>

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextCodec>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/CANTestIcon.png"));
    ui->IDEdit->setText(QString("00000000"));

    ui->DataEdit->setText(QString("00 00 00 00 00 00 00 00"));
    MyCANControlThread = new Thread;
    connect(MyCANControlThread,SIGNAL(my_signal(QString)),this,SLOT(deal_my_string(QString)));
    connect(MyCANControlThread,SIGNAL(my_signal1(QString)),this,SLOT(deal_my_string1(QString)));

    //�����ť ѡȡ�ļ�
    connect(ui->pushButton,&QPushButton::clicked,[=](){

    QString path = QFileDialog::getOpenFileName(this,"���ļ�","E:\\USBCAN\\6191124CanTest");

    if(path.isEmpty())
    {
       QMessageBox::warning(this,"����","��ʧ��");
    }
    else
    {
    ui->lineEdit->setText(path);//��·�� ���뵽lineEdit
    QFile file(path); //����·������
    //ָ���򿪷�ʽ(ֻ��)
    //file.open(QIODevice::ReadOnly);
    //file.open(QFileDevice::ReadOnly);
    file.open(QFileDevice::Append);
    QString str = ui->DisplayTextEdit->toPlainText(); // ������ͨ�ı�
    char*  ch;
    QByteArray ba = str.toLatin1();
    ch=ba.data();
    file.write(ch);
    file.close();
    }
    });

}


Widget::~Widget()
{
    delete ui;
}





int Widget::hex_str_to_int(unsigned char *ch)
{
    //qDebug("%c%c",ch[0],ch[1]);
    if(ch[0]>='0'&&ch[0]<='9')
    {
        if(ch[1]>='0'&&ch[1]<='9')
            return (((ch[0]-'0')*16)+(ch[1]-'0'));
        if(ch[1]>='a'&&ch[1]<='f')
            return (((ch[0]-'0')*16)+(ch[1]-'a'+10));
        if(ch[1]>='A'&&ch[1]<='F')
            return (((ch[0]-'0')*16)+(ch[1]-'A'+10));
    }
    else if(ch[0]>='a'&&ch[0]<='f')
    {
        if(ch[1]>='0'&&ch[1]<='9')
            return (((ch[0]-'a'+10)*16)+(ch[1]-'0'));
        if(ch[1]>='a'&&ch[1]<='f')
            return (((ch[0]-'a'+10)*16)+(ch[1]-'a'+10));
        if(ch[1]>='A'&&ch[1]<='F')
            return (((ch[0]-'a'+10)*16)+(ch[1]-'A'+10));
    }
    else if(ch[0]>='A'&&ch[0]<='F')
    {
        if(ch[1]>='0'&&ch[1]<='9')
            return (((ch[0]-'A'+10)*16)+(ch[1]-'0'));
        if(ch[1]>='a'&&ch[1]<='f')
            return (((ch[0]-'A'+10)*16)+(ch[1]-'a'+10));
        if(ch[1]>='A'&&ch[1]<='F')
            return (((ch[0]-'A'+10)*16)+(ch[1]-'A'+10));
    }
}



unsigned int Widget::hex_str_to_uint(char*ch_id)
{
    int sum = 0,num=1;
    for(int i=0; i<8; i++,num=1)
    {
        if('0'==ch_id[i])
            continue;

        for(int j=i;j < 7;j++)
            num = num*16;

        if (ch_id[i]>='0'&&ch_id[i]<='9')
        {
            int n = ch_id[i]-'0';
            sum = sum + n*num;
        }
        else if(ch_id[i]>='a'&&ch_id[i]<='f')
            sum += ((int)(ch_id[i]-'a'+10))*num;
        else if(ch_id[i]>='A'&&ch_id[i]<='F')
            sum += ((int)(ch_id[i]-'A'+10))*num;
    }

    return sum;
}






void Widget::on_StartDevice_clicked()//�����豸
{
    MyCANControlThread->OpenCANThread();
    MyCANControlThread->start();
}


void Widget::on_CloseDevice_clicked()
{
    MyCANControlThread->CloseCANThread();
    MyCANControlThread->stop();
}

void Widget::on_TransmitData_clicked()
{
    QString transmit_str = QStringLiteral("����֡ID:");
    transmit_str.append(ui->IDEdit->text());
    transmit_str.append("    Data:");
    transmit_str.append(ui->DataEdit->text());
    ui->DisplayTextEdit1->append(transmit_str);//���˽���������Ŀ����ôת��һĿ��Ȼ!!!
    QString str_id = ui->IDEdit->text();
    QByteArray ba = str_id.toLatin1();
    char* ch_id = ba.data();
    unsigned int id = 0;

    //���Գ����ܽᾭ�飬���ǲ��ܸ���Ч�ı�̡�
    id = hex_str_to_uint(ch_id);
    QString str_data = ui->DataEdit->text();
    unsigned char data_from_text[8];
    for(int i = 0; i < 8; i++)
    {
        //����char�������ݣ����ǻ�ȡ��ʮ���Ʊ�ʾ��ʽ������һЩ����ֱ�ӽ��ַ���ʽת��Ϊ���ͼ��ɣ�
        data_from_text[i] = hex_str_to_int((unsigned char *)str_data.section(' ',i,i).trimmed().toStdString().c_str());
    }
    MyCANControlThread->TransmitCANThread(id,(unsigned char *)data_from_text);
}

void Widget::deal_my_string(QString str)
{
    ui->DisplayTextEdit->append(str);
   // ui->DisplayTextEdit->append(str);
}

void Widget::deal_my_string1(QString str)
{
    ui->DisplayTextEdit1->append(str);
   // ui->DisplayTextEdit->append(str);
}
