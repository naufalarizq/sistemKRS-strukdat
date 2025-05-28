#include "utils.h"
#include "globals.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string YELLOW = "\033[33m";
const string BOLD = "\033[1m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    cout << "\n" << YELLOW << "Tekan Enter untuk melanjutkan..." << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getchar();
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    ostringstream oss;
    oss << setfill('0') 
        << setw(2) << ltm->tm_mday << "/"
        << setw(2) << 1 + ltm->tm_mon << "/"
        << 1900 + ltm->tm_year << " "
        << setw(2) << ltm->tm_hour << ":"
        << setw(2) << ltm->tm_min;
    
    return oss.str();
}

string formatTimeStamp(time_t t) {
    tm* ltm = localtime(&t);
    
    ostringstream oss;
    oss << setfill('0') 
        << setw(2) << ltm->tm_mday << "/"
        << setw(2) << 1 + ltm->tm_mon << "/"
        << 1900 + ltm->tm_year << " "
        << setw(2) << ltm->tm_hour << ":"
        << setw(2) << ltm->tm_min;
    
    return oss.str();
}

string hashPassword(const string& pw) {
    hash<string> hasher;
    return to_string(hasher(pw));
}

float nilaiMutuToAngka(char nilai) {
    switch(nilai) {
        case 'A': return 4.0;
        case 'B': return 3.0;
        case 'C': return 2.0;
        case 'D': return 1.0;
        case 'E': return 0.0;
        default: return 0.0;
    }
}

void printHeader() {
    clearScreen();
    cout << BOLD << BLUE << "----------------------------------------------------------------" << RESET << endl;
    cout << BOLD << BLUE << "|                                                              |" << RESET << endl;
    cout << BOLD << BLUE << "|             SISTEM INFORMASI AKADEMIK UNIVERSITAS            |" << RESET << endl;
    cout << BOLD << BLUE << "|                   PENGISIAN KARTU RENCANA STUDI              |" << RESET << endl;
    cout << BOLD << BLUE << "|                                                              |" << RESET << endl;
    cout << BOLD << BLUE << "----------------------------------------------------------------" << RESET << endl;
    
    if (sesi) {
        cout << CYAN << "Mahasiswa: " << sesi->nama << " (" << sesi->nim << ")" << RESET << endl;
        cout << CYAN << "Program Studi: " << sesi->prodi << " - Semester " << sesi->semester << RESET << endl;
        cout << CYAN << "Tanggal: " << getCurrentDateTime() << RESET << endl;
        cout << CYAN << "----------------------------------------------------------------" << RESET << endl;
    }
}

template <typename T>
T getNumericInput(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << RED << "Error: Masukan harus berupa angka." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Explicit template instantiations
template int getNumericInput<int>(const string& prompt);
template float getNumericInput<float>(const string& prompt);
