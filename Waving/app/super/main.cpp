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

using namespace std;

#define BLOCK 10000
const int interval = 10000000;

typedef std::chrono::high_resolution_clock::time_point TP;
inline TP now()
{
    return std::chrono::high_resolution_clock::now();
}

// const string FOLDER  = "../../dataset/";
const string FOLDER = "/root/CAIDA/";

//===============================================
// #define HIT 84
// const string FILE_NAME {"CAIDA2018_8.dat"};
//===============================================
// #define HIT 10
const string FILE_NAME{"CAIDA.dat"};
//===============================================

void Test_Hitter(string PATH);
void Test_Speed(string PATH);

int HIT;

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

vector<Data> all_from;
vector<Data> all_to;
ofstream fout("result.txt");
HashMap mp;
StreamMap sp;

int main()
{
    fout << "Mem"
         << "\t"
         << "Alg"
         << "\t"
         << "f1"
         << "\t"
         << "pr"
         << "\t"
         << "cr"
         << "\t"
         << "aae"
         << "\t"
         << "are" << endl;
    all_from.clear();
    all_to.clear();
    // ===================== Determine the threshold==================
    cout << FILE_NAME << endl;

    FILE *file = fopen((FOLDER + FILE_NAME).c_str(), "rb");
    Data from;
    Data to;
    uint num = 0;
    char tmp[20];
    TP start = now();
    while (1)
    {
        int m = fread(from.str, DATA_LEN, 1, file);
        // fread(tmp, 1, 2, file);
        int n = fread(to.str, DATA_LEN, 1, file);
        // fread(tmp, 1, 2, file);
        // fread(tmp, 1, 9, file);

        all_from.push_back(from);
        all_to.push_back(to);

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
    TP finish = now();

    double duration = (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000000>>>(finish - start).count();
    cout << "Read File: " << (num + 0.0) / duration << endl;

    cout << "True Distinct Item: " << sp.size() << endl;
    cout << "Total Packet Number: " << num << endl;
    HIT = Get_TopK(mp, 5000);
    printf("HIT=%d\n", HIT);
    fclose(file);
    // return 0;

    printf("\033[0m\033[1;32m====================================================\n\033[0m");
    printf("\033[0m\033[1;32m|         Application: Find Super Spreader         |\n\033[0m");
    printf("\033[0m\033[1;32m====================================================\n\033[0m");
    printf("\033[0m\033[1;32m|                     F1 SCORE                     |\n\033[0m");
    printf("\033[0m\033[1;32m====================================================\n\033[0m");
    // Test_Hitter(FOLDER + FILE_NAME);

    printf("\033[0m\033[1;32m====================================================\n\033[0m");
    printf("\033[0m\033[1;32m|                    THROUGHPUT                    |\n\033[0m");
    printf("\033[0m\033[1;32m====================================================\n\033[0m");
    Test_Speed(FOLDER + FILE_NAME);
    printf("\033[0m\033[1;32m====================================================\n\033[0m");
}

void Test_Hitter(string PATH)
{
    for (int i = 0; i < 10; ++i)
    {
        int memory = BLOCK * (i + 10);

        int BF_memory = 5000000;
        int bucket_num = 1000;

        printf("\033[0m\033[1;4;36m> Memory size: %dKB\n\033[0m", memory / 1000);

        int SKETCH_NUM = 2;
        Abstract *sketch[SKETCH_NUM];

        sketch[0] = new WavingSketch<16, 8>(memory - 24 * bucket_num, HIT, BF_memory, 0, bucket_num);
        sketch[1] = new WavingSketch_P<16, 8>(memory, HIT, BF_memory);
        // sketch[1] = new TLF(memory, HIT);
        // sketch[2] = new OLF(memory, HIT);
        // sketch[3] = new OpenSketch(memory, HIT);

        // HashMap mp;
        // StreamMap sp;

        // FILE *file = fopen(PATH.c_str(), "rb");
        Data from;
        Data to;
        int num = 0;

        // SKETCH_NUM = 1;
        num = all_from.size();
        for (int kkk = 0; kkk < num; kkk++)
        {
            auto from = all_from[kkk];
            auto to = all_to[kkk];
            for (int j = 0; j < SKETCH_NUM; ++j)
            {
                sketch[j]->Init(from, to);
            }
        }
        cout << num << endl;

        for (int j = 0; j < SKETCH_NUM; ++j)
        {
            sketch[j]->Check(mp);
            printf("\033[0m\033[1;36m|\033[0m\t");
            fout << memory << '\t';
            sketch[j]->print_f1(fout, memory);
            sketch[j]->print_info(fout);
        }

        for (int j = 0; j < SKETCH_NUM; ++j)
        {
            delete sketch[j];
        }
    }
}

void Test_Speed(string PATH)
{
    const int CYCLE = 1;
    Data from;
    Data to;

    for (int i = 0; i < 10; ++i)
    {
        int memory = BLOCK * (i + 10);
        int BF_memory = 5000000;
        int bucket_num = 1000;

        printf("\033[0m\033[1;4;36m> Memory size: %dKB\n\033[0m", memory / 1000);
        for (int k = 0; k < CYCLE; ++k)
        {
            // WavingSketch<8, 16> sketch(memory, HIT, 500000);
            WavingSketch<16, 8> sketch(memory - 24 * bucket_num, HIT, BF_memory, 0, bucket_num);
            int num = 0;

            TP start = now();
            num = all_from.size();
            for (int i = 0; i < num; i++)
            {
                sketch.Init(all_from[i], all_to[i]);
            }

            TP finish = now();

            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0) / duration << endl;
            fout << sketch.name << sketch.sep << (num + 0.0) / duration << endl;
        }
        for (int k = 0; k < CYCLE; ++k)
        {
            // WavingSketch_P<8, 16> sketch(memory, HIT, 500000);
            WavingSketch_P<16, 8> sketch(memory, HIT, BF_memory);
            int num = 0;

            TP start = now();
            num = all_from.size();
            for (int i = 0; i < num; i++)
            {
                sketch.Init(all_from[i], all_to[i]);
            }
            TP finish = now();
            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0) / duration << endl;
            fout << sketch.name << sketch.sep << (num + 0.0) / duration << endl;
        }
    }
}

