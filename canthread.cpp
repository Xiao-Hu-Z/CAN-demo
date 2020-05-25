#include "canthread.h"


Thread::Thread()
{
    stopped = false;

    devtype=3;//设备类型USBCAN2
    devind=0;//设备索引号
    res=0;//系统保留位，一般为0
    canind=0;//CAN通道0
    reftype=0;//参数类型
    bool ok;

    VCI_ERR_INFO vei;
    VCI_CAN_OBJ preceive[100];
    VCI_CAN_OBJ psend;
    int baud=0x10000000;//参数有关数据缓冲区地址首指针
    //导入库
    QLibrary lib("usbcan.dll");
    if(true==lib.load())
    {

        pOpenDevice = (VCI_OpenDevice *)lib.resolve("VCI_OpenDevice");//是不能直接使用VCI_InitCAN函数的，必须导出
        pCloseDevice = (VCI_CloseDevice *)lib.resolve("VCI_CloseDevice");
//        pInitCAN = (VCI_InitCAN *)lib.resolve("VCI_InitCAN");//两种导出dll中函数的方式
        pStartCAN = (VCI_StartCAN *)lib.resolve("VCI_StartCAN");
        pTransmitCAN = (VCI_Transmit *)lib.resolve("VCI_Transmit");
        pReceive = (VCI_Receive *)lib.resolve("VCI_Receive");
        pGetReceiveNum = (VCI_GetReceiveNum *)lib.resolve("VCI_GetReceiveNum");
        pClearBuffer = (VCI_ClearBuffer *)lib.resolve("VCI_ClearBuffer");
        pReadErrInfoCAN = (VCI_ReadErrInfo*)lib.resolve("VCI_ReadErrInfo");
        pResetCAN=(VCI_ResetCAN *)lib.resolve("VCI_ResetCAN");
        pSetReference=(VCI_SetReference *)lib.resolve("VCI_SetReference");
        pReadBoardInfo=(VCI_ReadBoardInfo *)lib.resolve("VCI_ReadBoardInfo");
        pReadCANStatus=(VCI_ReadCANStatus *)lib.resolve("VCI_ReadCANStatus");
        qDebug()<<"usbcan.dll load ok";
    }

    else
        qDebug()<<"usbcan.dll load failed";


}

void Thread::run()
{
    while(!stopped)
    {
        ReceiveCANThread();
    }
    stopped = false;
}

void Thread::stop()
{
    stopped = true;
}






QString Thread::binToDec(QString strBin){  
    QString decimal;
    int nDec = 0,nLen;
    int i,j,k;
    nLen = strBin.length();
    for(i=0;i<nLen;i++){
        if(strBin[nLen-i-1]=="0")
            continue;
        else{
            k=1;
            for(j=0;j<i;j++)
                k=k*2;
            nDec+=k;
        }
    }
    decimal = QString::number(nDec);
    return decimal;
}

