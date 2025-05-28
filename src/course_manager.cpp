#include "course_manager.h"
#include "globals.h"
#include "utils.h"
#include"data_manager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

void daftarMataKuliah() {
    // Mata kuliah semester 1
    matkul["IF101"] = {"IF101", "Algoritma dan Pemrograman", 3, 30};
    matkul["IF102"] = {"IF102", "Kalkulus I", 3, 25};
    matkul["IF103"] = {"IF103", "Pengantar Teknologi Informasi", 2, 20};
    matkul["IF104"] = {"IF104", "Fisika Dasar", 3, 20};
    matkul["IF105"] = {"IF105", "Bahasa Inggris", 2, 25};
    
    // Mata kuliah semester 2
    matkul["IF201"] = {"IF201", "Struktur Data", 3, 30};
    matkul["IF202"] = {"IF202", "Kalkulus II", 3, 25};
    matkul["IF203"] = {"IF203", "Sistem Digital", 3, 20};
    matkul["IF204"] = {"IF204", "Aljabar Linear", 3, 20};
    matkul["IF205"] = {"IF205", "Rekayasa Perangkat Lunak", 3, 25};
    
    // Mata kuliah semester 3
    matkul["IF301"] = {"IF301", "Basis Data", 3, 30};
    matkul["IF302"] = {"IF302", "Jaringan Komputer", 3, 25};
    matkul["IF303"] = {"IF303", "Pemrograman Berbasis Objek", 3, 20};
    matkul["IF304"] = {"IF304", "Sistem Operasi", 3, 20};
    matkul["IF305"] = {"IF305", "Metode Numerik", 3, 25};
    
    // Mata kuliah semester 4
    matkul["IF401"] = {"IF401", "Pemrograman Web", 3, 30};
    matkul["IF402"] = {"IF402", "Grafika Komputer", 3, 25};
    matkul["IF403"] = {"IF403", "Kecerdasan Buatan", 3, 20};
    matkul["IF404"] = {"IF404", "Interaksi Manusia dan Komputer", 3, 20};
    matkul["IF405"] = {"IF405", "Statistika", 3, 25};
    
    // Set prerequisites
    matkul["IF201"].prasyarat.push_back("IF101");
    matkul["IF202"].prasyarat.push_back("IF102");
    matkul["IF301"].prasyarat.push_back("IF201");
    matkul["IF302"].prasyarat.push_back("IF203");
    matkul["IF303"].prasyarat.push_back("IF101");
    matkul["IF401"].prasyarat.push_back("IF301");
    matkul["IF402"].prasyarat.push_back("IF204");
    matkul["IF403"].prasyarat.push_back("IF303");
}

bool cekPrasyarat(const string& kodeMK) {
    const MataKuliah& mk = matkul[kodeMK];
    
    if (mk.prasyarat.empty()) return true;
    
    for (const string& prasyarat : mk.prasyarat) {
        bool lulus = false;
        for (const RiwayatNilai& nilai : sesi->riwayatNilai) {
            if (nilai.kodeMK == prasyarat && nilai.nilai != 'E' && nilai.nilai != 'D') {
                lulus = true;
                break;
            }
        }
        if (!lulus) {
            cout << "\033[31m" << "Prasyarat " << prasyarat << " (" << matkul[prasyarat].nama << ") belum terpenuhi." << "\033[0m" << endl;
            return false;
        }
    }
    return true;
}

int hitungTotalSKS() {
    int total = 0;
    for (const string& kode : sesi->krs) {
        total += matkul[kode].sks;
    }
    return total;
}

