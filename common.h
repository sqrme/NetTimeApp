#include <string>
using namespace std;

#ifndef COMMON_H
#define COMMON_H

#define MAXADJUSTINTVAL  1000
#define MAXPROTECTRANGE  1000

#define PACK_SIZE 20
#define HEAD_SIZE 4

#define TIME_PACK_HEAD "ABCD"

#define VALIDVALUE 0xBB
#define INVALIDVALUE 0x55

#define TIMESERVERCONFIGURE "config.ini"


struct TimeInformation{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int MSecond;
    int Count_1Hz;
    bool Valid_1Hz;
    int Count_2Hz;
    bool Valid_2Hz;
    int Count_5Hz;
    bool Valid_5Hz;
    int Count_10Hz;
    bool Valid_10Hz;
    int Count_20Hz;
    bool Valid_20Hz;
    int Count_50Hz;
    bool Valid_50Hz;
};

union TimePacket{
    char Packet[20];
    struct{
        char Prepacket[4];
        unsigned short Year;
        char Month;
        char Day;
        char Hour;
        char Minute;
        char Second;
        unsigned short MSecond;
        char Valid_1Hz;
        char Valid_2Hz;
        char Valid_5Hz;
        char Valid_10Hz;
        char Valid_20Hz;
        char Valid_50Hz;
    }Time;
};




#endif // COMMON_H
