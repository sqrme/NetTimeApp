#include "nettimeapp.h"
#include "iostream"
#include <string>
#include "sys/socket.h"
#include "arpa/inet.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "netinet/in.h"
#include "netdb.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "time.h"

using namespace std;

NetTimeApp::NetTimeApp(QObject *parent ,string localIP,string broadcastIP,string sourceIP,int port) : QObject(parent)
{
    currentDateTime=new DateTimeInfo{0,0,0,0,0,0,0};
    latestUpdateTime.tv_sec=0;
    qRegisterMetaType<TimeInformation>("TimeInformation");
    netReciver=new NetReceiver(this,localIP,broadcastIP,sourceIP,port);
    connect(netReciver,SIGNAL(NetTimeUpdated(TimeInformation)),this,SLOT(onNetTimeUpdated(TimeInformation)));
    int res=pthread_create(&this->timeRecvThread,NULL,timeReceiveThread,this->netReciver);
    if(res!=0){
        cout<<"创建线程错误！"<<endl;
    }
}

void* NetTimeApp::timeReceiveThread(void *args){
    int res=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
        if(res!=0){
            cout<<"线程可取消属性配置错误。"<<endl;
            exit(EXIT_FAILURE);
        }

        res=pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
        if(res!=0){
            cout<<"线程可取消类型配置错误!"<<endl;
            exit(EXIT_FAILURE);
        }
        NetReceiver * netReciver=(NetReceiver * )args;
        netReciver->StartReceiving();
}

void NetTimeApp::onNetTimeUpdated(TimeInformation timeInfo){
    currentDateTime->Year=timeInfo.Year;
    currentDateTime->Month=timeInfo.Month;
    currentDateTime->Day=timeInfo.Day;
    currentDateTime->Hour=timeInfo.Hour;
    currentDateTime->Minute=timeInfo.Minute;
    currentDateTime->Second=timeInfo.Second;
    currentDateTime->MSecond=timeInfo.MSecond;
    clock_gettime(CLOCK_REALTIME,&this->latestUpdateTime);
    emit TimePackArrived(*currentDateTime);
    if(timeInfo.Valid_20Hz){
        emit Int20HzArrived(*currentDateTime);
    }
    if(timeInfo.Valid_1Hz){
        emit Int1HzArrived(*currentDateTime);
    }
}

bool NetTimeApp::isActive(){
    bool ret=false;
    if(currentDateTime->Year==0){
        ret=false;
    }else{
        timespec now;
        clock_gettime(CLOCK_REALTIME,&now);
        long delayTime=(now.tv_sec-latestUpdateTime.tv_sec)*1000+(now.tv_nsec-latestUpdateTime.tv_nsec)/1000000;
        if(delayTime<NOTACTIVETIME){
            ret=true;
        }else{
            ret=false;
        }
    }
    return ret;
}


bool NetTimeApp::getNetDateTime(DateTimeInfo& dateTimeInfo){
    bool ret=false;
    if(isActive()){
        dateTimeInfo.Year=currentDateTime->Year;
        dateTimeInfo.Month=currentDateTime->Month;
        dateTimeInfo.Day=currentDateTime->Day;
        dateTimeInfo.Hour=currentDateTime->Hour;
        dateTimeInfo.Minute=currentDateTime->Minute;
        dateTimeInfo.Second=currentDateTime->Second;
        dateTimeInfo.MSecond=currentDateTime->MSecond;
        ret=true;
    }
    return ret;
}


void NetTimeApp::getSysDateTime(DateTimeInfo &dateTimeInfo){
    struct tm * tm_ptr;
    time_t now;
    struct timeval tv;
    int msec=-1;
    time(&now);
    tm_ptr=localtime(&now);
    gettimeofday(&tv,NULL);
    msec=(int)(tv.tv_usec/1000);
    dateTimeInfo.Year=tm_ptr->tm_year+1900;
    dateTimeInfo.Month=tm_ptr->tm_mon+1;
    dateTimeInfo.Day=tm_ptr->tm_mday;
    dateTimeInfo.Hour=tm_ptr->tm_hour;
    dateTimeInfo.Minute=tm_ptr->tm_min;
    dateTimeInfo.Second=tm_ptr->tm_sec;
    dateTimeInfo.MSecond=msec;
}



