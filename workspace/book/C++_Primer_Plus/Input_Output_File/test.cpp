#include <iostream>
#include <fstream>
#include <string>


using namespace std;


// const int LIMIT = 255;





int main()
{

    string fileName;
    cin >> fileName;

    ofstream fout(fileName.c_str());
    fout << "For your eyes only!\n";
    float secret;
    cin >> secret;
    fout << "Your secret number is " << secret << endl;
    fout.close();

    ifstream fin(fileName.c_str());
    char ch;
    while(fin.get(ch)) cout << ch;
    fin.close();
    

    // char * amount = "done";
    // cout << (void*)amount << endl;
    // cout << amount << endl;
    // cout << &amount << endl;

    // long val = 560031841;
    // cout.write((char *)&val, sizeof(long));


    // cout << "Enter a integer: ";
    // int n;
    // cin >> n;

    // cout << n << " " << "(decimal)\n";
    // cout << hex; //set to hex;
    // cout << n << " " << "(hexadecimal)\n";
    // cout << oct << n << " " << "(octal)\n"; // set to oct;
    // dec(cout);
    // cout << n << " (decimal)\n";


    // char input[LIMIT];

    // cin.getline(input,LIMIT,'#');
    // cout << input << "\n";
    

    // char ch;
    // cin.get(ch);
    // cout << ch << endl;
    
    // if(ch != '\n')
    // {
    //     cin.ignore(LIMIT,'\n');
    // }

    // cin.get(input,LIMIT,'#');
    // cout << input << "\n";
    
    // cin.get(ch);
    // cout << ch << endl;


    return 0;
}