#include <stdio.h>
#include <string.h>


void compareFiles(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");

    if (f1 == NULL || f2 == NULL) {
        printf("Error: Could not open one or both files.\n");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return;
    }

    char line1[256], line2[256];
    int diffrent = 0, line_num = 1;
    int c1 = 0, c2 = 0, i1 = 0, i2 = 0;//we use these two variable to detect EOF by reading character by character
    while (c1 != EOF || c2 != EOF) {
        i1 = i2 = 0;

        while ((c1 = fgetc(f1)) != EOF && c1 != '\n') {
            if (i1 < 255) line1[i1++] = c1;
        }
        line1[i1] = '\0';

   
        while ((c2 = fgetc(f2)) != EOF && c2 != '\n') {
            if (i2 < 255) line2[i2++] = c2;
        }
        line2[i2] = '\0';

        if (c1 == EOF && c2 == EOF && i1 == 0 && i2 == 0)
            break;

        if (c1 == EOF && c2 != EOF) {
            printf("Extra line in %s at line %d: %s\n", file2, line_num, line2);
            diffrent++;
        } else if (c2 == EOF && c1 != EOF) {
            printf("Extra line in %s at line %d: %s\n", file1, line_num, line1);
            diffrent++;
        }
  
        else if (strcmp(line1, line2) != 0) {
            printf("Difference at line : %d\n", line_num);
            printf("file 1 : %s \n",line1);
            printf("file 2 : %s",line2);
            diffrent++;
        }

        line_num++;

      
        if (c1 == EOF && c2 == EOF)
            break;
    }

    if (diffrent == 0)
        printf("The two files are identical.\n");
    else
        printf("\nTotal differing lines: %d\n", diffrent);

    fclose(f1);
    fclose(f2);
}

int main(){
    compareFiles("file1.txt","file2.txt");
}