void tambahMK() {
    printHeader();
    cout << "\033[36m" << "== TAMBAH MATA KULIAH ==" << "\033[0m" << endl;
    
    // Tampilkan daftar mata kuliah yang tersedia dan belum diambil
    cout << "\033[33m" << "Daftar Mata Kuliah yang Tersedia:" << "\033[0m" << endl;
    cout << "┌───────┬─────────────────────────────────┬─────┬────────┬─────────┐" << endl;
    cout << "│ Kode  │ Nama Mata Kuliah               │ SKS │ Kuota  │ Status  │" << endl;
    cout << "├───────┼─────────────────────────────────┼─────┼────────┼─────────┤" << endl;
    
    vector<MataKuliah*> tersedia;
    for (auto& pair : matkul) {
        MataKuliah& mk = pair.second;
        if (find(sesi->krs.begin(), sesi->krs.end(), mk.kode) == sesi->krs.end()) {
            tersedia.push_back(&mk);
        }
    }
    
    sort(tersedia.begin(), tersedia.end(), [](MataKuliah* a, MataKuliah* b) {
        return a->kode < b->kode;
    });
    
    for (MataKuliah* mk : tersedia) {
        string status;
        if (mk->kuota_terisi >= mk->kuota_max) {
            status = "\033[31mPENUH\033[0m";
        } else {
            status = "\033[32mTERSEDIA\033[0m";
        }
        
        cout << "│ " << setw(5) << left << mk->kode 
             << "│ " << setw(31) << left << mk->nama 
             << "│ " << setw(3) << right << mk->sks 
             << " │ " << setw(3) << right << mk->kuota_terisi << "/" << setw(2) << left << mk->kuota_max 
             << " │ " << setw(7) << left << status << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴─────┴────────┴─────────┘" << endl;
    
    // Total SKS saat ini
    int totalSKS = hitungTotalSKS();
    cout << "Total SKS saat ini: " << totalSKS << "/" << sesi->max_sks << endl;
    
    string kode;
    cout << "\nMasukkan Kode MK (atau 0 untuk batal): "; cin >> kode;
    
    if (kode == "0") {
        return;
    }
    
    // Validasi input
    if (matkul.find(kode) == matkul.end()) {
        cout << "\033[31m" << "Kode mata kuliah tidak valid." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    if (find(sesi->krs.begin(), sesi->krs.end(), kode) != sesi->krs.end()) {
        cout << "\033[31m" << "Mata kuliah sudah terdaftar dalam KRS Anda." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    if (matkul[kode].kuota_terisi >= matkul[kode].kuota_max) {
        cout << "\033[31m" << "Kuota mata kuliah penuh." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    // Validasi total SKS
    if (totalSKS + matkul[kode].sks > sesi->max_sks) {
        cout << "\033[31m" << "Total SKS akan melebihi batas maksimum (" << sesi->max_sks << ")." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    // Validasi prasyarat
    if (!cekPrasyarat(kode)) {
        pressEnterToContinue();
        return;
    }

    // Tambahkan mata kuliah ke KRS
    sesi->krs.push_back(kode);
    matkul[kode].kuota_terisi++;
    
    // Simpan ke riwayat untuk undo
    Aksi aksi;
    aksi.kodeMK = kode;
    aksi.tambah = true;
    aksi.waktu = time(0);
    sesi->riwayat.push(aksi);
    
    saveDataToFile();
    
    cout << "\033[32m" << "Mata kuliah " << kode << " - " << matkul[kode].nama 
         << " berhasil ditambahkan ke KRS." << "\033[0m" << endl;
    pressEnterToContinue();
}

void batalMK() {
    printHeader();
    cout << "\033[36m" << "== BATAL MATA KULIAH ==" << "\033[0m" << endl;
    
    if (sesi->krs.empty()) {
        cout << "\033[33m" << "Belum ada mata kuliah yang terdaftar dalam KRS." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    // Tampilkan daftar mata kuliah yang sudah diambil
    cout << "Daftar Mata Kuliah dalam KRS:" << endl;
    cout << "┌───────┬─────────────────────────────────┬─────┐" << endl;
    cout << "│ Kode  │ Nama Mata Kuliah               │ SKS │" << endl;
    cout << "├───────┼─────────────────────────────────┼─────┤" << endl;
    
    for (const string& kode : sesi->krs) {
        cout << "│ " << setw(5) << left << kode 
             << "│ " << setw(31) << left << matkul[kode].nama 
             << "│ " << setw(3) << right << matkul[kode].sks << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴─────┘" << endl;
    
    // Total SKS
    int totalSKS = hitungTotalSKS();
    cout << "Total SKS: " << totalSKS << endl;
    
    string kode;
    cout << "\nMasukkan Kode MK yang ingin dibatalkan (atau 0 untuk batal): "; cin >> kode;
    
    if (kode == "0") {
        return;
    }
    
    vector<string>::iterator it = find(sesi->krs.begin(), sesi->krs.end(), kode);
    if (it == sesi->krs.end()) {
        cout << "\033[31m" << "Mata kuliah tidak terdaftar dalam KRS Anda." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    // Hapus mata kuliah dari KRS
    sesi->krs.erase(it);
    matkul[kode].kuota_terisi--;
    
    // Simpan ke riwayat untuk undo
    Aksi aksi;
    aksi.kodeMK = kode;
    aksi.tambah = false;
    aksi.waktu = time(0);
    sesi->riwayat.push(aksi);
    
    saveDataToFile();
    
    cout << "\033[32m" << "Mata kuliah " << kode << " - " << matkul[kode].nama 
         << " berhasil dibatalkan dari KRS." << "\033[0m" << endl;
    pressEnterToContinue();
}

void tampilMK(bool byNama) {
    printHeader();
    string sortBy = byNama ? "NAMA" : "KODE";
    cout << "\033[36m" << "== DAFTAR MATA KULIAH (SORT BY " << sortBy << ") ==" << "\033[0m" << endl;
    
    vector<MataKuliah> daftar;
    for (const auto& pair : matkul) {
        daftar.push_back(pair.second);
    }
    
    sort(daftar.begin(), daftar.end(), [=](const MataKuliah& a, const MataKuliah& b) {
        return byNama ? a.nama < b.nama : a.kode < b.kode;
    });
    
    cout << "----------------------------------------------------------------------" << endl;
    cout << "| Kode  | Nama Mata Kuliah                | SKS | Kuota  | Prasyarat |" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    for (const MataKuliah& mk : daftar) {
        string prasyarat;
        for (size_t i = 0; i < mk.prasyarat.size(); i++) {
            prasyarat += mk.prasyarat[i];
            if (i < mk.prasyarat.size() - 1) prasyarat += ", ";
        }
        
        cout << "│ " << setw(5) << left << mk.kode 
             << "│ " << setw(31) << left << mk.nama 
             << "│ " << setw(3) << right << mk.sks 
             << " │ " << setw(3) << right << mk.kuota_terisi << "/" << setw(2) << left << mk.kuota_max 
             << " │ " << setw(9) << left << prasyarat << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴─────┴────────┴───────────┘" << endl;
    
    int totalMK = daftar.size();
    int totalSKS = 0;
    for (const MataKuliah& mk : daftar) {
        totalSKS += mk.sks;
    }
    
    cout << "Total mata kuliah: " << totalMK << endl;
    cout << "Total SKS seluruh mata kuliah: " << totalSKS << endl;
    
    pressEnterToContinue();
}

void lihatKuota() {
    printHeader();
    cout << "\033[36m" << "== INFORMASI KUOTA MATA KULIAH ==" << "\033[0m" << endl;
    
    cout << "----------------------------------------------------------------------" << endl;
    cout << "| Kode  | Nama Mata Kuliah               |    Kuota   |  Persentase  |" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    vector<MataKuliah> daftar;
    for (const auto& pair : matkul) {
        daftar.push_back(pair.second);
    }
    
    sort(daftar.begin(), daftar.end(), [](const MataKuliah& a, const MataKuliah& b) {
        // Sort by utilization percentage (descending)
        return (static_cast<float>(a.kuota_terisi) / a.kuota_max) > 
               (static_cast<float>(b.kuota_terisi) / b.kuota_max);
    });
    
    for (const MataKuliah& mk : daftar) {
        float persentase = (static_cast<float>(mk.kuota_terisi) / mk.kuota_max) * 100;
        string status;
        
        if (persentase >= 90) {
            status = "\033[31m" + to_string(static_cast<int>(persentase)) + "%" + "\033[0m";
        } else if (persentase >= 70) {
            status = "\033[33m" + to_string(static_cast<int>(persentase)) + "%" + "\033[0m";
        } else {
            status = "\033[32m" + to_string(static_cast<int>(persentase)) + "%" + "\033[0m";
        }
        
        cout << "│ " << setw(5) << left << mk.kode 
             << "│ " << setw(31) << left << mk.nama 
             << "│ " << setw(5) << right << mk.kuota_terisi << "/" << setw(4) << left << mk.kuota_max 
             << " │ " << setw(9) << left << status << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴────────────┴───────────┘" << endl;
    
    pressEnterToContinue();
}

void lihatSebaran() {
    printHeader();
    cout << "\033[36m" << "== SEBARAN NILAI MATA KULIAH ==" << "\033[0m" << endl;
    
    // Tampilkan daftar mata kuliah
    cout << "Pilih mata kuliah untuk melihat sebaran nilai:" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << "| Kode     | Nama Mata Kuliah                      |   Jumlah Nilai  |" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    vector<pair<string, int>> mkWithGrades;
    for (const auto& pair : matkul) {
        if (!pair.second.nilaiMutu.empty()) {
            mkWithGrades.push_back({pair.first, pair.second.nilaiMutu.size()});
        }
    }
    
    sort(mkWithGrades.begin(), mkWithGrades.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second; // Sort by number of grades (descending)
    });
    
    for (const pair<string, int>& mk : mkWithGrades) {
        cout << "│ " << setw(5) << left << mk.first
             << "│ " << setw(31) << left << matkul[mk.first].nama
             << "│ " << setw(11) << right << mk.second << " │" << endl;
    }
    cout << "└───────┴─────────────────────────────────┴─────────────┘" << endl;
    
    string kode;
    cout << "\nMasukkan Kode MK (atau 0 untuk batal): "; cin >> kode;
    
    if (kode == "0") {
        return;
    }
    
    if (matkul.find(kode) == matkul.end()) {
        cout << "\033[31m" << "Kode mata kuliah tidak valid." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    MataKuliah& mk = matkul[kode];
    
    if (mk.nilaiMutu.empty()) {
        cout << "\033[33m" << "Belum ada data nilai untuk mata kuliah ini." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    map<char, int> count;
    for (const auto& nilai : mk.nilaiMutu) {
        count[nilai.second]++;
    }
    
    // Hitung statistik
    int total = mk.nilaiMutu.size();
    float rata2 = 0;
    for (const auto& nilai : mk.nilaiMutu) {
        rata2 += nilaiMutuToAngka(nilai.second);
    }
    rata2 /= total;
    
    // Tampilkan sebaran nilai dalam bentuk grafik bar
    cout << "\nSebaran Nilai untuk " << mk.kode << " - " << mk.nama << "\n" << endl;
    cout << "Total Mahasiswa: " << total << endl;
    cout << "Rata-rata Nilai: " << fixed << setprecision(2) << rata2 << endl << endl;
    
    const int maxBarWidth = 40;
    
    cout << "Grafik Sebaran Nilai:" << endl;
    cout << "┌───┬───────────────────────────────────────────┬───────┐" << endl;
    cout << "│ A │ ";
    int barWidth = count['A'] * maxBarWidth / total;
    cout << string(barWidth, '#') << string(maxBarWidth - barWidth, ' ');
    cout << " │ " << setw(5) << right << count['A'] << " │" << endl;
    
    cout << "│ B │ ";
    barWidth = count['B'] * maxBarWidth / total;
    cout << string(barWidth, '#') << string(maxBarWidth - barWidth, ' ');
    cout << " │ " << setw(5) << right << count['B'] << " │" << endl;
    
    cout << "│ C │ ";
    barWidth = count['C'] * maxBarWidth / total;
    cout << string(barWidth, '#') << string(maxBarWidth - barWidth, ' ');
    cout << " │ " << setw(5) << right << count['C'] << " │" << endl;
    
    cout << "│ D │ ";
    barWidth = count['D'] * maxBarWidth / total;
    cout << string(barWidth, '#') << string(maxBarWidth - barWidth, ' ');
    cout << " │ " << setw(5) << right << count['D'] << " │" << endl;
    
    cout << "│ E │ ";
    barWidth = count['E'] * maxBarWidth / total;
    cout << string(barWidth, '#') << string(maxBarWidth - barWidth, ' ');
    cout << " │ " << setw(5) << right << count['E'] << " │" << endl;
    cout << "└───┴───────────────────────────────────────────┴───────┘" << endl;
    
    // Tampilkan persentase
    cout << "\nPersentase:" << endl;
    cout << "A: " << fixed << setprecision(1) << (count['A'] * 100.0 / total) << "%" << endl;
    cout << "B: " << fixed << setprecision(1) << (count['B'] * 100.0 / total) << "%" << endl;
    cout << "C: " << fixed << setprecision(1) << (count['C'] * 100.0 / total) << "%" << endl;
    cout << "D: " << fixed << setprecision(1) << (count['D'] * 100.0 / total) << "%" << endl;
    cout << "E: " << fixed << setprecision(1) << (count['E'] * 100.0 / total) << "%" << endl;
    
    pressEnterToContinue();
}