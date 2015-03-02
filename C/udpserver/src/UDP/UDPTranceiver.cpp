#include "UDPTranceiver.h"
#include "ListenThread.h"
#include <string>
#include <iostream>

using namespace std;

UDPTranceiver::UDPTranceiver(int port) : _port(port) {
    listenThread = new ListenThread(port);
}

UDPTranceiver::~UDPTranceiver() {
    delete listenThread;
}

void UDPTranceiver::Run() {
    cout << "UDPTranceiver started on port: " << _port << endl;
    listenThread->Run();
}

void UDPTranceiver::Join() {
    listenThread->Join();
}