// C++ program to illustrate the client application in the
// socket programming
#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "../types.h"
#include "CClient.hpp"

#define     CLIENT_CONFIG       "Client_config.cfg"
#define     DEFAULT_PORT        8080u
#define     DEFAULT_SERV_IP     "127.0.0.1"
#define     DEFAULT_X           5.0

using namespace std;


void getConfigData();

uint16_t    Serv_Port = DEFAULT_PORT;
char        Serv_IP[32] = {0};
double      Interval_X = DEFAULT_X;


int main()
{
    getConfigData();
    cout << "Serv_Port = "<< Serv_Port << endl;
    cout << "Server IP = " << Serv_IP << endl;
    cout << "Interval = [-" << Interval_X  << ", " << Interval_X << "]" << endl;

    CClient client(Serv_Port, Serv_IP);
    if (true == client.isReady())
    {
        client.handleRequest(Interval_X);   // the same as eDouble, 1000000u
        _SLEEP_MS(500)
        client.handleExtRequest(Interval_X, eDouble, 100000u);
        _SLEEP_MS(500)
        client.handleExtRequest(Interval_X, eLongDouble, 100000u);
        _SLEEP_MS(500)
        client.handleExtRequest(Interval_X, eFloat, 100000u);
    }
    return 0;
}


void getConfigData()
{
    ifstream fl;
    fl.open(CLIENT_CONFIG);
    if (true == fl.is_open()) 
    {
        try
        {
            fl >> Serv_Port;
            fl >> Serv_IP;
            fl >> Interval_X;
        }
        catch(...)
        {
            fl.close();
            Serv_Port = DEFAULT_PORT;
            Interval_X = DEFAULT_X;
            memcpy(Serv_IP, DEFAULT_SERV_IP, sizeof(DEFAULT_SERV_IP));
            cout << "An exception has been generated on file reading." << endl
                 << "The default config will be used" << endl;
        }
        fl.close();
    }
    else
    {
        memcpy(Serv_IP, DEFAULT_SERV_IP, sizeof(DEFAULT_SERV_IP));
        cout << "File " << CLIENT_CONFIG << " cannot be open." << endl 
             << "The default config will be used" << endl;
    }
}

