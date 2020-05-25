#include "canthread.h"


Thread::Thread()
{
    stopped = false;

    devtype=3;//�豸����USBCAN2
    devind=0;//�豸������
    res=0;//ϵͳ����λ��һ��Ϊ0
    canind=0;//CANͨ��0
    reftype=0;//��������
    bool ok;

    VCI_ERR_INFO vei;
    VCI_CAN_OBJ preceive[100];
    VCI_CAN_OBJ psend;
    int baud=0x10000000;//�����й����ݻ�������ַ��ָ��
    //�����
    QLibrary lib("usbcan.dll");
    if(true==lib.load())
    {

        pOpenDevice = (VCI_OpenDevice *)lib.resolve("VCI_OpenDevice");//�ǲ���ֱ��ʹ��VCI_InitCAN�����ģ����뵼��
        pCloseDevice = (VCI_CloseDevice *)lib.resolve("VCI_CloseDevice");
//        pInitCAN = (VCI_InitCAN *)lib.resolve("VCI_InitCAN");//���ֵ���dll�к����ķ�ʽ
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
    int baud=0x10000000;//�����й����ݻ�������ַ��ָ��

    //����
    ULONG res = 0;

    //��ȡ�������н��յ���δ����ȡ��֡��.devtype=3:�豸����USBCAN1;devind=0:�豸������;canind=0:CANͨ��0
    res=pGetReceiveNum(devtype,devind,canind);

    if(res<=0)//δ���յ����ݻ����ʧ��
    {
        //CANIndex �ڼ�· CANע����Ҫ��ȡ�豸�����ʱ�򣬴˲���Ӧ����Ϊ��1��
        if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR)
        {
            qDebug()<<QStringLiteral("���մ�����:")<<QString::number(vei.ErrCode,16);//��ʮ�����ƽ���������ʾ����
        }
    }
    else
    {
        //(�豸���ͺ�,�豸������,�ڼ�· CAN,�������յ�����֡�������ָ��,�ȴ���ʱʱ�䣬�Ժ���Ϊ��λ)
        //����ʵ�ʶ�ȡ����֡�����������ֵΪ 0xFFFFFFFF�����ʾ��ȡ����ʧ�ܣ��д������������VCI_ReadErrInfo ��������ȡ������
        res=pReceive(devtype,devind,canind,preceive,50,200);//��������ȡ50֡���ȴ���ʱΪ200ms

        qDebug()<<"Frame ID"<<res;
        for(int i=0;i<res;i++)
        {
            if(res==4294967295)//4294967295=0xFFFFFFFF�������ȡ����
            {
                if(pReadErrInfoCAN(devtype,devind,canind,&vei)!=STATUS_ERR)
                {
                    qDebug()<<"Read Data failed"<<"Error Data:"<<QString::number(vei.ErrCode,16);
                }
            }
            if(preceive[i].ID>=1280&&preceive[i].ID<=1343)
            {

            QString receive_str1 = QStringLiteral("����    ֡ID:00000");
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
    //����
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
        qDebug()<<QStringLiteral("����֡����")<<Tr;
    }
}

void Thread::OpenCANThread()
{
    bool ok;
    VCI_ERR_INFO vei;
    VCI_CAN_OBJ preceive[100];
    VCI_CAN_OBJ psend;
    int baud=0x10000000;//�����й����ݻ�������ַ��ָ��
    //���豸
    if(pOpenDevice(devtype,devind,res)==STATUS_ERR )//����ֵ0��ʾʧ�ܣ�1λ�ɹ�,��֮ǰ�ǵó�ʼ��
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

    //��ʼ��
    VCI_INIT_CONFIG init_config;
    init_config.Mode=0;//����ģʽ��1Ϊֻ��ģʽ
    init_config.Filter=1;//�˲���ʽ�����˲�
    //��ʱ��0��1����������canͨ�Ų����ʵ�
    init_config.Timing0=0x00;//��ʱ��0
    init_config.Timing1=0x1c;//��ʱ��1
    init_config.AccCode=0x10000000;//������
    init_config.AccMask=0xFFFFFFFF;//������
    //�ȴ��豸���ٽ��г�ʼ��
    if(pInitCAN(devtype,devind,canind,&init_config)==STATUS_ERR){
        qDebug("Init Error");
        pCloseDevice(devtype,devind);
        return;
    }else
        qDebug()<<"Init successed";


    //��ȡ�豸��Ϣ
    VCI_BOARD_INFO vbi;
    if(pReadBoardInfo(devtype,devind,&vbi)!=STATUS_ERR){
        qDebug()<<QStringLiteral("CANͨ������")<<vbi.can_Num;
        qDebug()<<QStringLiteral("Ӳ���汾��:")<<vbi.hw_Version;
        qDebug()<<QStringLiteral("�ӿڿ�汾�ţ�")<<vbi.in_Version;
        qDebug()<<QStringLiteral("�жϺ�")<<vbi.irq_Num;
        qDebug()<<QStringLiteral("canͨ��������")<<vbi.can_Num;
    }
    //�����豸����(��Ҫ������ͨ�Ų����ʣ�USBCAN�豸�ò������ò������ͣ�NETUDP��NETTCP����Ҫ�����ұ����ڳ�
    if(pSetReference(devtype,devind,canind,reftype,&baud)==STATUS_ERR){
        qDebug("set reference error");
        pCloseDevice(devtype,devind);
        return;
    }
    //���������
    pClearBuffer(devtype,devind,canind);

    //�����豸һ��canͨ��
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
