#pragma once
#include <string>
#include <vector>
#include <random>

using namespace std;

class FiatShamir {
private:
    static mt19937 gen;
    static uniform_int_distribution<int> bit_dist;

public:
    struct KeyPair {
        string login;
        string secret;  // s
        string public_key;  // v = s^2 mod n
        string n;  // модуль
    };

    struct AuthData {
        string login;
        string x;  // r^2 mod n
        string y;  // r * s^e mod n
        int e;     // вызов (0 или 1)
    };

    // Генерация больших простых чисел
    static string generateLargePrime(int bits = 512);
    
    // Генерация ключей
    static KeyPair generateKeys(const string& login, int bits = 512);
    
    // Модульное возведение в степень
    static string modExp(const string& base, const string& exp, const string& mod);
    
    // Модульное умножение
    static string modMul(const string& a, const string& b, const string& mod);
    
    // Генерация случайного вызова (0 или 1)
    static int generateChallenge();
    
    // Проверка аутентификации
    static bool verifyAuthentication(const AuthData& auth, const string& public_key, const string& n);
    
    // Конвертация строковых чисел
    static string stringToNumber(const string& str);
    static string numberToString(const string& num);
};