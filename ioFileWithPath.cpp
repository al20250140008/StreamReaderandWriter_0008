#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
    //deklarasi variabel baris dan NamaFile
    string baris, NamaFile;

    //membuat input untuk menangkap nama file
    cout << "Masukkan nama file : ";
    cin >> NamaFile;

    //membuat file dalam mode menulis
    ofstream outfile;
    //menunjuk ke sebuah nama file dengan ekstentsi txt
    outfile.open(NamaFile + ".txt", ios::out);

    cout << ">= Menulis file, \'q\' untuk keluar" << endl;

    //Looping untuk menulis isi file
   