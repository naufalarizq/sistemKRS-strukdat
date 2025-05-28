#include "student_manager.h"
#include "globals.h"
#include "utils.h"
#include "data_manager.h"
#include "course_manager.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

void registerAkun() {
    printHeader();
    cout << "\033[36m" << "== REGISTRASI AKUN MAHASISWA ==" << "\033[0m" << endl;
    
    string nim, nama, pass, prodi;
    int semester;
    
    cout << "Masukkan NIM: "; cin >> nim;
    if (akun.find(nim) != akun.end()) {
        cout << "\033[31m" << "Akun dengan NIM tersebut sudah terdaftar." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    cin.ignore();
    cout << "Masukkan Nama Lengkap: "; getline(cin, nama);
    cout << "Masukkan Program Studi: "; getline(cin, prodi);
    
    semester = getNumericInput<int>("Masukkan Semester (1-8): ");
    if (semester < 1 || semester > 8) {
        cout << "\033[31m" << "Semester harus antara 1-8." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "Masukkan Password: "; getline(cin, pass);
    if (pass.length() < 6) {
        cout << "\033[31m" << "Password minimal 6 karakter." << "\033[0m" << endl;
        pressEnterToContinue();
        return;
    }

    Mahasiswa mhs;
    mhs.nim = nim;
    mhs.nama = nama;
    mhs.password_hash = hashPassword(pass);
    mhs.prodi = prodi;
    mhs.semester = semester;
    
    // Set max SKS berdasarkan semester
    if (semester <= 2) mhs.max_sks = 20;
    else mhs.max_sks = 24;
    
    akun[nim] = mhs;
    saveDataToFile();

    cout << "\033[32m" << "Registrasi berhasil. Silakan login." << "\033[0m" << endl;
    pressEnterToContinue();
}

void login() {
    printHeader();
    cout << "\033[36m" << "== LOGIN ==" << "\033[0m" << endl;
    
    string nim, pass;
    cout << "NIM: "; cin >> nim;
    cin.ignore();
    cout << "Password: "; getline(cin, pass);

    auto it = akun.find(nim);
    if (it != akun.end() && it->second.password_hash == hashPassword(pass)) {
        sesi = &it->second;
        isAdmin = (nim == "admin");
        cout << "\033[32m" << "Login berhasil. Selamat datang, " << sesi->nama << "!" << "\033[0m" << endl;
        pressEnterToContinue();
        menuAdmin();
    } else {
        cout << "\033[31m" << "Login gagal. NIM atau password salah." << "\033[0m" << endl;
        pressEnterToContinue();
    }
}

void menuAdmin() {
    int pilih;
    do {
        printHeader();
        cout << "\033[36m" << "== MENU ADMINISTRATOR ==" << "\033[0m" << endl;
        cout << "1. Kelola Mata Kuliah" << endl;
        cout << "2. Kelola Mahasiswa" << endl;
        cout << "3. Input Nilai" << endl;
        cout << "4. Laporan Pengisian KRS" << endl;
        cout << "5. Kembali ke Menu Utama" << endl;
        cout << "Pilih: ";
        cin >> pilih;
        
        switch (pilih) {
            case 1: {
                // Menu Kelola Mata Kuliah
                int subPilih;
                do {
                    printHeader();
                    cout << "\033[36m" << "== KELOLA MATA KULIAH ==" << "\033[0m" << endl;
                    cout << "1. Lihat Semua Mata Kuliah" << endl;
                    cout << "2. Tambah Mata Kuliah" << endl;
                    cout << "3. Edit Mata Kuliah" << endl;
                    cout << "4. Hapus Mata Kuliah" << endl;
                    cout << "5. Kembali" << endl;
                    cout << "Pilih: ";
                    cin >> subPilih;
                    
                    switch (subPilih) {
                        case 1:
                            tampilMK(false);
                            break;
                        case 2: {
                            printHeader();
                            cout << "\033[36m" << "== TAMBAH MATA KULIAH ==" << "\033[0m" << endl;
                            
                            MataKuliah mk;
                            cout << "Kode MK: "; cin >> mk.kode;
                            
                            if (matkul.find(mk.kode) != matkul.end()) {
                                cout << "\033[31m" << "Mata kuliah dengan kode tersebut sudah ada." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            cin.ignore();
                            cout << "Nama MK: "; getline(cin, mk.nama);
                            
                            mk.sks = getNumericInput<int>("SKS: ");
                            mk.kuota_max = getNumericInput<int>("Kuota Maksimum: ");
                            mk.kuota_terisi = 0;
                            
                            // Tambahkan prasyarat jika ada
                            string tambahPrasyarat;
                            cout << "Tambahkan Prasyarat? (y/n): "; cin >> tambahPrasyarat;
                            
                            if (tambahPrasyarat == "y" || tambahPrasyarat == "Y") {
                                while (true) {
                                    string kodePrasyarat;
                                    cout << "Kode MK Prasyarat (0 untuk selesai): ";
                                    cin >> kodePrasyarat;
                                    
                                    if (kodePrasyarat == "0") break;
                                    
                                    if (matkul.find(kodePrasyarat) == matkul.end()) {
                                        cout << "\033[31m" << "Mata kuliah prasyarat tidak ditemukan." << "\033[0m" << endl;
                                        continue;
                                    }
                                    
                                    mk.prasyarat.push_back(kodePrasyarat);
                                    cout << "\033[32m" << "Prasyarat " << kodePrasyarat << " ditambahkan." << "\033[0m" << endl;
                                }
                            }
                            
                            matkul[mk.kode] = mk;
                            saveDataToFile();
                            
                            cout << "\033[32m" << "Mata kuliah berhasil ditambahkan." << "\033[0m" << endl;
                            pressEnterToContinue();
                            break;
                        }
                        case 3: {
                            printHeader();
                            cout << "\033[36m" << "== EDIT MATA KULIAH ==" << "\033[0m" << endl;
                            
                            string kode;
                            cout << "Masukkan Kode MK yang akan diedit: "; cin >> kode;
                            
                            if (matkul.find(kode) == matkul.end()) {
                                cout << "\033[31m" << "Mata kuliah tidak ditemukan." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            MataKuliah& mk = matkul[kode];
                            cout << "Data Mata Kuliah:" << endl;
                            cout << "Kode: " << mk.kode << endl;
                            cout << "Nama: " << mk.nama << endl;
                            cout << "SKS: " << mk.sks << endl;
                            cout << "Kuota: " << mk.kuota_terisi << "/" << mk.kuota_max << endl;
                            
                            cout << "Prasyarat: ";
                            for (size_t i = 0; i < mk.prasyarat.size(); i++) {
                                cout << mk.prasyarat[i];
                                if (i < mk.prasyarat.size() - 1) cout << ", ";
                            }
                            cout << endl;
                            
                            cin.ignore();
                            cout << "\nEdit Nama MK (kosongkan jika tidak ingin mengubah): ";
                            string nama;
                            getline(cin, nama);
                            if (!nama.empty()) mk.nama = nama;
                            
                            cout << "Edit SKS (0 jika tidak ingin mengubah): ";
                            int sks;
                            cin >> sks;
                            if (sks > 0) mk.sks = sks;
                            
                            cout << "Edit Kuota Maksimum (0 jika tidak ingin mengubah): ";
                            int kuota;
                            cin >> kuota;
                            if (kuota > 0) mk.kuota_max = kuota;
                            
                            // Edit prasyarat jika diperlukan
                            string editPrasyarat;
                            cout << "Edit Prasyarat? (y/n): "; cin >> editPrasyarat;
                            
                            if (editPrasyarat == "y" || editPrasyarat == "Y") {
                                mk.prasyarat.clear();
                                while (true) {
                                    string kodePrasyarat;
                                    cout << "Kode MK Prasyarat (0 untuk selesai): ";
                                    cin >> kodePrasyarat;
                                    
                                    if (kodePrasyarat == "0") break;
                                    
                                    if (matkul.find(kodePrasyarat) == matkul.end()) {
                                        cout << "\033[31m" << "Mata kuliah prasyarat tidak ditemukan." << "\033[0m" << endl;
                                        continue;
                                    }
                                    
                                    mk.prasyarat.push_back(kodePrasyarat);
                                    cout << "\033[32m" << "Prasyarat " << kodePrasyarat << " ditambahkan." << "\033[0m" << endl;
                                }
                            }
                            
                            saveDataToFile();
                            cout << "\033[32m" << "Mata kuliah berhasil diupdate." << "\033[0m" << endl;
                            pressEnterToContinue();
                            break;
                        }
                        case 4: {
                            printHeader();
                            cout << "\033[36m" << "== HAPUS MATA KULIAH ==" << "\033[0m" << endl;
                            
                            string kode;
                            cout << "Masukkan Kode MK yang akan dihapus: "; cin >> kode;
                            
                            if (matkul.find(kode) == matkul.end()) {
                                cout << "\033[31m" << "Mata kuliah tidak ditemukan." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            // Cek apakah mata kuliah sedang digunakan dalam KRS
                            bool digunakan = false;
                            for (auto& pair : akun) {
                                Mahasiswa& mhs = pair.second;
                                for (const string& mkKode : mhs.krs) {
                                    if (mkKode == kode) {
                                        digunakan = true;
                                        break;
                                    }
                                }
                                if (digunakan) break;
                            }
                            
                            if (digunakan) {
                                cout << "\033[31m" << "Mata kuliah sedang digunakan dalam KRS mahasiswa." << "\033[0m" << endl;
                                cout << "Hapus mata kuliah dari KRS semua mahasiswa? (y/n): ";
                                
                                string konfirmasi;
                                cin >> konfirmasi;
                                
                                if (konfirmasi != "y" && konfirmasi != "Y") {
                                    cout << "\033[33m" << "Penghapusan dibatalkan." << "\033[0m" << endl;
                                    pressEnterToContinue();
                                    break;
                                }
                                
                                // Hapus mata kuliah dari KRS semua mahasiswa
                                for (auto& pair : akun) {
                                    Mahasiswa& mhs = pair.second;
                                    mhs.krs.erase(remove(mhs.krs.begin(), mhs.krs.end(), kode), mhs.krs.end());
                                }
                            }
                            
                            // Cek jika mata kuliah digunakan sebagai prasyarat
                            vector<string> digunakan_sebagai_prasyarat;
                            for (auto& pair : matkul) {
                                for (const string& prasyarat : pair.second.prasyarat) {
                                    if (prasyarat == kode) {
                                        digunakan_sebagai_prasyarat.push_back(pair.first);
                                        break;
                                    }
                                }
                            }
                            
                            if (!digunakan_sebagai_prasyarat.empty()) {
                                cout << "\033[33m" << "Mata kuliah digunakan sebagai prasyarat untuk: ";
                                for (size_t i = 0; i < digunakan_sebagai_prasyarat.size(); i++) {
                                    cout << digunakan_sebagai_prasyarat[i];
                                    if (i < digunakan_sebagai_prasyarat.size() - 1) cout << ", ";
                                }
                                cout << "\033[0m" << endl;
                                
                                cout << "Hapus prasyarat dari mata kuliah tersebut? (y/n): ";
                                string konfirmasi;
                                cin >> konfirmasi;
                                
                                if (konfirmasi != "y" && konfirmasi != "Y") {
                                    cout << "\033[33m" << "Penghapusan dibatalkan." << "\033[0m" << endl;
                                    pressEnterToContinue();
                                    break;
                                }
                                
                                // Hapus prasyarat dari mata kuliah lain
                                for (auto& pair : matkul) {
                                    pair.second.prasyarat.erase(
                                        remove(pair.second.prasyarat.begin(), pair.second.prasyarat.end(), kode),
                                        pair.second.prasyarat.end()
                                    );
                                }
                            }
                            
                            // Hapus mata kuliah
                            matkul.erase(kode);
                            saveDataToFile();
                            
                            cout << "\033[32m" << "Mata kuliah berhasil dihapus." << "\033[0m" << endl;
                            pressEnterToContinue();
                            break;
                        }
                        case 5:
                            break;
                        default:
                            cout << "\033[31m" << "Pilihan tidak valid." << "\033[0m" << endl;
                            pressEnterToContinue();
                    }
                } while (subPilih != 5);
                break;
            }
            case 2: {
                // Menu Kelola Mahasiswa
                int subPilih;
                do {
                    printHeader();
                    cout << "\033[36m" << "== KELOLA MAHASISWA ==" << "\033[0m" << endl;
                    cout << "1. Lihat Semua Mahasiswa" << endl;
                    cout << "2. Tambah Mahasiswa" << endl;
                    cout << "3. Edit Mahasiswa" << endl;
                    cout << "4. Hapus Mahasiswa" << endl;
                    cout << "5. Reset Password" << endl;
                    cout << "6. Kembali" << endl;
                    cout << "Pilih: ";
                    cin >> subPilih;
                    
                    switch (subPilih) {
                        case 1: {
                            printHeader();
                            cout << "\033[36m" << "== DAFTAR MAHASISWA ==" << "\033[0m" << endl;
                            
                            cout << "┌────────────┬────────────────────────────┬────────────────┬─────────┐" << endl;
                            cout << "│ NIM        │ Nama                       │ Program Studi  │ Semester│" << endl;
                            cout << "├────────────┼────────────────────────────┼────────────────┼─────────┤" << endl;
                            
                            for (const auto& pair : akun) {
                                if (pair.first != "admin") {
                                    cout << "│ " << setw(10) << left << pair.second.nim 
                                         << "│ " << setw(26) << left << pair.second.nama 
                                         << "│ " << setw(14) << left << pair.second.prodi 
                                         << "│ " << setw(7) << right << pair.second.semester << " │" << endl;
                                }
                            }
                            cout << "└────────────┴────────────────────────────┴────────────────┴─────────┘" << endl;
                            
                            pressEnterToContinue();
                            break;
                        }
                        case 2: {
                            // Tambah Mahasiswa (sama dengan registerAkun)
                            registerAkun();
                            break;
                        }
                        case 3: {
                            printHeader();
                            cout << "\033[36m" << "== EDIT MAHASISWA ==" << "\033[0m" << endl;
                            
                            string nim;
                            cout << "Masukkan NIM mahasiswa yang akan diedit: "; cin >> nim;
                            
                            if (nim == "admin" || akun.find(nim) == akun.end()) {
                                cout << "\033[31m" << "Mahasiswa tidak ditemukan." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            Mahasiswa& mhs = akun[nim];
                            cout << "Data Mahasiswa:" << endl;
                            cout << "NIM: " << mhs.nim << endl;
                            cout << "Nama: " << mhs.nama << endl;
                            cout << "Program Studi: " << mhs.prodi << endl;
                            cout << "Semester: " << mhs.semester << endl;
                            cout << "Max SKS: " << mhs.max_sks << endl;
                            
                            cin.ignore();
                            cout << "\nEdit Nama (kosongkan jika tidak ingin mengubah): ";
                            string nama;
                            getline(cin, nama);
                            if (!nama.empty()) mhs.nama = nama;
                            
                            cout << "Edit Program Studi (kosongkan jika tidak ingin mengubah): ";
                            string prodi;
                            getline(cin, prodi);
                            if (!prodi.empty()) mhs.prodi = prodi;
                            
                            cout << "Edit Semester (0 jika tidak ingin mengubah): ";
                            int semester;
                            cin >> semester;
                            if (semester > 0) mhs.semester = semester;
                            
                            cout << "Edit Max SKS (0 jika tidak ingin mengubah): ";
                            int maxSks;
                            cin >> maxSks;
                            if (maxSks > 0) mhs.max_sks = maxSks;
                            
                            saveDataToFile();
                            cout << "\033[32m" << "Data mahasiswa berhasil diupdate." << "\033[0m" << endl;
                            pressEnterToContinue();
                            break;
                        }
                        case 4: {
                            printHeader();
                            cout << "\033[36m" << "== HAPUS MAHASISWA ==" << "\033[0m" << endl;
                            
                            string nim;
                            cout << "Masukkan NIM mahasiswa yang akan dihapus: "; cin >> nim;
                            
                            if (nim == "admin" || akun.find(nim) == akun.end()) {
                                cout << "\033[31m" << "Mahasiswa tidak ditemukan." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            cout << "Yakin ingin menghapus mahasiswa " << nim << " - " << akun[nim].nama << "? (y/n): ";
                            string konfirmasi;
                            cin >> konfirmasi;
                            
                            if (konfirmasi == "y" || konfirmasi == "Y") {
                                // Kurangi kuota_terisi untuk setiap mata kuliah yang diambil
                                for (const string& kode : akun[nim].krs) {
                                    matkul[kode].kuota_terisi--;
                                }
                                
                                akun.erase(nim);
                                saveDataToFile();
                                
                                cout << "\033[32m" << "Mahasiswa berhasil dihapus." << "\033[0m" << endl;
                            } else {
                                cout << "\033[33m" << "Penghapusan dibatalkan." << "\033[0m" << endl;
                            }
                            
                            pressEnterToContinue();
                            break;
                        }
                        case 5: {
                            printHeader();
                            cout << "\033[36m" << "== RESET PASSWORD MAHASISWA ==" << "\033[0m" << endl;
                            
                            string nim;
                            cout << "Masukkan NIM mahasiswa yang akan direset passwordnya: "; cin >> nim;
                            
                            if (akun.find(nim) == akun.end()) {
                                cout << "\033[31m" << "Mahasiswa tidak ditemukan." << "\033[0m" << endl;
                                pressEnterToContinue();
                                break;
                            }
                            
                            string newPass = nim;  // Password default adalah NIM
                            akun[nim].password_hash = hashPassword(newPass);
                            saveDataToFile();
                            
                            cout << "\033[32m" << "Password mahasiswa " << nim << " berhasil direset." << "\033[0m" << endl;
                            cout << "Password baru: " << newPass << endl;
                            pressEnterToContinue();
                            break;
                        }
                        case 6:
                            break;
                        default:
                            cout << "\033[31m" << "Pilihan tidak valid." << "\033[0m" << endl;
                            pressEnterToContinue();
                    }
                } while (subPilih != 6);
                break;
            }
            case 3: {
                // Input Nilai
                printHeader();
                cout << "\033[36m" << "== INPUT NILAI MAHASISWA ==" << "\033[0m" << endl;
                
                string nim;
                cout << "Masukkan NIM mahasiswa: "; cin >> nim;
                
                if (akun.find(nim) == akun.end()) {
                    cout << "\033[31m" << "Mahasiswa tidak ditemukan." << "\033[0m" << endl;
                    pressEnterToContinue();
                    break;
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
                
                if (kodeMK == "0") break;
                
                if (find(mhs.krs.begin(), mhs.krs.end(), kodeMK) == mhs.krs.end()) {
                    cout << "\033[31m" << "Mata kuliah tidak terdaftar dalam KRS mahasiswa." << "\033[0m" << endl;
                    pressEnterToContinue();
                    break;
                }
                
                char nilai;
                cout << "Masukkan Nilai (A/B/C/D/E): "; cin >> nilai;
                
                if (nilai != 'A' && nilai != 'B' && nilai != 'C' && nilai != 'D' && nilai != 'E') {
                    cout << "\033[31m" << "Nilai tidak valid." << "\033[0m" << endl;
                    pressEnterToContinue();
                    break;
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
                break;
            }
            case 4: {
                // Laporan Pengisian KRS
                printHeader();
                cout << "\033[36m" << "== LAPORAN PENGISIAN KRS ==" << "\033[0m" << endl;
                
                // Tampilkan statistik pengambilan mata kuliah
                cout << "Statistik Pengambilan Mata Kuliah:" << endl;
                cout << "┌───────┬─────────────────────────────────┬─────┬────────┬───────────┐" << endl;
                cout << "│ Kode  │ Nama Mata Kuliah               │ SKS │ Kuota  │ Persentase│" << endl;
                cout << "├───────┼─────────────────────────────────┼─────┼────────┼───────────┤" << endl;
                
                vector<pair<string, float>> utilization;
                for (const auto& pair : matkul) {
                    float persentase = (static_cast<float>(pair.second.kuota_terisi) / pair.second.kuota_max) * 100;
                    utilization.push_back({pair.first, persentase});
                }
                
                sort(utilization.begin(), utilization.end(), [](const pair<string, float>& a, const pair<string, float>& b) {
                    return a.second > b.second; // Sort by percentage (descending)
                });
                
                for (const auto& pair : utilization) {
                    const MataKuliah& mk = matkul[pair.first];
                    string status;
                    
                    if (pair.second >= 90) {
                        status = "\033[31m" + to_string(static_cast<int>(pair.second)) + "%" + "\033[0m";
                    } else if (pair.second >= 70) {
                        status = "\033[33m" + to_string(static_cast<int>(pair.second)) + "%" + "\033[0m";
                    } else {
                        status = "\033[32m" + to_string(static_cast<int>(pair.second)) + "%" + "\033[0m";
                    }
                    
                    cout << "│ " << setw(5) << left << mk.kode 
                         << "│ " << setw(31) << left << mk.nama 
                         << "│ " << setw(3) << right << mk.sks 
                         << " │ " << setw(3) << right << mk.kuota_terisi << "/" << setw(2) << left << mk.kuota_max 
                         << " │ " << setw(9) << left << status << " │" << endl;
                }
                cout << "└───────┴─────────────────────────────────┴─────┴────────┴───────────┘" << endl;
                
                // Tampilkan jumlah mahasiswa per semester
                cout << "\nJumlah Mahasiswa per Semester:" << endl;
                cout << "┌─────────┬─────────────┐" << endl;
                cout << "│ Semester│ Jumlah Mhs  │" << endl;
                cout << "├─────────┼─────────────┤" << endl;
                
                map<int, int> mhsPerSemester;
                for (const auto& pair : akun) {
                    if (pair.first != "admin") {
                        mhsPerSemester[pair.second.semester]++;
                    }
                }
                
                for (const auto& pair : mhsPerSemester) {
                    cout << "│ " << setw(7) << right << pair.first 
                         << " │ " << setw(11) << right << pair.second << " │" << endl;
                }
                cout << "└─────────┴─────────────┘" << endl;
                
                // Tampilkan jumlah pengambilan mata kuliah
                cout << "\nJumlah Pengambilan Mata Kuliah per Mahasiswa:" << endl;
                cout << "┌────────────┬─────────────────────────────────┬───────────┐" << endl;
                cout << "│ NIM        │ Nama                            │ Jumlah MK │" << endl;
                cout << "├────────────┼─────────────────────────────────┼───────────┤" << endl;
                
                vector<pair<string, int>> krsCount;
                for (const auto& pair : akun) {
                    if (pair.first != "admin") {
                        krsCount.push_back({pair.first, pair.second.krs.size()});
                    }
                }
                
                sort(krsCount.begin(), krsCount.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
                    return a.second > b.second; // Sort by count (descending)
                });
                
                for (const auto& pair : krsCount) {
                    cout << "│ " << setw(10) << left << pair.first 
                         << "│ " << setw(33) << left << akun[pair.first].nama 
                         << "│ " << setw(9) << right << pair.second << " │" << endl;
                }
                cout << "└────────────┴─────────────────────────────────┴───────────┘" << endl;
                
                pressEnterToContinue();
                break;
            }
            case 5:
                break;
            default:
                cout << "\033[31m" << "Pilihan tidak valid." << "\033[0m" << endl;
                pressEnterToContinue();
        }
    } while (pilih != 5);
}