void Thread::ReceiveCANThread()
{
    bool ok;
    VCI_ERR_INFO vei;
    VCI_CAN_OBJ preceive[1000];
    VCI_CAN_OBJ psend;
    int baud=0x10000000;//参数有关数据缓冲区地址首指针

    //接收
    ULONG res = 0;

    //获取缓冲区中接收但尚未被读取的帧数.devtype=3:设备类型USBCAN1;devind=0:设备索引号;canind=0:CAN通道0
    res=pGetReceiveNum(devtype,devind,canind);

    if(res<=0)//未接收到数据或操作失败
    {
        //CANIndex 第几路 CAN注：当要读取设备错误的时候，此参数应该设为－1。
        if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR)
        {
            qDebug()<<QStringLiteral("接收错误码:")<<QString::number(vei.ErrCode,16);//以十六进制将错误码显示出来
        }
    }
    else
    {
        //(设备类型号,设备索引号,第几路 CAN,用来接收的数据帧数组的首指针,等待超时时间，以毫秒为单位)
        //返回实际读取到的帧数。如果返回值为 0xFFFFFFFF，则表示读取数据失败，有错误发生，请调用VCI_ReadErrInfo 函数来获取错误码
        res=pReceive(devtype,devind,canind,preceive,50,200);//单次最多读取50帧，等待超时为200ms

        qDebug()<<"Frame ID"<<res;
        for(int i=0;i<res;i++)
        {
            if(res==4294967295)//4294967295=0xFFFFFFFF，代表读取错误
            {
                if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR)
                {
                    qDebug()<<"Read Data failed"<<"Error Data:"<<QString::number(vei.ErrCode,16);
                }
            }
            if(preceive[i].ID>=1280&&preceive[i].ID<=1343)
            {

            QString receive_str1 = QStringLiteral("发送    帧ID:00000");
            receive_str1.append(QString::number(preceive[i].ID,16));
            receive_str1.append("    Data:");
            receive_str1.append(QString::number(preceive[i].Data[0],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[1],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[2],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[3],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[4],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[5],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[6],16));
            receive_str1.append(" ");
            receive_str1.append(QString::number(preceive[i].Data[7],16));
            emit my_signal1(receive_str1);
            }



            QString receive_str = QStringLiteral("ID:0x");

            if(preceive[i].ID>=1280&&preceive[i].ID<=1343)
            {
            receive_str.append(QString::number(preceive[i].ID,16));

            QString str1=QString::number(preceive[i].Data[0],2);
            QString strNew1 = QString("%1").arg(str1.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew1);
            //receive_str.append(" ");

            QString str2=QString::number(preceive[i].Data[1],2);
            QString strNew2 = QString("%1").arg(str2.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew2);
            //receive_str.append(" ");

            QString str3=QString::number(preceive[i].Data[2],2);
            QString strNew3 = QString("%1").arg(str3.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew3);
            //receive_str.append(" ");

            QString str4=QString::number(preceive[i].Data[3],2);
            QString strNew4 = QString("%1").arg(str4.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew4);
            //receive_str.append(" ");

            QString str5=QString::number(preceive[i].Data[4],2);
            QString strNew5 = QString("%1").arg(str5.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew5);
            //receive_str.append(" ");

            QString str6=QString::number(preceive[i].Data[5],2);
            QString strNew6 = QString("%1").arg(str6.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew6);
            //receive_str.append(" ");

            QString str7=QString::number(preceive[i].Data[6],2);
            QString strNew7 = QString("%1").arg(str7.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew7);
            //receive_str.append(" ");

            QString str8=QString::number(preceive[i].Data[7],2);
            QString strNew8 = QString("%1").arg(str8.toInt(), 8, 10, QLatin1Char('0'));
            //receive_str.append(strNew8);
            //receive_str.append(" ");

            receive_str.append("    range:");
            QString range=strNew3[5];
            range.append(strNew3[6]);
            range.append(strNew3[7]);
            range.append(strNew4);
            //qDebug()<<range;
            QString decimal_range = binToDec(range);
            //qDebug()<<decimal_range;
            double juli=decimal_range.toFloat()*0.1;
            //qDebug()<<juli;
            receive_str.append(QString::number(juli));
            //receive_str.append(decimal_range);
            //receive_str.append(range);

            receive_str.append("    range_rate:");
            QString range_rate=strNew7[2];
            range_rate.append(strNew7[3]);
            range_rate.append(strNew7[4]);
            range_rate.append(strNew7[5]);
            range_rate.append(strNew7[6]);
            range_rate.append(strNew7[7]);
            range_rate.append(strNew8);
            //qDebug()<<range_rate;
            QString decimal_range_rate = binToDec(range_rate);
            //qDebug()<<decimal_range_rate;
            double sudu=decimal_range_rate.toFloat()*0.01;
            receive_str.append(QString::number(sudu));
            //receive_str.append(decimal_range_rate);
            //receive_str.append(range_rate);

            receive_str.append("    angle:");
            QString angle=strNew3[3];
            angle.append(strNew3[4]);
            angle.append(strNew3[5]);
            angle.append(strNew3[6]);
            angle.append(strNew3[7]);
            angle.append(strNew4[0]);
            angle.append(strNew4[1]);
            angle.append(strNew4[2]);
            angle.append(strNew4[3]);
            angle.append(strNew4[4]);
            //qDebug()<<angle;
            QString decimal_angle = binToDec(angle);
            //qDebug()<<decimal_angle;
            double jiaodu=decimal_angle.toFloat()*0.1;
            receive_str.append(QString::number(jiaodu));
            //receive_str.append(decimal_angle);
            //receive_str.append(angle);






            //emit my_signal(decimal_range);
            emit my_signal(receive_str);



//            QString s="0x";
//            qDebug()<<QStringLiteral("?ID??")<<s.append(QString::number(preceive[i].ID,2));
//            qDebug()<<QStringLiteral("??????")<<QString::number(preceive[i].Data[0],16)<<QString::number(preceive[i].Data[1],2)
//                    <<QString::number(preceive[i].Data[2],2)<<QString::number(preceive[i].Data[3],2)
//                    <<QString::number(preceive[i].Data[4],2)<<QString::number(preceive[i].Data[5],2)
//                    <<QString::number(preceive[i].Data[6],2)<<QString::number(preceive[i].Data[7],2);
//            qDebug()<<QStringLiteral("??????")<<preceive[i].DataLen;
            }
        }
    }

    sleep(2);
}

