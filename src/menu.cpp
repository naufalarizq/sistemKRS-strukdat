#include "menu.h"
#include "student_manager.h"
#include "course_manager.h"
#include "grade_manager.h"
#include "krs_manager.h"
#include "utils.h"
#include <iostream>

using namespace std;

void mainMenu() {
    int pilih;
    do {
        // Add explicit declaration for printHeader function
        extern void printHeader();
        printHeader();
        cout << "\033[36m" << "== SISTEM KRS ==" << "\033[0m" << endl;
        cout << "1. Registrasi Akun" << endl;
        cout << "2. Login" << endl;
        cout << "3. Keluar" << endl;
        cout << "Pilih: ";
        cin >> pilih;
        
        switch (pilih) {
            case 1:
                registerAkun();
                break;
            case 2:
                login();
                break;
            case 3:
                cout << "Terima kasih telah menggunakan sistem KRS." << endl;
                break;
            default:
                cout << "\033[31m" << "Pilihan tidak valid." << "\033[0m" << endl;
                pressEnterToContinue();
        }
    } while (pilih != 3);
}
