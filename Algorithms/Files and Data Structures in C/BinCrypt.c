#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int encrypt_binary_file(const char *input_file, const char *output_file, const char *key) {
    FILE *in = fopen(input_file, "rb");
    if (!in) {
        perror("Erreur ouverture fichier binaire d'entree");
        return -1;
    }

    FILE *out = fopen(output_file, "wb");
    if (!out) {
        perror("Erreur ouverture fichier binaire de sortie");
        fclose(in);
        return -1;
    }


    unsigned char buffer[1024];
    size_t read_bytes;
    size_t i;

    while ((read_bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (i = 0; i < read_bytes; i++) {
            buffer[i] = (unsigned char)(buffer[i] + (unsigned char)key[i % strlen(key)]); 
        }
        fwrite(buffer, 1, read_bytes, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}


int decrypt_binary_file(const char *input_file, const char *output_file, const char *key) {
    FILE *in = fopen(input_file, "rb");
    if (!in) {
        perror("Erreur ouverture fichier binaire d'entree");
        return -1;
    }

    FILE *out = fopen(output_file, "wb");
    if (!out) {
        perror("Erreur ouverture fichier binaire de sortie");
        fclose(in);
        return -1;
    }


    unsigned char buffer[1024];
    size_t read_bytes;
    size_t i;

    while ((read_bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (i = 0; i < read_bytes; i++) {
            buffer[i] = (unsigned char)(buffer[i] - (unsigned char)key[i % strlen(key)]); 
        }
        fwrite(buffer, 1, read_bytes, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}

