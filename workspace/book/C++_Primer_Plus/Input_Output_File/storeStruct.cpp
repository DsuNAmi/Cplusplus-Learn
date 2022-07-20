#include <iostream>
#include <fstream>
#include <iomanip>


using namespace std;

inline void eatline() {while(cin.get() != '\n') continue;}

struct planet
{
    char name[20];
    double population;
    double g;
};

const char * fileName = "planet.dat";

int main()
{
    planet p1;
    cout << fixed << right;

    //可能两次运行，先读取，因为第一次没有内容，所以会跳过的
    ifstream fin;
    fin.open(fileName,ios::in | ios::binary);
    if(fin.is_open())
    {
        while(fin.read((char*)&p1, sizeof(p1)))
        {
            cout << setw(20) << p1.name << ":"
                 << setprecision(0) << setw(12) << p1.population
                 << setprecision(2) << setw(6) << p1.g << endl;
        }
        fin.close();
    }


    //加入新数据
    fstream fout(fileName,ios::out | ios::app | ios::binary);
    if(!fout.is_open())
    {
        abort();
    }
    cin.get(p1.name,20);
    while(p1.name[0] != '\0')
    {
        eatline();
        cin >> p1.population;
        cin >> p1.g;
        fout.write((char*)&p1,sizeof(p1));
        cin.get(p1.name,20);
    }
    fout.close();

    //重新连接之前的流
    fin.clear();
    fin.open(fileName,ios::in | ios::binary);
    if(fin.is_open())
    {
        while(fin.read((char*)&p1, sizeof(p1)))
        {
            cout << setw(20) << p1.name << ":"
                 << setprecision(0) << setw(12) << p1.population
                 << setprecision(2) << setw(6) << p1.g << endl;
        }
        fin.close();
    }

    return 0;
}