#include "krs_manager.h"
#include "globals.h"
#include "utils.h"
#include "data_manager.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

void lihatKRS() {
    printHeader();
    cout << "\033[36m" << "== KARTU RENCANA STUDI (KRS) ==" << "\033[0m" << endl;
    
    if (sesi->krs.empty()) {
        cout << "\033[33m" << "Belum ada mata kuliah yang terdaftar dalam KRS." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "┌───────┬─────────────────────────────────┬─────┐" << endl;
    cout << "│ Kode  │ Nama Mata Kuliah               │ SKS │" << endl;
    cout << "├───────┼─────────────────────────────────┼─────┤" << endl;
    
    int totalSKS = 0;
    for (const string& kode : sesi->krs) {
        cout << "│ " << setw(5) << left << kode 
             << "│ " << setw(31) << left << matkul[kode].nama 
             << "│ " << setw(3) << right << matkul[kode].sks << " │" << endl;
        totalSKS += matkul[kode].sks;
    }
    cout << "└───────┴─────────────────────────────────┴─────┘" << endl;
    cout << "Total SKS: " << totalSKS << endl;
    
    pressEnterToContinue();
}

void exportKRS() {
    printHeader();
    cout << "\033[36m" << "== EXPORT KRS KE FILE ==" << "\033[0m" << endl;
    
    string filename;
    cout << "Masukkan nama file (misal: krs_export.txt): ";
    cin >> filename;
    
    ofstream outFile(filename);
    if (!outFile) {
        cout << "\033[31m" << "Gagal membuka file untuk menulis." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    outFile << "KARTU RENCANA STUDI (KRS)" << endl;
    outFile << "NIM: " << sesi->nim << endl;
    outFile << "Nama: " << sesi->nama << endl;
    outFile << "Program Studi: " << sesi->prodi << endl;
    outFile << "Semester: " << sesi->semester << endl;
    outFile << endl;
    
    outFile << "┌───────┬─────────────────────────────────┬─────┐" << endl;
    outFile << "│ Kode  │ Nama Mata Kuliah               │ SKS │" << endl;
    outFile << "├───────┼─────────────────────────────────┼─────┤" << endl;
    
    int totalSKS = 0;
    for (const string& kode : sesi->krs) {
        outFile << "│ " << kode << " │ " << matkul[kode].nama << " │ " << matkul[kode].sks << " │" << endl;
        totalSKS += matkul[kode].sks;
    }
    outFile << "└───────┴─────────────────────────────────┴─────┘" << endl;
    outFile << "Total SKS: " << totalSKS << endl;
    
    outFile.close();
    
    cout << "\033[32m" << "KRS berhasil diexport ke file " << filename << "\033[0m" << endl;
    pressEnterToContinue();
}

void undo() {
    if (sesi->riwayat.empty()) {
        cout << "\033[33m" << "Tidak ada aksi yang dapat di-undo." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    Aksi aksi = sesi->riwayat.top();
    sesi->riwayat.pop();
    
    if (aksi.tambah) {
        // Undo tambah MK
        auto it = find(sesi->krs.begin(), sesi->krs.end(), aksi.kodeMK);
        if (it != sesi->krs.end()) {
            sesi->krs.erase(it);
            matkul[aksi.kodeMK].kuota_terisi--;
            cout << "\033[32m" << "Undo: Mata kuliah " << aksi.kodeMK << " berhasil dibatalkan." << "\033[0m" << endl;
        }
    } else {
        // Undo batal MK
        sesi->krs.push_back(aksi.kodeMK);
        matkul[aksi.kodeMK].kuota_terisi++;
        cout << "\033[32m" << "Undo: Mata kuliah " << aksi.kodeMK << " berhasil ditambahkan kembali." << "\033[0m" << endl;
    }
    
    saveDataToFile();
    pressEnterToContinue();
}
