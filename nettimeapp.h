#ifndef NETTIMEAPP_H
#define NETTIMEAPP_H

#include <QObject>
#include "netinet/in.h"
#include "time.h"
#include "common.h"
#include "netreceiver.h"
#include <QObject>


using namespace std;

//网络时间超时判别 单位：毫秒
#define NOTACTIVETIME  500

struct DateTimeInfo{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int MSecond;
};

class NetTimeApp : public QObject
{
    Q_OBJECT
public:
    explicit NetTimeApp(QObject *parent ,string localIP,string broadcastIP,string sourceIP,int port);

    bool getNetDateTime(DateTimeInfo &dateTimeInfo);

    bool isActive();

    void getSysDateTime(DateTimeInfo &dateTimeInfo);

private:

    NetReceiver* netReciver;

    pthread_t timeRecvThread;

    static void * timeReceiveThread(void *);

    timespec latestUpdateTime;

    DateTimeInfo * currentDateTime;

public slots:
    void onNetTimeUpdated(TimeInformation timeInfo);

signals:
    void Int1HzArrived(DateTimeInfo dateTimeInfo);
    void Int20HzArrived(DateTimeInfo dateTimeInfo);
    void TimePackArrived(DateTimeInfo dateTimeInfo);

};

#endif // NETTIMEAPP_H
