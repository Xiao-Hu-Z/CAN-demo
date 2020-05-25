#ifndef CANTHREAD_H
#define CANTHREAD_H



#include <QThread>
#include <iostream>
#include <QDebug>
#include <QLibrary>
#include <QTimer>
#include <QObject>
#include "contorlcan.h"

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread();
    void stop();
    void OpenCANThread();
    void CloseCANThread();
    void ReceiveCANThread();
    void TransmitCANThread(unsigned int id, unsigned char *ch);

public:
	//用于连接打开 
    typedef DWORD(__stdcall VCI_OpenDevice)(DWORD,DWORD,DWORD);
    typedef DWORD(__stdcall VCI_ResetCAN)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall VCI_CloseDevice)(DWORD DeviceType,DWORD DeviceInd);
    typedef DWORD(__stdcall VCI_InitCAN)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);
    typedef DWORD(__stdcall VCI_StartCAN)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef ULONG(__stdcall VCI_Transmit)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,ULONG Len);

    typedef ULONG(__stdcall VCI_Receive)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,ULONG Len,INT WaitTime/*=-1*/);
    typedef ULONG(__stdcall VCI_GetReceiveNum)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall VCI_ClearBuffer)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
    typedef DWORD(__stdcall VCI_ReadErrInfo)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo);
    typedef DWORD(__stdcall VCI_ReadCANStatus)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_STATUS pCANStatus);
    typedef DWORD(__stdcall VCI_SetReference)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);
    typedef DWORD(__stdcall VCI_ReadBoardInfo)(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);

    int devtype;//设备类型号
    int devind;//设备索引号
    int res;//保留参数，通常为0
    int canind;//第几路can
    int reftype;//参数类型

protected:
    void run();

private:
    volatile bool stopped;

    VCI_OpenDevice *pOpenDevice;
    VCI_ResetCAN *pResetCAN;
    VCI_CloseDevice *pCloseDevice;
    VCI_InitCAN *pInitCAN;
    VCI_StartCAN *pStartCAN;
    VCI_Transmit *pTransmitCAN;
    VCI_Receive *pReceive;
    VCI_GetReceiveNum *pGetReceiveNum;
    VCI_ClearBuffer *pClearBuffer;
    VCI_ReadErrInfo *pReadErrInfoCAN;
    VCI_ReadCANStatus *pReadCANStatus;
    VCI_SetReference *pSetReference;
    VCI_ReadBoardInfo *pReadBoardInfo;
public:
    QString binToDec(QString strBin);

    //void baocun();



signals:
    void my_signal(QString str);
    void my_signal1(QString str);
};




#endif // CANTHREAD_H
