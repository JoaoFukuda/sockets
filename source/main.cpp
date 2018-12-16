// Main file used just for testing of some simpler functionalities

#include <cstring>
#include <iostream>
using namespace std;

void Change(bool *l)
{
    l[0] = true;
}

int main(int argc, char const *argv[])
{
    bool list[3];
    memset(&list, 0, sizeof(list));
    for(int n = 0; n < 3; n++) cout << list[n] << endl;
    Change(list);
    for(int n = 0; n < 3; n++) cout << list[n] << endl;
    return 0;
}
