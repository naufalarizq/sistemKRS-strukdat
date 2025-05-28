#include "data_manager.h"
#include "globals.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void saveDataToFile() {
    ofstream outMhs("mahasiswa.dat");
    for (const auto& pair : akun) {
        const Mahasiswa& mhs = pair.second;
        outMhs << mhs.nim << "|" << mhs.nama << "|" << mhs.password_hash << "|" 
               << mhs.prodi << "|" << mhs.semester << "|" << mhs.max_sks << endl;
    }
    outMhs.close();
    
    ofstream outMatkul("matakuliah.dat");
    for (const auto& pair : matkul) {
        const MataKuliah& mk = pair.second;
        outMatkul << mk.kode << "|" << mk.nama << "|" << mk.sks << "|" 
                  << mk.kuota_max << "|" << mk.kuota_terisi;
        
        // Save prerequisites
        outMatkul << "|";
        for (size_t i = 0; i < mk.prasyarat.size(); i++) {
            outMatkul << mk.prasyarat[i];
            if (i < mk.prasyarat.size() - 1) outMatkul << ",";
        }
        
        outMatkul << endl;
    }
    outMatkul.close();
    
    // Save KRS and grades separately
    ofstream outKRS("krs.dat");
    for (const auto& pair : akun) {
        const Mahasiswa& mhs = pair.second;
        outKRS << mhs.nim;
        for (const string& kode : mhs.krs) {
            outKRS << "|" << kode;
        }
        outKRS << endl;
    }
    outKRS.close();
    
    ofstream outNilai("nilai.dat");
    for (const auto& pair : akun) {
        const Mahasiswa& mhs = pair.second;
        for (const RiwayatNilai& nilai : mhs.riwayatNilai) {
            outNilai << mhs.nim << "|" << nilai.kodeMK << "|" << nilai.nilai 
                    << "|" << nilai.semester << endl;
        }
    }
    outNilai.close();
}

void daftarMataKuliah();

void loadDataFromFile() {
    // Try to load files, if they exist
    ifstream inMhs("mahasiswa.dat");
    if (inMhs.is_open()) {
        string line;
        while (getline(inMhs, line)) {
            stringstream ss(line);
            string nim, nama, hash, prodi, semStr, maxSksStr;
            
            getline(ss, nim, '|');
            getline(ss, nama, '|');
            getline(ss, hash, '|');
            getline(ss, prodi, '|');
            getline(ss, semStr, '|');
            getline(ss, maxSksStr, '|');
            
            Mahasiswa mhs;
            mhs.nim = nim;
            mhs.nama = nama;
            mhs.password_hash = hash;
            mhs.prodi = prodi;
            mhs.semester = stoi(semStr);
            mhs.max_sks = stoi(maxSksStr);
            
            akun[nim] = mhs;
        }
        inMhs.close();
    }
    
    // Add admin account if not exists
    if (akun.find("admin") == akun.end()) {
        Mahasiswa admin;
        admin.nim = "admin";
        admin.nama = "Administrator";
        admin.password_hash = hashPassword("admin123");
        admin.prodi = "Admin";
        admin.semester = 0;
        akun["admin"] = admin;
    }
    
    // Load mata kuliah
    ifstream inMatkul("matakuliah.dat");
    if (inMatkul.is_open()) {
        string line;
        while (getline(inMatkul, line)) {
            stringstream ss(line);
            string kode, nama, sksStr, maxStr, terisiStr, prasyaratStr;
            
            getline(ss, kode, '|');
            getline(ss, nama, '|');
            getline(ss, sksStr, '|');
            getline(ss, maxStr, '|');
            getline(ss, terisiStr, '|');
            getline(ss, prasyaratStr);
            
            MataKuliah mk;
            mk.kode = kode;
            mk.nama = nama;
            mk.sks = stoi(sksStr);
            mk.kuota_max = stoi(maxStr);
            mk.kuota_terisi = stoi(terisiStr);
            
            // Parse prerequisites
            stringstream prasyaratSS(prasyaratStr);
            string prasyarat;
            while (getline(prasyaratSS, prasyarat, ',')) {
                if (!prasyarat.empty()) {
                    mk.prasyarat.push_back(prasyarat);
                }
            }
            
            matkul[kode] = mk;
        }
        inMatkul.close();
    } else {
        // Jika file tidak ada, buat data default
        daftarMataKuliah();
    }
    
    // Load KRS
    ifstream inKRS("krs.dat");
    if (inKRS.is_open()) {
        string line;
        while (getline(inKRS, line)) {
            stringstream ss(line);
            string nim, kode;
            
            getline(ss, nim, '|');
            while (getline(ss, kode, '|')) {
                akun[nim].krs.push_back(kode);
            }
        }
        inKRS.close();
    }
    
    // Load Nilai
    ifstream inNilai("nilai.dat");
    if (inNilai.is_open()) {
        string line;
        while (getline(inNilai, line)) {
            stringstream ss(line);
            string nim, kodeMK, nilaiStr, semester;
            
            getline(ss, nim, '|');
            getline(ss, kodeMK, '|');
            getline(ss, nilaiStr, '|');
            getline(ss, semester);
            
            if (akun.find(nim) != akun.end() && matkul.find(kodeMK) != matkul.end()) {
                char nilai = nilaiStr[0];
                matkul[kodeMK].nilaiMutu[nim] = nilai;
                
                RiwayatNilai rn;
                rn.kodeMK = kodeMK;
                rn.namaMK = matkul[kodeMK].nama;
                rn.sks = matkul[kodeMK].sks;
                rn.nilai = nilai;
                rn.semester = semester;
                
                akun[nim].riwayatNilai.push_back(rn);
            }
        }
        inNilai.close();
    }
}
