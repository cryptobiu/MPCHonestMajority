//
// Created by hila on 18/09/16.
//

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "stdlib.h"
#include "../../workspace/paho/src/MQTTClient.h"
#include "TParty.h"
#include "VDM.h"
#include "TGate.h"
#include "ArithmeticCircuit.h"
#include <atomic>

extern void ConnectHandler(const char *topicName, MQTTClient_message *&message, const string &str);
extern int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
extern void connlost(void *context, char *cause);
extern void delivered(void *context, MQTTClient_deliveryToken dt);

class Communication {
private:
    static bool m_instanceFlag;
    static Communication* m_single;
    Communication(int n, int id);

public:
    static Communication* getInstance(int numOfParties, int id);
    static Communication* getInstance();

    int PARTYID, N, T, M;

    // two of messages can come one by one
    std::atomic<int> countRoundRecieve;
    std::atomic<int> countXRecieve;
    std::atomic<int> countYRecieve;
    std::atomic<int> countRoundFinish;
    std::atomic<int> countPartOfPoly;
    std::atomic<int> countDoubleShare;
    std::atomic<int> countGateShareArr;
    std::atomic<int> countLast;
    std::atomic<int>  countRecon;
    std::atomic<int>  count10;

    std::atomic<int> countThis;
    std::atomic<int>  countNext;
    vector<string> vecThis, vecNext;


    vector<int> vecConn;
    vector<TFieldElement> vecRecX;
    vector<string> vec;
    vector<string> vec10;
    vector<string> vecRecForCheck, vecSendPartOfPoly, vecDoubleShare, vecGateShareArr, vecLast, vecRecon;
    volatile MQTTClient_deliveryToken deliveredtoken;
    MQTTClient_connectOptions m_conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient m_client;
    int m_rc;
    int m_ch;
    char** topic = new char*[9];
    int len;
    MQTTClient_message m_pubmsg;
    MQTTClient_deliveryToken m_token;
    string s3;
    string s1;
    string s2, s4, s5, s6, s7, s8, s9;



    void ConnectionToServer(const MQTTClient &m_client, const string &s, string &myTopicForMessage, MQTTClient_message &m_pubmsg,
                                     MQTTClient_deliveryToken &m_token);
    void SendTheResult(string &myMessage, MQTTClient const &m_client, string &myTopicForMessage,
                       MQTTClient_message &m_pubmsg,
                       MQTTClient_deliveryToken &m_token, const string &s, const vector<string> &buffers, vector<string> &recBufs);
    void SendXVectorToAllParties(string &myMessage, MQTTClient const &m_client, char *const *topic,
                                 string &myTopicForMessage, MQTTClient_message &m_pubmsg,
                                 MQTTClient_deliveryToken &m_token, string &s, vector<string> &recBufs);
    void roundFunction(vector<string> &sendBufs, vector<string> &recBufs);

    virtual ~Communication();
    void sendPartOfPoly(vector<string> &sendBufs,vector<string> &recBufs);
    void send(const string &myTopicForMessage, const string &myMessage);
    void sendDoubleShare(vector<string> &sendBufs,vector<string> &recBufs);
    void sendGateShareArr(vector<string> &sendBufs,vector<string> &recBufs);
    void Lastsend(vector<string> &sendBufs,vector<string> &recBufs);
    void sendRecon(vector<string> &sendBufs,vector<string> &recBufs);
    void roundfunction(vector<string> &sendBufs, vector<string> &recBufs, int num);
    void roundfunction10(vector<string> &sendBufs,vector<string> &recBufs);
};


#endif /* CONTEXT_H_ */