/*
void Old_Test_Speed(string PATH){
    const int CYCLE = 1;
    Data from;
    Data to;

    for(int i = 0;i < 5;++i){
        int memory = 6*BLOCK + BLOCK * i;
        printf("\033[0m\033[1;4;36m> Memory size: %dKB\n\033[0m",memory/1000);
        for(int k = 0;k < CYCLE;++k){
            WavingSketch<8,16> sketch(memory, HIT);
            FILE* file = fopen(PATH.c_str(),"rb");
            int num = 0;

            TP start = now();
            while(num<interval)
            {
                fread(from.str, DATA_LEN, 1, file);
                fread(to.str, DATA_LEN, 1, file);
                ++num;
                sketch.Init(from, to);
            }
            TP finish = now();

            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0)/ duration << endl;

            fclose(file);
        }

        for(int k = 0;k < CYCLE;++k){
            TLF sketch(memory, HIT);
            FILE* file = fopen(PATH.c_str(),"rb");
            int num = 0;

            TP start = now();
            while(num<interval)
            {
                fread(from.str, DATA_LEN, 1, file);
                fread(to.str, DATA_LEN, 1, file);
                ++num;
                sketch.Init(from, to);
            }
            TP finish = now();

            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0)/ duration << endl;

            fclose(file);
        }

        for(int k = 0;k < CYCLE;++k){
            OLF sketch(memory, HIT);
            FILE* file = fopen(PATH.c_str(),"rb");
            int num = 0;

            TP start = now();
            while(num<interval)
            {
                fread(from.str, DATA_LEN, 1, file);
                fread(to.str, DATA_LEN, 1, file);
                ++num;
                sketch.Init(from, to);
            }
            TP finish = now();

            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0)/ duration << endl;

            fclose(file);
        }

        for(int k = 0;k < CYCLE;++k){
            OpenSketch sketch(memory, HIT);
            FILE* file = fopen(PATH.c_str(),"rb");
            int num = 0;

            TP start = now();
            while(num<interval)
            {
                fread(from.str, DATA_LEN, 1, file);
                fread(to.str, DATA_LEN, 1, file);
                ++num;
                sketch.Init(from, to);
            }
            TP finish = now();

            double duration = (double)std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000>>>(finish - start).count();
            printf("\033[0m\033[1;36m|\033[0m\t");
            cout << sketch.name << sketch.sep << (num + 0.0)/ duration << endl;

            fclose(file);
        }
    }
}
*/
