#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <ctime>

struct Aksi {
    std::string kodeMK;
    bool tambah;
    time_t waktu;
};

struct MataKuliah {
    std::string kode;
    std::string nama;
    int sks;
    int kuota_max;
    int kuota_terisi = 0;
    std::vector<std::string> prasyarat; // Kode MK prasyarat
    std::map<std::string, char> nilaiMutu; // NIM -> Nilai
};

struct RiwayatNilai {
    std::string kodeMK;
    std::string namaMK;
    int sks;
    char nilai;
    std::string semester;
};

struct Mahasiswa {
    std::string nim;
    std::string nama;
    std::string password_hash;
    std::string prodi;
    int semester;
    std::vector<std::string> krs;
    std::stack<Aksi> riwayat;
    std::vector<RiwayatNilai> riwayatNilai;
    int max_sks = 24; // Default max SKS
};

// unordered_map<string, Mahasiswa> akun;
// unordered_map<string, MataKuliah> matkul;
// Mahasiswa* sesi = nullptr;
// bool isAdmin = false;


#endif // MODELS_H
