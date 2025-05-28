#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ctime>

void clearScreen();
void printHeader();
void pressEnterToContinue();
std::string getCurrentDateTime();
std::string formatTimeStamp(time_t t);
std::string hashPassword(const std::string& pw);
float nilaiMutuToAngka(char nilai);

template <typename T>
T getNumericInput(const std::string& prompt);

#endif // UTILS_H
