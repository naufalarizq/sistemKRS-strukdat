#ifndef COURSE_MANAGER_H
#define COURSE_MANAGER_H

#include <string>

void daftarMataKuliah();
bool cekPrasyarat(const std::string& kodeMK);
int hitungTotalSKS();
void tambahMK();
void batalMK();
void tampilMK(bool byNama = false);
void lihatKuota();
void lihatSebaran();
void cariMataKuliah();

#endif // COURSE_MANAGER_H
