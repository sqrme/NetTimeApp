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
#include "netreceiver.h"
#include "iostream"
#include "common.h"

using namespace std;


NetReceiver::NetReceiver(QObject *parent,string localIP, string receiveIP, string sourceIP,int port): QObject(parent)
{
    this->socketReady=false;
    this->timeInfo=new TimeInformation{0,0,0,0,0,0,0,0,false,0,false,0,false,0,false,0,false,0,false};
    this->sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(this->sockfd<0){
        cout<<"socket打开错误！"<<endl;
    }else{
        int reuse=1;
        if(setsockopt(this->sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse))<0){
            cout<<"设定组播属性错误！"<<endl;
            close(sockfd);
        }else{
            memset((char *)&(this->localSock),0,sizeof(this->localSock));
            this->localSock.sin_family=AF_INET;
            this->localSock.sin_port=htons(port);
            this->localSock.sin_addr.s_addr=INADDR_ANY;
            if(bind(this->sockfd,(struct sockaddr*)&(this->localSock),sizeof(this->localSock))){
                cout<<"绑定地址错误！"<<endl;
                close(sockfd);
            }else{
                this->group.imr_interface.s_addr=inet_addr(localIP.c_str());
                this->group.imr_multiaddr.s_addr=inet_addr(receiveIP.c_str());
                this->group.imr_sourceaddr.s_addr=inet_addr(sourceIP.c_str());
                if(setsockopt(this->sockfd,IPPROTO_IP,IP_ADD_SOURCE_MEMBERSHIP,(char*)&(this->group),sizeof(this->group))<0){
                    cout<<"加入组播错误！"<<endl;
                    close(this->sockfd);
                }else{
                    this->timeOut.tv_sec=2;
                    this->timeOut.tv_usec=0;
                    this->socketReady=true;
                }
            }
        }
    }
}

void NetReceiver::StartReceiving(){
    if(this->socketReady){
        ////////////////////////////////
        int result;
        fd_set readfds,testfds;
        FD_ZERO(&(readfds));
        FD_SET(this->sockfd,&readfds);
        while(1){
            int readLen;
            int socklen=sizeof(this->localSock);
            testfds=readfds;
            timeval temp_timeout=this->timeOut;
            /*
             * select will modify timeout value ,so pass a copy value.....
            */
            result=select(FD_SETSIZE,&testfds,(fd_set *)0,(fd_set*)0, &temp_timeout);
            if(result<0){
                cout<<"socket接收错误！"<<endl;
                return;
            }
            switch(result){
            case 0://网络超时...
                cout<<"网络超时"<<endl;
                break;
            case -1://网络错误....
                cout<<"网络错误"<<endl;
                break;
            default://接收到的信息可用....
                readLen = recvfrom(this->sockfd,readBuffer,sizeof(readBuffer),0,(struct sockaddr *) &this->localSock,(socklen_t *)&(socklen));
                if(readLen==PACK_SIZE && (strncmp(readBuffer,TIME_PACK_HEAD,HEAD_SIZE)==0)){
                    ParseTimePacket();
                }
                break;
            }
        }
    }
}

void NetReceiver::ParseTimePacket(){
    char valid=(char) 0xBB;
    TimePacket * timePacket=(TimePacket *) this->readBuffer;
    this->timeInfo->Year=(int)timePacket->Time.Year;
    this->timeInfo->Month=(int )timePacket->Time.Month;
    this->timeInfo->Day=(int)timePacket->Time.Day;
    this->timeInfo->Hour=(int)timePacket->Time.Hour;
    this->timeInfo->Minute=(int)timePacket->Time.Minute;
    this->timeInfo->Second=(int)timePacket->Time.Second;
    this->timeInfo->MSecond=(int)timePacket->Time.MSecond;
    this->timeInfo->Valid_1Hz=false;
    this->timeInfo->Valid_2Hz=false;
    this->timeInfo->Valid_5Hz=false;
    this->timeInfo->Valid_10Hz=false;
    this->timeInfo->Valid_20Hz=false;
    this->timeInfo->Valid_50Hz=false;
    if(timePacket->Time.Valid_1Hz==valid){
        this->timeInfo->Valid_1Hz=true;
        this->timeInfo->Count_1Hz++;
    }
    if(timePacket->Time.Valid_2Hz==valid){
        this->timeInfo->Valid_2Hz=true;
        this->timeInfo->Count_2Hz++;
    }
    if(timePacket->Time.Valid_5Hz==valid){
        this->timeInfo->Valid_5Hz=true;
        this->timeInfo->Count_5Hz++;
    }
    if(timePacket->Time.Valid_10Hz==valid){
        this->timeInfo->Valid_10Hz=true;
        this->timeInfo->Count_10Hz++;
    }
    if(timePacket->Time.Valid_20Hz==valid){
        this->timeInfo->Valid_20Hz=true;
        this->timeInfo->Count_20Hz++;
    }
    if(timePacket->Time.Valid_50Hz==valid){
        this->timeInfo->Valid_50Hz=true;
        this->timeInfo->Count_50Hz++;
    }
    emit NetTimeUpdated(*this->timeInfo);
}
