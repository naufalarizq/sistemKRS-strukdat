#include "grade_manager.h"
#include "globals.h"
#include "utils.h"
#include "data_manager.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void inputNilai() {
    printHeader();
    cout << "\033[36m" << "== INPUT NILAI MAHASISWA ==" << "\033[0m" << endl;
    
    string nim;
    cout << "Masukkan NIM mahasiswa: "; cin >> nim;
    
    if (akun.find(nim) == akun.end()) {
        cout << "\033[31m" << "Mahasiswa tidak ditemukan." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    Mahasiswa& mhs = akun[nim];
    cout << "Mahasiswa: " << mhs.nim << " - " << mhs.nama << endl;
    
    // Tampilkan mata kuliah yang diambil
    cout << "\nMata Kuliah yang diambil:" << endl;
    cout << "┌───────┬─────────────────────────────────┬─────┐" << endl;
    cout << "│ Kode  │ Nama Mata Kuliah               │ SKS │" << endl;
    cout << "├───────┼─────────────────────────────────┼─────┤" << endl;
    
    for (const string& kode : mhs.krs) {
        cout << "│ " << setw(5) << left << kode 
             << "│ " << setw(31) << left << matkul[kode].nama 
             << "│ " << setw(3) << right << matkul[kode].sks << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴─────┘" << endl;
    
    string kodeMK;
    cout << "\nMasukkan Kode MK yang akan diinput nilainya (atau 0 untuk batal): "; cin >> kodeMK;
    
    if (kodeMK == "0") return;
    
    if (find(mhs.krs.begin(), mhs.krs.end(), kodeMK) == mhs.krs.end()) {
        cout << "\033[31m" << "Mata kuliah tidak terdaftar dalam KRS mahasiswa." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    char nilai;
    cout << "Masukkan Nilai (A/B/C/D/E): "; cin >> nilai;
    
    if (nilai != 'A' && nilai != 'B' && nilai != 'C' && nilai != 'D' && nilai != 'E') {
        cout << "\033[31m" << "Nilai tidak valid." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    // Input nilai
    matkul[kodeMK].nilaiMutu[nim] = nilai;
    
    // Tambahkan ke riwayat nilai
    RiwayatNilai rn;
    rn.kodeMK = kodeMK;
    rn.namaMK = matkul[kodeMK].nama;
    rn.sks = matkul[kodeMK].sks;
    rn.nilai = nilai;
    rn.semester = to_string(mhs.semester);
    
    // Hapus dari riwayat nilai jika sudah ada
    for (auto it = mhs.riwayatNilai.begin(); it != mhs.riwayatNilai.end(); ) {
        if (it->kodeMK == kodeMK) {
            it = mhs.riwayatNilai.erase(it);
        } else {
            ++it;
        }
    }
    
    mhs.riwayatNilai.push_back(rn);
    
    // Hapus dari KRS jika sudah dinilai
    mhs.krs.erase(remove(mhs.krs.begin(), mhs.krs.end(), kodeMK), mhs.krs.end());
    matkul[kodeMK].kuota_terisi--;
    
    saveDataToFile();
    
    cout << "\033[32m" << "Nilai berhasil diinput." << "\033[0m" << endl;
    pressEnterToContinue();
}

void lihatRiwayatDanIPB() {
    printHeader();
    cout << "\033[36m" << "== RIWAYAT NILAI DAN IPB ==" << "\033[0m" << endl;
    
    cout << "NIM: " << sesi->nim << endl;
    cout << "Nama: " << sesi->nama << endl;
    cout << "Program Studi: " << sesi->prodi << endl;
    cout << "Semester: " << sesi->semester << endl;
    
    if (sesi->riwayatNilai.empty()) {
        cout << "\033[33m" << "Belum ada riwayat nilai." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "┌─────────┬─────────────────────────────┬─────┬───────┐" << endl;
    cout << "│ Kode MK │ Nama Mata Kuliah            │ SKS │ Nilai │" << endl;
    cout << "├─────────┼─────────────────────────────┼─────┼───────┤" << endl;
    
    float totalBobot = 0;
    int totalSKS = 0;
    
    for (const RiwayatNilai& rn : sesi->riwayatNilai) {
        cout << "│ " << setw(7) << left << rn.kodeMK 
             << " │ " << setw(27) << left << rn.namaMK 
             << " │ " << setw(3) << right << rn.sks 
             << " │ " << setw(5) << right << rn.nilai << " │" << endl;
        
        totalBobot += rn.sks * nilaiMutuToAngka(rn.nilai);
        totalSKS += rn.sks;
    }
    
    cout << "└─────────┴─────────────────────────────┴─────┴───────┘" << endl;
    
    float ipb = (totalSKS > 0) ? (totalBobot / totalSKS) : 0;
    cout << "Indeks Prestasi Bersama (IPB): " << fixed << setprecision(2) << ipb << endl;
    
    pressEnterToContinue();
}

void simulasiIPK() {
    printHeader();
    cout << "\033[36m" << "== SIMULASI IPK ==" << "\033[0m" << endl;
    
    int jumlahMK = getNumericInput<int>("Masukkan jumlah mata kuliah yang akan disimulasikan: ");
    if (jumlahMK <= 0) {
        cout << "\033[31m" << "Jumlah mata kuliah harus lebih dari 0." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    vector<int> sksList(jumlahMK);
    vector<char> nilaiList(jumlahMK);
    
    for (int i = 0; i < jumlahMK; i++) {
        cout << "Mata Kuliah ke-" << (i+1) << ":" << endl;
        sksList[i] = getNumericInput<int>("SKS: ");
        
        char nilai;
        do {
            cout << "Nilai (A/B/C/D/E): ";
            cin >> nilai;
            nilai = toupper(nilai);
        } while (nilai != 'A' && nilai != 'B' && nilai != 'C' && nilai != 'D' && nilai != 'E');
        
        nilaiList[i] = nilai;
    }
    
    float totalBobot = 0;
    int totalSKS = 0;
    
    for (int i = 0; i < jumlahMK; i++) {
        totalBobot += sksList[i] * nilaiMutuToAngka(nilaiList[i]);
        totalSKS += sksList[i];
    }
    
    float ipk = (totalSKS > 0) ? (totalBobot / totalSKS) : 0;
    cout << "Simulasi IPK: " << fixed << setprecision(2) << ipk << endl;
    
    pressEnterToContinue();
}
