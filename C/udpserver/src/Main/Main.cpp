#include <iostream>
#include "UDPTranceiver.h"

using namespace std;

int main() {
    UDPTranceiver tranceiver(11111);
    tranceiver.run();
    tranceiver.join();

    return 0;
}