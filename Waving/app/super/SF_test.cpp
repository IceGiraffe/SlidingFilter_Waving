#include "SF_noSIMD.h"

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <numeric>
#include <cmath>
using namespace std;

int main()
{
    auto sf = new SlidingFilter(2, 1, 0, 4, 32, 32, 2, 2);
    sf->printInfo();
    for (int j = 0; j < 10; j++)
    {
        for (int i = 1; i <= 9; i++)
        {
            sf->insert(i, 3);
            cout << "INSERT " << i << endl;
            cout << "Layer 1" << endl;
            sf->Bucket1[0].print();
            sf->Bucket1[1].print();
            cout << "Layer 2" << endl;
            sf->Bucket2[0].print();
            cout << "Layer 3" << endl;
            // sf->Bucket3[0].print();
            getchar();
        }
    }

    for (int i = 0; i <= 32; i++)
    {
        cout << COUNTER_MASK(i) << endl;
    }
    return 0;
}