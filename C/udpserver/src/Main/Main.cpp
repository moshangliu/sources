#include <iostream>
#include "UDPTranceiver.h"

using namespace std;

int main() {
    UDPTranceiver tranceiver(11111);
    tranceiver.Run();
    tranceiver.Join();

    cout << "UDPTranceiver stopped" << endl;
    return 0;
}