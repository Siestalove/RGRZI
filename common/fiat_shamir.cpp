#include "fiat_shamir.h"
#include <sstream>
#include <iomanip>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <ctime>
#include <iostream>

mt19937 FiatShamir::gen(random_device{}());
uniform_int_distribution<int> FiatShamir::bit_dist(0, 1);

string FiatShamir::generateLargePrime(int bits) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* prime = BN_new();
    
    // Генерируем простое число
    BN_generate_prime_ex(prime, bits, 1, NULL, NULL, NULL);
    
    char* prime_str = BN_bn2hex(prime);
    string result(prime_str);
    OPENSSL_free(prime_str);
    
    BN_free(prime);
    BN_CTX_free(ctx);
    
    return result;
}

FiatShamir::KeyPair FiatShamir::generateKeys(const string& login, int bits) {
    KeyPair keys;
    keys.login = login;
    
    BN_CTX* ctx = BN_CTX_new();
    
    // Генерируем два простых числа p и q
    BIGNUM* p = BN_new();
    BIGNUM* q = BN_new();
    BIGNUM* n = BN_new();
    BIGNUM* s = BN_new();
    BIGNUM* v = BN_new();
    
    BN_generate_prime_ex(p, bits/2, 1, NULL, NULL, NULL);
    BN_generate_prime_ex(q, bits/2, 1, NULL, NULL, NULL);
    
    // Вычисляем n = p * q
    BN_mul(n, p, q, ctx);
    
    // Генерируем секретное число s (1 < s < n)
    BN_rand_range(s, n);
    if (BN_is_one(s) || BN_is_zero(s)) {
        BN_add_word(s, 2);
    }
    
    // Вычисляем v = s^2 mod n
    BN_mod_sqr(v, s, n, ctx);
    
    // Конвертируем в строки
    char* n_str = BN_bn2hex(n);
    char* s_str = BN_bn2hex(s);
    char* v_str = BN_bn2hex(v);
    
    keys.n = string(n_str);
    keys.secret = string(s_str);
    keys.public_key = string(v_str);
    
    OPENSSL_free(n_str);
    OPENSSL_free(s_str);
    OPENSSL_free(v_str);
    
    BN_free(p);
    BN_free(q);
    BN_free(n);
    BN_free(s);
    BN_free(v);
    BN_CTX_free(ctx);
    
    return keys;
}

string FiatShamir::modExp(const string& base, const string& exp, const string& mod) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_base = BN_new();
    BIGNUM* bn_exp = BN_new();
    BIGNUM* bn_mod = BN_new();
    BIGNUM* result = BN_new();
    
    BN_hex2bn(&bn_base, base.c_str());
    BN_hex2bn(&bn_exp, exp.c_str());
    BN_hex2bn(&bn_mod, mod.c_str());
    
    BN_mod_exp(result, bn_base, bn_exp, bn_mod, ctx);
    
    char* result_str = BN_bn2hex(result);
    string result_string(result_str);
    OPENSSL_free(result_str);
    
    BN_free(bn_base);
    BN_free(bn_exp);
    BN_free(bn_mod);
    BN_free(result);
    BN_CTX_free(ctx);
    
    return result_string;
}

string FiatShamir::modMul(const string& a, const string& b, const string& mod) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* bn_a = BN_new();
    BIGNUM* bn_b = BN_new();
    BIGNUM* bn_mod = BN_new();
    BIGNUM* result = BN_new();
    
    BN_hex2bn(&bn_a, a.c_str());
    BN_hex2bn(&bn_b, b.c_str());
    BN_hex2bn(&bn_mod, mod.c_str());
    
    BN_mod_mul(result, bn_a, bn_b, bn_mod, ctx);
    
    char* result_str = BN_bn2hex(result);
    string result_string(result_str);
    OPENSSL_free(result_str);
    
    BN_free(bn_a);
    BN_free(bn_b);
    BN_free(bn_mod);
    BN_free(result);
    BN_CTX_free(ctx);
    
    return result_string;
}

int FiatShamir::generateChallenge() {
    return bit_dist(gen);
}

bool FiatShamir::verifyAuthentication(const AuthData& auth, const string& public_key, const string& n) {
    // Проверяем условие: y^2 ≡ x * v^e (mod n)
    string y_squared = modExp(auth.y, "2", n);
    string v_exp_e = modExp(public_key, to_string(auth.e), n);
    string x_mul_v = modMul(auth.x, v_exp_e, n);
    
    return y_squared == x_mul_v;
}

string FiatShamir::stringToNumber(const string& str) {
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* num = BN_new();
    
    // Используем хеш строки как число
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);
    
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, num);
    
    char* num_str = BN_bn2hex(num);
    string result(num_str);
    OPENSSL_free(num_str);
    
    BN_free(num);
    BN_CTX_free(ctx);
    
    return result;
}

string FiatShamir::numberToString(const string& num) {
    // Для простоты возвращаем hex представление
    return num;
}