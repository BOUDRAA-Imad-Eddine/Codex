#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int CCPnum;
    int balance;
    }Account;


int update_balance(char f1[],char f2[]){
    FILE *bin = fopen(f1,'wb+');
    FILE *txt = fopen(f2, 'r');
    
    int account_num;
    int amount;
    Account a;
    while (fscanf(txt, "%s %d", &account_num,&amount)){
        fseek(bin,0,SEEK_SET);
        int found =0;
        while(fread(&a,sizeof a,1,bin) == 1){
            if(a.CCPnum == account_num){
                a.balance += amount;
                fseek(bin, sizeof a,SEEK_CUR);
                fwrite(&a, sizeof a,1, bin);
                found = 1;
                break;
            }
        }
        if (found = 0){
            printf("account not found");
        }
        fclose(bin);
        fclose(txt);

    }
}


int main() {
    // Step 1: Create test accounts (F1)
    FILE *f1 = fopen("F1.bin", "wb");
    Account accounts[] = {
        {1001, 2500.00},
        {1002, 4200.50},
        {1003, 3100.75},
        {1004, 150.00}
    };

    fwrite(accounts, sizeof(Account), 4, f1);
    fclose(f1);

    
    FILE *f2 = fopen("F2.txt", "w");
    fprintf(f2, "1001 500.00\n");   
    fprintf(f2, "1003 -100.75\n");  
    fprintf(f2, "1004 50.00\n");   
    fprintf(f2, "9999 100.00\n");   

    apply_updates("F1.bin", "F2.txt");

    f1 = fopen("F1.bin", "rb");
    if (!f1) return 1;
    Account a;
    printf("\nUpdated account balances:\n");
    while (fread(&a, sizeof(Account), 1, f1) == 1) {
        printf("Account %d , Balance = %.2f\n", a.CCPnum, a.balance);
    }
    fclose(f1);

    return 0;
}