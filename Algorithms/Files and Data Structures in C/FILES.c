#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void Display(const char fname[]) {
    FILE *f = fopen(fname, "r");
    char str1[100];

    if (f == NULL) {
        printf("Error: Could not open %s for reading.\n", fname);
        return;
    }

    while (fgets(str1, sizeof(str1), f)) {
        printf("Content of the line: %s", str1);
    }

    fclose(f);
}

void Concatenate(const char fname1[], const char fname2[], const char fname3[]) {
    FILE *f1, *f2, *f3;
    char str[100];
    printf("Attempting to create: %s\n", fname3);
    f3 = fopen(fname3, "w");
    if (f3 == NULL) {
        printf("Error: Could not open %s for writing.\n", fname3);
        return;
    }

    f1 = fopen(fname1, "r");
    if (f1 == NULL) {
        printf("Error: Could not open %s for reading.\n", fname1);
        fclose(f3);
        return;
    }

    while (fgets(str, sizeof(str), f1)) {
        printf("Content of line: %s", str);
        fputs(str, f3);
    }
    fclose(f1);

    f2 = fopen(fname2, "r");
    if (f2 == NULL) {
        printf("Error: Could not open %s for reading.\n", fname2);
        fclose(f3);
        return;
    }

    while (fgets(str, sizeof(str), f2)) {
        printf("Content of line: %s", str);
        fputs(str, f3);
    }

    fclose(f2);
    fclose(f3);
    printf("\nConcatenation complete!\n");
}

int NBocc(char fname[]){
    FILE *f;
    char temp[100];
    int nb_occ = 0;
    char word[100];

    printf("give the word to search :\n");
    scanf("%s",word);

    toLowerCase(word);
    f = fopen(fname, "r");
    if (f == NULL) {
        printf("Error: Could not open file %s\n", fname);
        return 1;
    }
    while (fgets(temp, sizeof(temp), f)){
        char *spec = strtok(temp, " ,.-\n\r\t?!:;");

        while (spec != NULL){
            char file_word[50];
            strcpy(file_word, spec);
            toLowerCase(file_word);

            printf("Comparing '%s' with '%s'\n", word, file_word);

            if (strcmp(word, file_word) == 0){
                nb_occ++;
            }
            spec = strtok(NULL, " ,.-\n\r\t?!:;");

        }
    }
    fclose(f);
    printf("The word '%s' appears %d times in the file '%s'.\n", word, nb_occ, fname);

    return nb_occ;

}

int num_lines(char fname[]){
    FILE *f = fopen(fname, "r");
    if (f == NULL) {
        printf("Error: Could not open file %s\n", fname);
        return 1;
    }
    int numb_lines = 0;
    int caractere = '\n';

    while ((caractere = fgetc(f)) != EOF){
        if (caractere == '\n') {
            numb_lines++;
        }
    }

    fclose(f);
    return numb_lines;
}


int encrypt_text_file(const char *input_file, const char *output_file, const char *key) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Erreur ouverture fichier texte d'entree");
        return -1;
    }

    FILE *out = fopen(output_file, "w");

    if (out == NULL) {
        perror("Erreur ouverture fichier texte de sortie");
        fclose(in);
        return -1;
    }

    char line[1024];
    size_t i;

    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line);
        for (i = 0; i < len; i++) {
            unsigned char c = (unsigned char)line[i];
            unsigned char k = (unsigned char)key[i % strlen(key)];
            line[i] = (char)(c + k);  
        }
        fwrite(line, 1, len, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}

int decrypt_text_file(const char *input_file, const char *output_file, const char *key) {
    FILE *in = fopen(input_file, "r");
    if (in == NULL) {
        perror("Erreur ouverture fichier texte d'entree");
        return -1;
    }

    FILE *out = fopen(output_file, "w");
    if (out == NULL) {
        perror("Erreur ouverture fichier texte de sortie");
        fclose(in);
        return -1;
    }
    char line[1024];
    size_t i;

    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line);
        for (i = 0; i < len; i++) {
            unsigned char c = (unsigned char)line[i];
            unsigned char k = (unsigned char)key[i % strlen(key)];
            line[i] = (char)(c - k);  
        }
        fwrite(line, 1, len, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}

void compareFiles(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return;
    }
    char line1[100], line2[100];
    int lineNumber = 1, different = 0;
    while (1) {
        char *l1 = fgets(line1, sizeof line1, f1);
        char *l2 = fgets(line2, sizeof line2, f2);
        if (l1 == NULL && l2 == NULL) break;
        if ((l1 == NULL && l2 != NULL) || (l1 != NULL && l2 == NULL) || (strcmp(line1, line2) != 0)) {
            different = 1;
            printf("The files are different");
        }
        lineNumber++;
    }
    if (!different) printf("The two files are identical.\n");
    fclose(f1);
    fclose(f2);
}




int main(void) {
    Concatenate("myfile.txt","myfile2.txt","concat.txt");
}



