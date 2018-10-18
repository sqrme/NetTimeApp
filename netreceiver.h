#include <string>
#include "sys/socket.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include "netinet/in.h"
#include "netdb.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "common.h"
#include <QObject>

using namespace std;

#ifndef NETRECEIVER_H
#define NETRECEIVER_H


class NetReceiver: public QObject
{
    Q_OBJECT
private:
    struct sockaddr_in localSock;
    ip_mreq_source group;
    int sockfd;
    bool socketReady;
    TimeInformation *timeInfo;
    char readBuffer[1000];
    timeval timeOut;
    void ParseTimePacket();
public:
    NetReceiver(QObject *parent,string localIP, string receiveIP,string sourceIP, int port);
    void StartReceiving();

signals:
    void NetTimeUpdated(TimeInformation timeInfo);
};

#endif // NETRECEIVER_H
