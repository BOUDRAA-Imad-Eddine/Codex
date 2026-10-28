#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char city[25];
    char date[25];
    float temperature;
    } Record;

void addRecord(const char *filename) {
    FILE *f = fopen(filename, "ab");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    Record r;
    printf("Nom de la ville: ");
    scanf("%s", r.city);
    printf("Date (YYYY-MM-DD): ");
    scanf("%s", r.date);
    printf("Température: ");
    scanf("%f", &r.temperature);
    fwrite(&r, sizeof(Record), 1, f);
    fclose(f);
    printf("Enregistrement ajoute avec succes.\n");
}

void modifyTemperature(const char *filename) {
    FILE *f = fopen(filename, "r+b");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return;
    }
    char city[25], date[25];
    printf("Ville à modifier: ");
    scanf("%s", city);
    printf("Date (YYYY-MM-DD): ");
    scanf("%s", date);
    Record r;
    int found = 0;
    while (fread(&r, sizeof(Record), 1, f)) {
        if (strcmp(r.city, city) == 0 && strcmp(r.date, date) == 0) {
            printf("Temperature actuelle: %.2f\n", r.temperature);
            printf("Nouvelle temperature: ");
            scanf("%f", &r.temperature);
            fseek(f, sizeof(Record), SEEK_CUR);
            fwrite(&r, sizeof(Record), 1, f);
            found = 1;
            printf("Temperature modifiee.\n");
            break;
        }
    }
    if (!found) printf("Enregistrement non trouve.\n");
    fclose(f);
}

void deleteCityRecords(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return;
    }
    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) {
        perror("Erreur de creation du fichier temporaire");
        fclose(f);
        return;
    }
    char city[25];
    printf("Ville a supprimer: ");
    scanf("%s", city);
    Record r;
    int deleted = 0;
    while (fread(&r, sizeof(Record), 1, f)) {
        if (strcmp(r.city, city) != 0) fwrite(&r, sizeof(Record), 1, temp);
        else deleted++;
    }
    fclose(f);
    fclose(temp);
    remove(filename);
    rename("temp.dat", filename);
    printf("%d enregistrement supprime pour la ville '%s'.\n", deleted, city);
}

void cityStatistics(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return;
    }
    char city[25];
    printf("Ville: ");
    scanf("%s", city);
    Record r;
    float min = 1000, max = -1000, sum = 0;
    int count = 0;
    while (fread(&r, sizeof(Record), 1, f)) {
        if (strcmp(r.city, city) == 0) {
            if (r.temperature < min) min = r.temperature;
            if (r.temperature > max) max = r.temperature;
            sum += r.temperature;
            count++;
        }
    }
    fclose(f);
    if (count == 0) {
        printf("Aucune donnée trouvee pour la ville '%s'.\n", city);
        return;
    }
    printf("Statistiques pour %s\n", city);
    printf("Température minimale : %.2f\n", min);
    printf("Température maximale : %.2f\n", max);
    printf("Température moyenne  : %.2f\n", sum / count);
}

int main() {
    const char *filename = "temperatures.dat";
    int choice;
    do {
        printf("\n1. Ajouter un enregistrement\n");
        printf("2. Modifier une temperature\n");
        printf("3. Supprimer les enregistrements d'une ville\n");
        printf("4. Afficher les statistiques d'une ville\n");
        printf("0. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: addRecord(filename); break;
            case 2: modifyTemperature(filename); break;
            case 3: deleteCityRecords(filename); break;
            case 4: cityStatistics(filename); break;
            case 0: break;
            default: printf("Choix invalide.\n");
        }
    } while (choice != 0);
    return 0;
}