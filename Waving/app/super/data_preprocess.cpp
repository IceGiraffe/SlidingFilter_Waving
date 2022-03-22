#pragma GCC optimize("O3")
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <unordered_map>
#include <algorithm>
#include <chrono>

#include "abstract.h"
#include "Waving.h"
#include "Waving_Pure.h"
#include "opensketch.h"
#include "olf.h"
#include "tlf.h"

typedef std::chrono::high_resolution_clock::time_point TP;
inline TP now()
{
    return std::chrono::high_resolution_clock::now();
}

const string FOLDER = "/root/CAIDA/";

//===============================================
// #define HIT 84
// const string FILE_NAME {"CAIDA2018_8.dat"};
//===============================================
// #define HIT 10
vector<string> FILE_NAMES = {"130000.dat", "130100.dat", "130200.dat", "130300.dat", "130400.dat", "130500.dat", "130600.dat", "130700.dat", "130800.dat", "130900.dat"};
vector<Data> all_from;
vector<Data> all_to;
ofstream fout;


int Get_TopK(HashMap mp, int k)
{
    int size = mp.size();
    int *num = new int[size];
    int pos = 0;
    HashMap::iterator it;
    for (it = mp.begin(); it != mp.end(); ++it)
    {
        num[pos++] = it->second;
    }
    nth_element(num, num + size - k, num + size);
    int ret = num[size - k];
    delete num;
    return ret;
}

int combine()
{
    fout.open("/root/CAIDA/CAIDA.dat", ios::binary);
    if (fout.fail())
    {
        cout << "Open Output File Failed..." << endl;
        return 0;
    }

    all_from.clear();
    all_to.clear();
    // ===================== Determine the threshold==================

    HashMap mp;
    StreamMap sp;
    uint num = 0;

    for (int i = 0; i < 10; i++)
    {
        auto FILE_NAME = FILE_NAMES[i];
        cout << "FIle Name: " << FILE_NAME << endl;
        FILE *file = fopen((FOLDER + FILE_NAME).c_str(), "rb");
        Data from;
        Data to;

        char tmp[20];
        TP start = now();
        while (1)
        {
            int m = fread(from.str, DATA_LEN, 1, file);
            fread(tmp, 1, 2, file);
            int n = fread(to.str, DATA_LEN, 1, file);
            fread(tmp, 1, 2, file);
            fread(tmp, 1, 9, file);
            // fout << from.str << to.str;
            fout.write((const char *)from.str, 4);
            fout.write((const char *)to.str, 4);
            // all_from.push_back(from);
            // all_to.push_back(to);

            if (m != 1 || n != 1)
                break;
            Stream stream(from, to);
            if (sp.find(stream) == sp.end())
            {
                sp[stream] = 1;
                if (mp.find(from) == mp.end())
                    mp[from] = 1;
                else
                    mp[from] += 1;
            }
            num++;
        }
        fclose(file);
    }
    cout << "True Distinct Item: " << sp.size() << endl;
    cout << "Total Packet Number: " << num << endl;
    int HIT = Get_TopK(mp, 1500);
    printf("HIT=%d\n", HIT);
    return 0;
}



int test_integrated()
{
    HashMap mp;
    StreamMap sp;
    int num = 0;
    FILE *file = fopen("/root/CAIDA/CAIDA.dat", "rb");
    Data from;
    Data to;

    TP start = now();
    while (1)
    {
        int m = fread(from.str, DATA_LEN, 1, file);
        // fread(tmp, 1, 2, file);
        int n = fread(to.str, DATA_LEN, 1, file);
        // fread(tmp, 1, 2, file);
        // fread(tmp, 1, 9, file);
        // all_from.push_back(from);
        // all_to.push_back(to);

        if (m != 1 || n != 1)
            break;
        Stream stream(from, to);
        if (sp.find(stream) == sp.end())
        {
            sp[stream] = 1;
            if (mp.find(from) == mp.end())
                mp[from] = 1;
            else
                mp[from] += 1;
        }
        num++;
        if(num % 10000000 == 0) cout << "%" << endl;
        // cout << num << endl;
    }
    fclose(file);
    cout << "True Distinct Item: " << sp.size() << endl;
    cout << "Total Packet Number: " << num << endl;
    int HIT = Get_TopK(mp, 1500);
    printf("HIT=%d\n", HIT);
    return 0;
}

int main(){
    // combine();
    // test_integrated();
    return 0;
}