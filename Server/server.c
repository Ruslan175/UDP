#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include "CListener.hpp"
#include "CRxTxQueue.hpp"

using namespace std;

#define  DEFAULT_PORT   8080
#define  SERVER_CONFIG   "server_config.cfg"

CRxTxQueue RxTxQueue;


void runListener(int port);
void runWorker();
int  getConfig();


int main()
{
    int port = getConfig();    

    cout << endl << "Run server on port " << port << endl;
    thread(runWorker).detach();
    thread(runListener, port).join();
    return 0;
}


int getConfig()
{
    int port = DEFAULT_PORT;
    ifstream fl;
    fl.open(SERVER_CONFIG);
    if (true == fl.is_open()) 
    {
        try
        {
            fl >> port;
        }
        catch(...)
        {
            fl.close();
            port = DEFAULT_PORT;
            cout << "An exception has been generated on file reading" << endl
                 << "The default config will be used" << endl;
        }
        fl.close();
    }
    else
    {
        cout << "File " << SERVER_CONFIG << " cannot be open" << endl 
             << "The default config will be used" << endl;
    }
    return port;
}


void runListener(int port)
{
    CListener ls(port, RxTxQueue);
    if (true == ls.isReady()) ls.run();
}


void runWorker()
{
    do
    {
        RxTxQueue.run();
        _SLEEP_MS(5)
    }
    while (true == RxTxQueue.isRunning());
}
