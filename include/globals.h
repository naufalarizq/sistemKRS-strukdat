#ifndef GLOBALS_H
#define GLOBALS_H

#include "models.h"
#include <unordered_map>

extern std::unordered_map<std::string, Mahasiswa> akun;
extern std::unordered_map<std::string, MataKuliah> matkul;
extern Mahasiswa* sesi;
extern bool isAdmin;

#endif // GLOBALS_H
