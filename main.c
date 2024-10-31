#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <ctype.h>
#include <assert.h>

#define SHA_LENGTH 32

uint8_t hex_to_byte(unsigned char h1, unsigned char h2) {
    uint8_t byte = 0;
    if (h1 >= '0' && h1 <= '9') byte += (h1 - '0') * 16;
    else if (h1 >= 'a' && h1 <= 'f') byte += (h1 - 'a' + 10) * 16;
    else if (h1 >= 'A' && h1 <= 'F') byte += (h1 - 'A' + 10) * 16;

    if (h2 >= '0' && h2 <= '9') byte += (h2 - '0');
    else if (h2 >= 'a' && h2 <= 'f') byte += (h2 - 'a' + 10);
    else if (h2 >= 'A' && h2 <= 'F') byte += (h2 - 'A' + 10);

    return byte;
}

void hexstr_to_hash(char hexstr[], unsigned char hash[32]) {
    for (int i = 0; i < 32; i++) {
        hash[i] = hex_to_byte(hexstr[2 * i], hexstr[2 * i + 1]);
    }
}

int8_t check_password(char password[], unsigned char given_hash[32]) {
    unsigned char hash[32];
    SHA256((unsigned char*)password, strlen(password), hash);

    for (int i = 0; i < 32; i++) {
        if (hash[i] != given_hash[i]) {
            return 0;
        }
    }
    return 1;
}

int8_t crack_password(char password[], unsigned char given_hash[]) {

    if (check_password(password, given_hash)) {
        return 1;
    }

    for (int i = 0; i < strlen(password); i++) {
        if (isalpha(password[i])) {
            char original_char = password[i];


            password[i] = islower(password[i]) ? toupper(password[i]) : tolower(password[i]);

            if (check_password(password, given_hash)) {
                return 1;
            }

            password[i] = original_char;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <SHA256 hash>\n", argv[0]);
        return 1;
    }
    unsigned char given_hash[32];
    hexstr_to_hash(argv[1], given_hash);
    char password[256];
    while (fgets(password, sizeof(password), stdin) != NULL) {

        password[strcspn(password, "\n")] = 0;

        if (crack_password(password, given_hash)) {
            printf("Found password: SHA256(%s) = %s\n", password, argv[1]);
            return 0;
        }
    }

    printf("Did not find a matching password\n");
    return 1;
}