void Thread::TransmitCANThread(unsigned int id,unsigned char *ch)
{
    VCI_CAN_OBJ psend;
    //发送
    ULONG Tr;
    psend.ID=id;
    psend.SendType=0;
    psend.RemoteFlag=0;
    psend.ExternFlag=0;
    psend.DataLen=8;
    for (int i=0; i < 8; i++)
    {
        psend.Data[i]=ch[i];
    }

    Tr=pTransmitCAN(devtype,devind,canind,&psend,1);
    if(Tr!=STATUS_ERR){
        qDebug()<<QStringLiteral("发送帧数：")<<Tr;
    }
}

void Thread::OpenCANThread()
{
    bool ok;
    VCI_ERR_INFO vei;
    VCI_CAN_OBJ preceive[100];
    VCI_CAN_OBJ psend;
    int baud=0x10000000;//参数有关数据缓冲区地址首指针
    //打开设备
    if(pOpenDevice(devtype,devind,res)==STATUS_ERR )//返回值0表示失败，1位成功,打开之前记得初始化
    {
        if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR)
        {
        qDebug()<<"Open failed"<<QString::number(vei.ErrCode,16);
        }
        else
            qDebug()<<"error";
        return;
    }else
        qDebug()<<"open successed";

    //初始化
    VCI_INIT_CONFIG init_config;
    init_config.Mode=0;//正常模式，1为只听模式
    init_config.Filter=1;//滤波方式，单滤波
    //定时器0和1适用于配置can通信波特率的
    init_config.Timing0=0x00;//定时器0
    init_config.Timing1=0x1c;//定时器1
    init_config.AccCode=0x10000000;//验收码
    init_config.AccMask=0xFFFFFFFF;//屏蔽码
    //先打开设备，再进行初始化
    if(pInitCAN(devtype,devind,canind,&init_config)==STATUS_ERR){
        qDebug("Init Error");
        pCloseDevice(devtype,devind);
        return;
    }else
        qDebug()<<"Init successed";


    //读取设备信息
    VCI_BOARD_INFO vbi;
    if(pReadBoardInfo(devtype,devind,&vbi)!=STATUS_ERR){
        qDebug()<<QStringLiteral("CAN通道数：")<<vbi.can_Num;
        qDebug()<<QStringLiteral("硬件版本号:")<<vbi.hw_Version;
        qDebug()<<QStringLiteral("接口库版本号：")<<vbi.in_Version;
        qDebug()<<QStringLiteral("中断号")<<vbi.irq_Num;
        qDebug()<<QStringLiteral("can通道个数：")<<vbi.can_Num;
    }
    //设置设备参数(主要是设置通信波特率，USBCAN设备用不着设置参数类型，NETUDP和NETTCP才需要，而且必须在初
    if(pSetReference(devtype,devind,canind,reftype,&baud)==STATUS_ERR){
        qDebug("set reference error");
        pCloseDevice(devtype,devind);
        return;
    }
    //清除缓冲区
    pClearBuffer(devtype,devind,canind);

    //启动设备一个can通道
    if(pStartCAN(devtype,devind,canind)==STATUS_ERR){
        qDebug()<<"start fail";
        pCloseDevice(devtype,devind);
        return;
    }else
        qDebug()<<"start successed";
}




void Thread::CloseCANThread()
{
    pCloseDevice(devtype,devind);
    qDebug()<<"closed";
}
