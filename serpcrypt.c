#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KEY_SIZE 32  // 256-bit key for Serpent
#define BLOCK_SIZE 16

void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Initialize the encryption operation with Serpent
    if (1 != EVP_EncryptInit_ex(ctx, EVP_serpent_cbc(), NULL, key, iv))
        handleErrors();

    // Encrypt the plaintext
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    // Finalize the encryption
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Initialize the decryption operation with Serpent
    if (1 != EVP_DecryptInit_ex(ctx, EVP_serpent_cbc(), NULL, key, iv))
        handleErrors();

    // Decrypt the ciphertext
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    // Finalize the decryption
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void print_hex(unsigned char *data, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

int hex_to_bytes(const char *hex_str, unsigned char *byte_array, int max_len) {
    int len = strlen(hex_str);
    if (len % 2 != 0 || len / 2 > max_len) return -1;

    for (int i = 0; i < len / 2; i++) {
        sscanf(hex_str + 2 * i, "%2hhx", &byte_array[i]);
    }

    return len / 2;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s -e <plaintext> <key (hex)> <iv (hex)> (to encrypt)\n", argv[0]);
        fprintf(stderr, "       %s -d <ciphertext (hex)> <key (hex)> <iv (hex)> (to decrypt)\n", argv[0]);
        return 1;
    }

    // Variables for encryption and decryption
    unsigned char key[KEY_SIZE];
    unsigned char iv[BLOCK_SIZE];
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];
    unsigned char plaintext[128];

    // Convert the provided key and IV from hex to bytes
    if (hex_to_bytes(argv[3], key, sizeof(key)) == -1) {
        fprintf(stderr, "Invalid key format\n");
        return 1;
    }
    if (hex_to_bytes(argv[4], iv, sizeof(iv)) == -1) {
        fprintf(stderr, "Invalid IV format\n");
        return 1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        // Encrypt the provided plaintext
        strncpy((char *)plaintext, argv[2], sizeof(plaintext) - 1);
        plaintext[sizeof(plaintext) - 1] = '\0';  // Ensure null termination

        int ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);

        // Output the encrypted text in hex format
        printf("Ciphertext (hex): ");
        print_hex(ciphertext, ciphertext_len);
    }
    else if (strcmp(argv[1], "-d") == 0) {
        // Assuming the input is in hex format (ciphertext)
        char *hex_ciphertext = argv[2];

        int ciphertext_len = hex_to_bytes(hex_ciphertext, ciphertext, sizeof(ciphertext));
        if (ciphertext_len == -1) {
            fprintf(stderr, "Invalid ciphertext format\n");
            return 1;
        }

        // Decrypt the ciphertext
        int decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);
        decryptedtext[decryptedtext_len] = '\0';  // Null-terminate the decrypted text

        // Output the decrypted text
        printf("Decrypted text: %s\n", decryptedtext);
    }
    else {
        fprintf(stderr, "Invalid option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
