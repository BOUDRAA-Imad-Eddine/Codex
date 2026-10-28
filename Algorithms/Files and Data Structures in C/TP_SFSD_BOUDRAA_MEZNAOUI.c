#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>

/*STRUCTURES AND ELEMENTARY FUNCTION*/

typedef struct t_rec {
int student1_id ;
int student2_id  ;
int rating ;
char time [20] ;

}t_rec; 

// type of a data block 
typedef struct t_block_LnOF {
	   t_rec tab[101];	// array of records inside a block
	   char del[101];	// logical erase indicators ('*' erased / ' ' not erased)
	   int nb;
	   long next ;	// number of records inserted in the block
        } t_block_LnOF;



typedef struct t_header {
	   long head;       // the first block in the file (i.e. the head block of the list of blocks)
	   long freepos;    // the first free position in the last block (i.e. in the tail block of the list)
	   long lostspace;  // the total space lost due to logical deletions
	   long newblck;    // the file border (the first free block available to expand the file size)
	   long freeblck;   // head of the list of freed blocks (list of the unused blocks)
	   long nBlock; // number of blocks in the file (this is also the number of the last block)
	   long nIns;   // number of records in the file
	   long nDel;   // number of records deleted (logically) from file
} t_header_lnof;


// LnOVS file structure
typedef struct LnOF_str {
            FILE *f;    // C stream implementing the file
            t_header_lnof h; // the header in main memory
} t_LnOF;



// open a LnOVS file / mode ='N' for a New file and mode ='E' for an Existing file
// returns in *F a pointer to a newly allocated variable of type 't_LnOF'
void LnOF_open( t_LnOF **F, char *fname, char mode )
{
   t_block_LnOF buf;

   *F = malloc( sizeof( t_LnOF) );

   if ( mode == 'E' || mode == 'e' ) {
       // openning an existing file
       (*F)->f = fopen( fname, "rb+" );
       if ( (*F)->f == NULL ) {
           perror( "LnOF_open" );
           exit( EXIT_FAILURE );
       }
       // loading header part in main memory (in (*F)->h)
       fread( &((*F)->h), sizeof(t_header_lnof), 1, (*F)->f );
   }
   else {
       // creating a new LnOVS file
       (*F)->f = fopen( fname, "wb+" );
       if ( (*F)->f == NULL ) {
           perror( "LnOVS_open" );
           exit( EXIT_FAILURE );
       }
       // initializing the header part in main memory (in (*F)->h)
       (*F)->h.head = 1;      // the first allocated empty block
       (*F)->h.freepos = 0;   // first free position in the last block
       (*F)->h.lostspace = 0; // initially there are 0 bytes occupied by the logically deleted records
       (*F)->h.newblck = 2;   // the next allocated new block (in case the list of freed blocks is empty)
       (*F)->h.freeblck = -1; // initially the list of freed blocks is empty
       // writing the headers at offset 0 of stream (*F)->f
       fwrite( &((*F)->h), sizeof(t_header_lnof), 1, (*F)->f );
       // writing the first allocated block
       buf.next = -1;
       fwrite( &buf, sizeof(t_block_LnOF), 1, (*F)->f );
   }
}  // LnOVS_open


// close a LnOVS file :
// the header is first saved at the beginning of the file (offset 0) and the t_LnOF variable is freed
void LnOF_close( t_LnOF *F )
{
   // saving header part in secondary memory (at the begining of the stream F->f)
   fseek(F->f, 0L, SEEK_SET);
   fwrite( &F->h, sizeof(t_header_lnof), 1, F->f );
   fclose(F->f);
   free( F );
}


// reading data block number i into variable buf
void LnOF_readBlock( t_LnOF *F, long i, t_block_LnOF *buf )
{
   fseek(F->f, sizeof(t_header_lnof) + (i-1)*sizeof(t_block_LnOF), SEEK_SET);
   fread( buf, sizeof(t_block_LnOF), 1, F->f );
}


// writing the contents of the variable buf in data block number i
void LnOF_writeBlock( t_LnOF *F, long i, t_block_LnOF *buf )
{
   fseek(F->f, sizeof(t_header_lnof) + (i-1)*sizeof(t_block_LnOF), SEEK_SET);
   fwrite( buf, sizeof(t_block_LnOF), 1, F->f );
}


// header updates in main memory
void lnofsetHeader( t_LnOF *F , char *hname , long val )
{
   if ( strcmp( hname , "head" ) == 0 ) { F->h.head = val; return; }
   if ( strcmp( hname , "freepos" ) == 0 ) { F->h.freepos = val; return; }
   if ( strcmp( hname , "lostspace" ) == 0 ) { F->h.lostspace = val; return; }
   if ( strcmp( hname , "newblck" ) == 0 ) { F->h.newblck = val; return; }
   if ( strcmp( hname , "freeblck" ) == 0 ) { F->h.freeblck = val; return; }
   fprintf(stderr, "setHeader : Unknown headerName: \"%s\"\n", hname);
}


// header values (from main memory)
long lnofgetHeader( t_LnOF *F , char * hname )
{
   if ( strcmp( hname , "head" ) == 0 ) return F->h.head;
   if ( strcmp( hname , "freepos" ) == 0 ) return F->h.freepos;
   if ( strcmp( hname , "lostspace" ) == 0 ) return F->h.lostspace;
   if ( strcmp( hname , "newblck" ) == 0 ) return F->h.newblck;
   if ( strcmp( hname , "freeblck" ) == 0 ) return F->h.freeblck;
   fprintf(stderr, "getHeader : Unknown headerName: \"%s\"\n", hname);
}


// allocate a new block to the file
long LnOF_allocBlock( t_LnOF *F )
{
   long i;
   t_block_LnOF buf;

   if ( F->h.freeblck == -1 ) {
      // if there is no freed block, extend the file size by one additional block
      i = F->h.newblck;
      F->h.newblck++;
   }
   else {
      // else, reuse the first block from the list of freed blocks...
      i = F->h.freeblck;
      // and update the head of the list of freed blocks to point to the next block
      // = deletion at the beginning of the list
      LnOF_readBlock( F, i , &buf );
      F->h.freeblck = buf.next;
   }

   return i;
}


// mark block i as unused
void LnOF_freeBlock( t_LnOF *F, long i )
{
   t_block_LnOF buf;

   // add i to the list of freed blocks (insertion at the beginning of the ist) ...
   buf.next = F->h.freeblck;
   LnOF_writeBlock( F , i , &buf );
   F->h.freeblck = i;          // the new head of the free-blocks list is now i
}
// type of a data block (and therefore of buffer variables as well)
typedef struct blck {
	   t_rec tab[101];	// array of records inside a block
	   char del[101];	// logical erase indicators ('*' erased / ' ' not erased)
	   int nb;
	   long overflow ;
        } t_block;


// the file header (characteristics)
typedef struct hdr {
	   long nBlock; // number of blocks in the file (this is also the number of the last block)
	   long nIns;   // number of records in the file
	   long nDel;   // number of records deleted (logically) from file
	} t_header;


// TOF file structure
typedef struct TOFstr {
            FILE *f;    // C stream implementing the file
            t_header h; // the header in main memory
        } t_TOF;





void TOF_open( t_TOF **F, char *fname, char mode )
{
   *F = malloc( sizeof( t_TOF) );
   if ( mode == 'E' || mode == 'e' ) { 
        // openning an existing TOF file ...
	(*F)->f = fopen( fname, "rb+" );
	if ( (*F)->f == NULL ) { 
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // loading header part in main memory (in (*F)->h)
	fread( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
   else { // mode == 'N' || mode == 'n'
        // creating a new TOF file ... 
	(*F)->f = fopen( fname, "wb+" );
	if ( (*F)->f == NULL ) {
           perror( "TOF_open" );
           exit( EXIT_FAILURE );
        }
        // initializing the header part in main memory (in (*)->h)
	(*F)->h.nBlock = 0;
	(*F)->h.nIns = 0;
	(*F)->h.nDel = 0;
	fwrite( &((*F)->h), sizeof(t_header), 1, (*F)->f );
   }
}


// close a TOF file :
// the header is first saved at the beginning of the file and the t_TOF variable is freed
void TOF_close( t_TOF *F )
{
   // saving header part in secondary memory (at the begining of the stream F->f)
   fseek(F->f, 0L, SEEK_SET);
   fwrite( &F->h, sizeof(t_header), 1, F->f );
   fclose(F->f);
   free( F );
}


// reading data block number i into variable buf
void TOF_readBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fread( buf, sizeof(t_block), 1, F->f );
}


// writing the contents of the variable buf in data block number i
void TOF_writeBlock( t_TOF *F, long i, t_block *buf )
{
   fseek(F->f, sizeof(t_header) + (i-1)*sizeof(t_block), SEEK_SET);
   fwrite( buf, sizeof(t_block), 1, F->f );
}


// header modification
void setHeader( t_TOF *F , char *hname , long val )
{
   if ( strcmp( hname , "nBlock" ) == 0 ) { F->h.nBlock = val; return; }
   if ( strcmp( hname , "nIns" ) == 0 ) { F->h.nIns = val; return; }
   if ( strcmp( hname , "nDel" ) == 0 ) { F->h.nDel = val; return; }
   fprintf(stderr, "setHeader : Unknown headerName: \"%s\"\n", hname);
}


// header value
long getHeader( t_TOF *F , char * hname )
{
   if ( strcmp( hname , "nBlock" ) == 0 ) return F->h.nBlock;
   if ( strcmp( hname , "nIns" ) == 0 ) return F->h.nIns;
   if ( strcmp( hname , "nDel" ) == 0 ) return F->h.nDel;
   fprintf(stderr, "getHeader : Unknown headerName: \"%s\"\n", hname);
}















/* =======================================================
   =============== PART 1 : BUILD TOF ====================
   ======================================================= */

typedef t_rec recarr[1531];

t_rec read_record(FILE *F) {
    t_rec record;
    int id1, id2, rate;
    char time_var[20];


    fscanf(F, "%d %d %d %s", &id1, &id2, &rate, time_var);

    record.student1_id = id1;
    record.student2_id = id2;
    record.rating = rate;
    strcpy(record.time, time_var);

    return record;
}

void fillarray(recarr arr) {
    FILE *file = fopen("part1.txt", "r");

    if (!file) {
        printf("Could not open part1.txt\n");
        return;
    }

    for (int i = 0; i < 1531; i++) {
        arr[i] = read_record(file);
    }

    fclose(file);
}
void displayArray(recarr arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("rec %d : %d %d %d %s\n",i,
               arr[i].student1_id,
               arr[i].student2_id,
               arr[i].rating,
               arr[i].time);

    }
}

void display_TOF(t_TOF *F) {
    if (F == NULL) return;
    t_block B;
    long blockCount = getHeader(F, "nBlock");

    printf("====== File Content ======\n");
    for (long l = 1; l <= blockCount; l++) {
        TOF_readBlock(F, l, &B);
        printf("\n--- Block %ld | nb=%d ---\n", l, B.nb);

        // Fixed: i <= B.nb to show all records
        for (int i = 1; i <= B.nb; i++) {
            printf("Rec %d: %d %d %d %s\n",
                    i, B.tab[i].student1_id, B.tab[i].student2_id,
                    B.tab[i].rating, B.tab[i].time);
        }
    }
}
void split() {
    char fname[50];

    printf("give the database filename : ");
    scanf("%s",fname); 
    FILE *file = fopen(fname, "r");
    FILE *file2 = fopen("part1.txt", "w");
    FILE *file3 = fopen("part2.txt", "w");
    int i = 0;
    char record[200];

    if (!file || !file2) {
        printf("Error opening files.\n");
        return;
    }

    while (fgets(record, sizeof(record), file) && i < 1531) {
        fputs(record, file2);
        i++;
    }
    while (fgets(record, sizeof(record), file) && i < 3062) {
        fputs(record, file3);
        i++;
    }

    fclose(file);
    fclose(file2);
}


int CmpRec(t_rec rec1, t_rec rec2) {
    if (rec1.student1_id < rec2.student1_id) return true;
    if (rec1.student1_id > rec2.student1_id) return false;

    if (rec1.student2_id < rec2.student2_id) return true;
    if (rec1.student2_id > rec2.student2_id) return false;

    if (strcmp(rec1.time, rec2.time) < 0) return true;
    if (strcmp(rec1.time, rec2.time) > 0) return false;

    return 100;
}

void order(recarr arr) {
    t_rec temp;
    for (int i = 0; i < 1530; i++) {
        for (int j = 0; j < 1530 - i; j++) {
            if (!CmpRec(arr[j], arr[j + 1])) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}


void bulkload(recarr arr, t_TOF *f) {
    int n = 1531;
    int j = 1;
    long i = 1;
    t_block buf;
    int max_fill = 75; // 75% of 100

    printf("Bulk Loading...\n");
    for (int k = 0; k < n; k++) {
        if (j <= max_fill) {
            buf.tab[j] = arr[k];
            buf.del[j] = ' ';
            j++;
        } else {
            buf.nb = j - 1;
            buf.overflow = -1;
            TOF_writeBlock(f, i, &buf);
            i++;
            j = 1;
            k--; // Re-process this record in the next block
        }
    }

    // Write the last remaining block
    if (j > 1) {
        buf.nb = j - 1;
        buf.overflow = -1;
        TOF_writeBlock(f, i, &buf);
        setHeader(f, "nBlock", i);
    } else {
        setHeader(f, "nBlock", i - 1);
    }
    setHeader(f, "nIns", n);
}


/* =======================================================
   =============== PART 2 : SEARCH & INSERT ===============
   ======================================================= */





long binsearch(t_rec val, int *found, long *i, int *j) {
    long low = 1, up;
    int stop = 0;
    t_TOF *F;
    t_block buf;

    TOF_open(&F, "ordered.TOF", 'E');
    up = getHeader(F, "nBlock");

    *found = 0;

    while (low <= up && !*found && !stop) {
        *i = (low + up) / 2;
        TOF_readBlock(F, *i, &buf);

        if (CmpRec(val, buf.tab[1])) up = *i - 1;
        else if (CmpRec(buf.tab[buf.nb], val)) low = *i + 1;
        else stop = 1;
    }

    if (stop) {
        int inf = 1, sup = buf.nb;
        while (inf <= sup && !*found) {
            *j = (inf + sup) / 2;
            int c = CmpRec(val, buf.tab[*j]);
            if (c == 100) *found = 1;
            else if (c) sup = *j - 1;
            else inf = *j + 1;
        }
        if (!*found) *j = inf;
    } else {
        *i = low;
        *j = 1;
    }

    TOF_close(F);
    return 0;
}


void insert_rec(t_rec record) {
    int j, found;
    long i;
    t_LnOF *L = NULL;
    t_block_LnOF buflnof;
    t_TOF *F = NULL;
    long current_ovf;

    // 1. Find the position
    binsearch(record, &found, &i, &j);

    if (found) {
        printf("Record already exists. Insertion aborted.\n");
        return;
    }

    TOF_open(&F, "ordered.TOF", 'E');
    if (F == NULL) return;

    t_block buf;
    TOF_readBlock(F, i, &buf);


    if (buf.nb < 100) {
        for (int k = buf.nb; k >= j; k--) {
            buf.tab[k + 1] = buf.tab[k];
            buf.del[k + 1] = buf.del[k];
        }
        buf.tab[j] = record;
        buf.del[j] = ' ';
        buf.nb++;

        TOF_writeBlock(F, i, &buf);
        setHeader(F, "nIns", getHeader(F, "nIns") + 1);
        printf("Record inserted successfully.\n");
    }

    else {
        t_rec recordshifted = buf.tab[100];

        for (int k = 99; k >= j; k--) {
            buf.tab[k + 1] = buf.tab[k];
            buf.del[k + 1] = buf.del[k];
        }
        buf.tab[j] = record;
        buf.del[j] = ' ';




        if (buf.overflow == -1) {
            LnOF_open(&L, "overflow.lnof", 'N');
            buf.overflow = LnOF_allocBlock(L);
            LnOF_readBlock(L, current_ovf, &buflnof);

            buflnof.tab[1] = recordshifted;
            buflnof.del[1] = ' ';
            buflnof.nb = 1;
            buflnof.next = -1;
            LnOF_writeBlock(L, buf.overflow, &buflnof);
        }
        else {
             LnOF_open(&L, "overflow.lnof", 'E');
            long current_ovf = buf.overflow;
            long prev_ovf = -1;

            while (current_ovf != -1) {
                LnOF_readBlock(L, current_ovf, &buflnof);
                if (buflnof.nb < 100) {
                    buflnof.nb++;
                    buflnof.tab[buflnof.nb] = recordshifted;
                    buflnof.del[buflnof.nb] = ' ';
                    LnOF_writeBlock(L, current_ovf, &buflnof);
                    break;
                }
                prev_ovf = current_ovf;
                current_ovf = buflnof.next;
            }

            if (current_ovf == -1) {
                long new_block = LnOF_allocBlock(L);

                buflnof.next = new_block;
                LnOF_writeBlock(L, prev_ovf, &buflnof);
                LnOF_readBlock(L, new_block, &buflnof);

                buflnof.tab[1] = recordshifted;
                buflnof.del[1] = ' ';
                buflnof.nb = 1;
                buflnof.next = -1;
                LnOF_writeBlock(L, new_block, &buflnof);
            }
        }

        TOF_writeBlock(F, i, &buf);
        LnOF_close(L);
        printf("Record inserted (Overflow used).\n");
    }

    TOF_close(F);
}


int CmpRec2(t_rec rec1, t_rec rec2) {
    if (rec1.student1_id < rec2.student1_id) return true;
    if (rec1.student1_id > rec2.student1_id) return false;

    if (rec1.student2_id < rec2.student2_id) return true;
    if (rec1.student2_id > rec2.student2_id) return false;
    return 100;
}

long binsearch2(t_rec val, int *found, long *i, int *j) {
    long low = 1, up;
    int stop = 0;
    t_TOF *F;
    t_block buf;

    TOF_open(&F, "ordered.TOF", 'E');
    up = getHeader(F, "nBlock");

    *found = 0;

    while (low <= up && !*found && !stop) {
        *i = (low + up) / 2;
        TOF_readBlock(F, *i, &buf);

        if (CmpRec2(val, buf.tab[1]) == true)
            up = *i - 1;
        else if (CmpRec2(val, buf.tab[buf.nb]) == false)
            low = *i + 1;
        else
            stop = 1;
    }

    if (stop) {
        int inf = 1, sup = buf.nb;
        while (inf <= sup && !*found) {
            *j = (inf + sup) / 2;
            int c = CmpRec2(val, buf.tab[*j]);

            if (c == 100) *found = 1;
            else if (c) sup = *j - 1;
            else inf = *j + 1;
        }
        if (!*found) *j = inf;
    } else {
        *i = low;
        *j = 1;
    }

    TOF_close(F);
    return 0;
}





void search(int id1, int id2) {
    t_rec key;
    key.student1_id = id1;
    key.student2_id = id2;

    int found, j;
    long i;
    int count = 1;

    t_TOF *F = NULL;
    t_LnOF *L = NULL;
    t_block buf;
    t_block_LnOF buflnof;


    binsearch2(key, &found, &i, &j);

if (!found) {
    printf("No record found (%d,%d)\n", id1, id2);
    return;
}

TOF_open(&F, "ordered.TOF", 'E');


TOF_readBlock(F, i, &buf);
while (j > 1 && CmpRec2(key, buf.tab[j-1]) == 100) {
    j--;
};




while (i <= getHeader(F, "nBlock")) {

    TOF_readBlock(F, i, &buf);


    while (j <= buf.nb && CmpRec2(key, buf.tab[j]) == 100) {
        printf("Rating %d : %d %d %d %s\n",
               count++,
               buf.tab[j].student1_id,
               buf.tab[j].student2_id,
               buf.tab[j].rating,
               buf.tab[j].time);
        j++;
    }


    if (j <= buf.nb && CmpRec2(key, buf.tab[j]) != 100)
        break;


    if (j > buf.nb && buf.overflow != -1) {
        LnOF_open(&L, "overflow.lnof", 'E');
        long ovf = buf.overflow;

        while (ovf != -1) {
            LnOF_readBlock(L, ovf, &buflnof);

            for (int k = 1; k <= buflnof.nb; k++) {
                if (CmpRec2(key, buflnof.tab[k]) == 100) {
                    printf("Rating %d : %d %d %d %s\n",
                           count++,
                           buflnof.tab[k].student1_id,
                           buflnof.tab[k].student2_id,
                           buflnof.tab[k].rating,
                           buflnof.tab[k].time);
                }
            }
            ovf = buflnof.next;
        }
        LnOF_close(L);
    }

    i++;
    j = 1;
}

TOF_close(F);
}



void insert_overflow_only(t_rec rec)
{
    t_TOF *F = NULL;
    t_LnOF *L = NULL;
    t_block buf;
    t_block_LnOF buflnof;

    int found, j;
    long i;

    binsearch2(rec, &found, &i, &j);

    TOF_open(&F, "ordered.TOF", 'E');
    TOF_readBlock(F, i, &buf);


    LnOF_open(&L, "overflow.lnof", 'E');

    if (buf.overflow == -1) {

        t_block_LnOF newblk;
        newblk.nb = 1;
        newblk.next = -1;
        newblk.tab[1] = rec ;
        newblk.del[1] = ' ';

        long adr = lnofgetHeader(L, "nBlock") + 1;
        LnOF_writeBlock(L, adr, &newblk);

        buf.overflow = adr;
        TOF_writeBlock(F, i, &buf);
    }
    else {
        long ovf = buf.overflow;

        while (1) {
            LnOF_readBlock(L, ovf, &buflnof);

            if (buflnof.nb < 100) {
                buflnof.nb++;
                buflnof.tab[buflnof.nb] = rec;
                buflnof.del[buflnof.nb] = ' ';
                LnOF_writeBlock(L, ovf, &buflnof);
                break;
            }

            if (buflnof.next == -1) {
                t_block_LnOF newblk;
                newblk.nb = 1;
                newblk.next = -1;
                newblk.tab[1] = rec;
                newblk.del[1] = ' ';

                long adr = lnofgetHeader(L, "nBlock") + 1;
                LnOF_writeBlock(L, adr, &newblk);

                buflnof.next = adr;
                LnOF_writeBlock(L, ovf, &buflnof);
                break;
            }

            ovf = buflnof.next;
        }
    }

    LnOF_close(L);
    TOF_close(F);
}




void update_rating(int id1, int id2, int new_rating, char *time)
{
    t_rec key, newrec;
    int found, j;
    long i;

    t_TOF *F = NULL;
    t_LnOF *L = NULL;
    t_block buf;
    t_block_LnOF buflnof;

    key.student1_id = id1;
    key.student2_id = id2;



    binsearch2(key, &found, &i, &j);
    if (!found) {
        printf("No previous rating found then we insert new one.\n");
    }

    TOF_open(&F, "ordered.TOF", 'E');

    while (i <= getHeader(F, "nBlock")) {
        TOF_readBlock(F, i, &buf);

        int modified = 0;

        for (int k = 1; k <= buf.nb; k++) {
            if (buf.del[k]==' ' && CmpRec2(key, buf.tab[k]) == 100) {
                buf.del[k] = '*';
                modified = 1;
            }
        }

        if (modified)
            TOF_writeBlock(F, i, &buf);

        if (buf.nb < 1 || CmpRec2(key, buf.tab[buf.nb]) != 100)
            break;

        i++;
    }

    TOF_close(F);



    TOF_open(&F, "ordered.TOF", 'E');

    for (long b = 1; b <= getHeader(F, "nBlock"); b++) {
        TOF_readBlock(F, b, &buf);

        if (buf.overflow == -1) {
            LnOF_open(&L, "overflow.lnof", 'N');
            buf.overflow = LnOF_allocBlock(L);
        };

        LnOF_open(&L, "overflow.lnof", 'E');
        long ovf = buf.overflow;

        while (ovf != -1) {
            LnOF_readBlock(L, ovf, &buflnof);

            int modified = 0;

            for (int k = 1; k <= buflnof.nb; k++) {
                if (buflnof.del[k]==' ' &&
                    CmpRec2(key, buflnof.tab[k]) == 100) {
                    buflnof.del[k] = '*';
                    modified = 1;
                }
            }

            if (modified)
                LnOF_writeBlock(L, ovf, &buflnof);

            ovf = buflnof.next;
        }

        LnOF_close(L);
    }

    TOF_close(F);


    newrec.student1_id = id1;
    newrec.student2_id = id2;
    newrec.rating      = new_rating;
    strcpy(newrec.time, time);

    insert_rec(newrec);

    printf("Rating updated successfully.\n");
}

void List_Friends(int id1) {
    t_TOF *F = NULL;
    t_LnOF *L = NULL;
    t_block buf;
    t_block_LnOF buflnof;
    int found = 0;

    TOF_open(&F, "ordered.TOF", 'E');
    
    // Attempt to open LnOF if it exists
    FILE *f_check = fopen("overflow.lnof", "rb");
    if (f_check != NULL) {
        fclose(f_check);
        LnOF_open(&L, "overflow.lnof", 'E');
    }

    // Array to prevent printing the same overflow block multiple times
    bool visitedLnOF[10000] = {false};

    printf("\nFriends of Student %d:\n", id1);
    printf("--------------------------------------------------\n");

    for (long b = 1; b <= getHeader(F, "nBlock"); b++) {
        TOF_readBlock(F, b, &buf);

        // 1. Check main TOF block
        for (int k = 1; k <= buf.nb; k++) {
            if (buf.del[k] == ' ' && buf.tab[k].student1_id == id1 && buf.tab[k].rating > 0) {
                found = 1;
                printf("[TOF Block %ld] Student2: %-4d | Rating: %-2d | Time: %s\n",
                       b, buf.tab[k].student2_id, buf.tab[k].rating, buf.tab[k].time);
            }
        }

        // 2. Check the LnOF chain for this block
        if (L != NULL && buf.overflow != -1) {
            long ovf = buf.overflow;
            while (ovf != -1) {
                if (!visitedLnOF[ovf]) {
                    LnOF_readBlock(L, ovf, &buflnof);
                    for (int k = 1; k <= buflnof.nb; k++) {
                        if (buflnof.del[k] == ' ' && buflnof.tab[k].student1_id == id1 && buflnof.tab[k].rating > 0) {
                            found = 1;
                            printf("[LnOF Block %ld] Student2: %-4d | Rating: %-2d | Time: %s\n",
                                   ovf, buflnof.tab[k].student2_id, buflnof.tab[k].rating, buflnof.tab[k].time);
                        }
                    }
                    visitedLnOF[ovf] = true;
                }
                ovf = buflnof.next;
            }
        }
    }

    if (!found) printf("No friends found for Student %d.\n", id1);
    if (L != NULL) LnOF_close(L);
    TOF_close(F);
}
void Display_Block_Content(int block_num) {
    t_TOF *F = NULL;
    t_block buf;
    TOF_open(&F, "ordered.TOF", 'E');

    if (block_num < 1 || block_num > getHeader(F, "nBlock")) {
        printf("Error: Block %d does not exist.\n", block_num);
        TOF_close(F);
        return;
    }

    // Calculate physical address: Header size + (block_index * block_size)
    long address = sizeof(t_header) + (block_num - 1) * sizeof(t_block);

    TOF_readBlock(F, block_num, &buf);

    printf("\n==================================================");
    printf("\n CONTENT OF BLOCK: %d", block_num);
    printf("\n DISK ADDRESS:     0x%lX (%ld bytes)", address, address);
    printf("\n NUMBER OF RECS:   %d", buf.nb);
    printf("\n OVERFLOW POINTER: %ld", buf.overflow);
    printf("\n==================================================\n");

    for (int k = 1; k <= buf.nb; k++) {
        printf("[%s] ID1: %-4d | ID2: %-4d | Rate: %-2d | Time: %s\n",
               (buf.del[k] == ' ' ? "VALID" : "DELET"),
               buf.tab[k].student1_id, buf.tab[k].student2_id,
               buf.tab[k].rating, buf.tab[k].time);
    }
    printf("==================================================\n");

    TOF_close(F);
}


//=========================================================================
//================ PART 3 : REORGANIZATION ===================================
//=========================================================================

typedef t_rec rec_array[5000];

// Remplit le tableau avec les enregistrements du TOF
int fillArrayFromTOF_LnOF(t_TOF *F, t_LnOF *L, rec_array arr) {
    t_block buf;
    t_block_LnOF buflnof;
    int count = 0;

    long nBlock = getHeader(F, "nBlock");

    // Tableau pour marquer les blocks LnOF déjà lus
    bool visitedLnOF[10000] = {false};

    // Parcourir tous les blocks TOF
    for (long i = 1; i <= nBlock; i++) {
        TOF_readBlock(F, i, &buf);

        // Ajouter les records valides du TOF
        for (int k = 1; k <= buf.nb; k++) {
            if (buf.del[k] == ' ') {
                arr[count++] = buf.tab[k];
                printf("Added TOF record: %d %d %d %s\n",
                       buf.tab[k].student1_id,
                       buf.tab[k].student2_id,
                       buf.tab[k].rating,
                       buf.tab[k].time);
            }
        }

        // Ajouter les records de l'overflow lié, sans doublons
        long ovf = buf.overflow;
        while (ovf != -1) {
            if (!visitedLnOF[ovf]) {
                LnOF_readBlock(L, ovf, &buflnof);

                for (int k = 1; k <= buflnof.nb; k++) {
                    if (buflnof.del[k] == ' ') {
                        arr[count++] = buflnof.tab[k];
                        printf("Added LnOF record: %d %d %d %s\n",
                               buflnof.tab[k].student1_id,
                               buflnof.tab[k].student2_id,
                               buflnof.tab[k].rating,
                               buflnof.tab[k].time);
                    }
                }

                visitedLnOF[ovf] = true;
            }

            ovf = buflnof.next;
        }
    }

    return count; // nombre total de records remplis dans arr
}




// Fonction de fusion TOF + LnOF
void Merge_TOF_LnOF() {
    t_TOF *F = NULL;
    t_LnOF *L = NULL;
    rec_array arr;
    int totalRecords;


    TOF_open(&F, "ordered.TOF", 'E');
    LnOF_open(&L, "overflow.lnof", 'E');

    totalRecords = fillArrayFromTOF_LnOF(F, L, arr);



    TOF_close(F);
    LnOF_close(L);


    order(arr);


    t_TOF *F_reorg = NULL;
    TOF_open(&F_reorg, "Reorganized.TOF", 'N');

    bulkload(arr, F_reorg);
    TOF_close(F_reorg);
    TOF_open(&F_reorg, "Reorganized.TOF", 'E');
    display_TOF(F_reorg);

    TOF_close(F_reorg);




    printf("Merge TOF + LnOF completed. Total records: %d\n", totalRecords);
}

void Reset_LnOF() {
    t_LnOF *L = NULL;

    LnOF_open(&L, "overflow.lnof", 'N');

    LnOF_close(L);
}

void display_TOF_LnOF(t_TOF *F, t_LnOF *L) {
    t_block buf;
    t_block_LnOF buflnof;
    long nBlock = getHeader(F, "nBlock");

   
    bool visitedLnOF[5000] = {false};

    for (long i = 1; i <= nBlock; i++) {
        TOF_readBlock(F, i, &buf);

        printf("\n--- TOF Block %ld | nb=%d | overflow=%ld ---\n", i, buf.nb, buf.overflow);

    
        for (int k = 1; k <= buf.nb; k++) {
            if (buf.del[k] == ' ') {
                printf("TOF Record: %d %d %d %s\n",
                       buf.tab[k].student1_id,
                       buf.tab[k].student2_id,
                       buf.tab[k].rating,
                       buf.tab[k].time);
            }
        }

     
        long ovf = buf.overflow;
        while (ovf != -1) {
            if (!visitedLnOF[ovf]) {
                LnOF_readBlock(L, ovf, &buflnof);

                printf("  LnOF Block %ld | nb=%d | next=%ld\n", ovf, buflnof.nb, buflnof.next);

                for (int k = 1; k <= buflnof.nb; k++) {
                    if (buflnof.del[k] == ' ') {
                        printf("  LnOF Record: %d %d %d %s\n",
                               buflnof.tab[k].student1_id,
                               buflnof.tab[k].student2_id,
                               buflnof.tab[k].rating,
                               buflnof.tab[k].time);
                    }
                }

                visitedLnOF[ovf] = true; 
            }

            ovf = buflnof.next;  
        }
    }
}






//=========================================================================================================
//=========================================================================================================
//=========================================================================================================
//=============================================== main =============================================




#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>



#define FG_WHITE   15
#define FG_YELLOW  14

#define BG_BLUE    (BACKGROUND_BLUE)
#define BG_BLUE_I  (BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BG_CYAN_I  (BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY)

#define NORMAL 7

void set_color(WORD fg, WORD bg) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg | bg);
}

void clear() {
    system("cls");
}

void goto_xy(int x, int y) {
    COORD c = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void move_sound() {
    Beep(800, 40);
}

// ===================== ASCII Loading Animation =====================
void loading_animation(const char *msg) {
    clear();

    // Title
    set_color(FG_YELLOW, BG_BLUE_I);
    printf("\n\n   %s\n\n", msg);

    // Progress bar
    int total = 30;
    for (int i = 0; i <= total; i++) {
        set_color(FG_WHITE, BG_BLUE_I);
        printf("\r   [");
        for (int j = 0; j < i; j++) printf("=");
        for (int j = i; j < total; j++) printf(" ");
        printf("] %3d%%", (i * 100) / total);
        Sleep(50);
    }

    printf("\n\n   Done! Press any key to continue...");
    getch();
}

// ===================== Draw Box =====================
void draw_box(int x, int y, int w, int h) {
    for (int i = 0; i < h; i++) {
        COORD c = {x, y + i};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
        set_color(FG_WHITE, BG_BLUE_I);
        for (int j = 0; j < w; j++) printf(" ");
    }
}

// ===================== Fancy Menu =====================
int fancy_menu(const char *title, const char *items[], int n) {
    int pos = 0, ch;

    int bx = 20, by = 5, bw = 60;

    while (1) {
        clear();
        draw_box(bx, by, bw, n + 6);

        COORD t = {bx + 18, by + 1};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), t);
        set_color(FG_YELLOW, BG_BLUE_I);
        printf("%s", title);

        for (int i = 0; i < n; i++) {
            COORD c = {bx + 4, by + 3 + i};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

            if (i == pos) {
                set_color(FG_WHITE, BG_CYAN_I);
                printf(">> %-48s", items[i]);
            } else {
                set_color(FG_WHITE, BG_BLUE_I);
                printf("   %-48s", items[i]);
            }
        }

        ch = getch();
        if (ch == 224) {
            ch = getch();
            move_sound();
            if (ch == 72 && pos > 0) pos--;
            if (ch == 80 && pos < n - 1) pos++;
        } else if (ch == 13) {
            return pos;
        }
    }
}

// ===================== Main =====================
int main() {
    recarr arr;
    int built = 0;

    while (1) {
        const char *main_menu[] = {
            "PART 1 : BUILD TOF",
            "PART 2 : SEARCH & INSERT & LIST",
            "PART 3 : REORGANIZATION",
            "EXIT"
        };

        
        int c = fancy_menu("MAIN MENU", main_menu, 4);
        if (c == 3) break;

        /* ================= PART 1 ================= */
        if (c == 0) {
            const char *p1[] = {
                "Split databasis.txt",
                "Load + Sort array",
                "Bulkload ordered.TOF",
                "Display TOF",
                "Back" // Index 4
            };

            while (1) {
                
                int s = fancy_menu("PART 1 : BUILD TOF", p1, 5);
                if (s == 4) break;

                if (s == 0) {
                    loading_animation("Splitting database... \n");
                    split();
                }
                if (s == 1) {
                    loading_animation("Loading & sorting...");
                    fillarray(arr);
                    order(arr);
                }
                if (s == 2) {
                    loading_animation("Bulkloading TOF...");
                    t_TOF *F = NULL;
                    TOF_open(&F, "ordered.TOF", 'N');
                    bulkload(arr, F);
                    TOF_close(F);
                    built = 1;
                }
                if (s == 3) {
                    if (built) {
                        t_TOF *F = NULL;
                        TOF_open(&F, "ordered.TOF", 'E');
                        display_TOF(F);
                        TOF_close(F);
                    } else {
                        printf("\n[!] Please bulkload the file first.\n");
                    }
                    getch();
                }
            }
        }

        /* ================= PART 2 ================= */
        if (c == 1) {
            if (!built) {
                printf("\n[!] You must build the TOF in PART 1 first!\n");
                getch();
                continue;
            }

            const char *p2[] = {
                "Insert records from part2.txt",
                "Search (id1,id2)",
                "Display TOF",
                "Update record",
                "List all friends",
                "List Block content",
                "Back" 
            };

            while (1) {
               
                int s = fancy_menu("PART 2 : SEARCH & INSERT", p2, 7);
                if (s == 6) break;

                if (s == 0) {
                    FILE *f = fopen("part2.txt", "r");
                    if (!f) {
                        printf("Error: part2.txt not found. Run Part 1 Split first.\n");
                    } else {
                        int N;
                        printf("\nHow many records to insert: ");
                        scanf("%d", &N);
                        for (int i = 0; i < N; i++) {
                            t_rec r = read_record(f);
                            if (r.student1_id != -1) insert_rec(r);
                        }
                        fclose(f);
                        printf("\nInsertion complete.");
                    }
                    getch();
                }

                if (s == 1) {
                    int id1, id2;
                    printf("\nID1: "); scanf("%d", &id1);
                    printf("ID2: "); scanf("%d", &id2);
                    search(id1, id2);
                    getch();
                }

                if (s == 2) {
                    t_TOF *F = NULL;
                    t_LnOF *L = NULL;
                    TOF_open(&F, "ordered.TOF", 'E');
                    // Check if LnOF exists before opening
                    FILE *ftemp = fopen("overflow.lnof", "rb");
                    if (ftemp) {
                        fclose(ftemp);
                        LnOF_open(&L, "overflow.lnof", 'E');
                    }
                    display_TOF(F);
                    if (L) {
                        display_TOF_LnOF(F, L);
                        LnOF_close(L);
                    }
                    TOF_close(F);
                    getch();
                }

                if (s == 3) {
                    int id1, id2, r;
                    char t[20];
                    printf("\nID1: "); scanf("%d", &id1);
                    printf("ID2: "); scanf("%d", &id2);
                    printf("New Rating: "); scanf("%d", &r);
                    printf("New Time: "); scanf("%s", t);
                    update_rating(id1, id2, r, t);
                    getch();
                }

                if (s == 4) {
                    int id;
                    printf("\nStudent ID: ");
                    scanf("%d", &id);
                    List_Friends(id);
                    getch();
                }

                if (s == 5) {
                    int block_num;
                    printf("\nEnter Block Number to Display: ");
                    scanf("%d", &block_num);
                    Display_Block_Content(block_num);
                    getch();
                }
            }
        }

        /* ================= PART 3 ================= */
        if (c == 2) {
            const char *p3[] = {
                "Merge TOF and LnOF",
                "Reset LnOF",
                "Display Reorganized TOF",
                "Back" 
            };

            while (1) {
               
                int s = fancy_menu("PART 3 : REORGANIZATION", p3, 4);
                if (s == 3) break;

                if (s == 0) {
                    loading_animation("Merging files...");
                    Merge_TOF_LnOF();
                    getch();
                }
                if (s == 1) {
                    loading_animation("Resetting LnOF...");
                    Reset_LnOF();
                    getch();
                }
                if (s == 2) {
                    t_TOF *F1 = NULL;
                    FILE *ftemp = fopen("Reorganized.TOF", "rb");
                    if (ftemp) {
                        fclose(ftemp);
                        TOF_open(&F1, "Reorganized.TOF", 'E');
                        display_TOF(F1);
                        TOF_close(F1);
                    } else {
                        printf("\n[!] Reorganized.TOF not found. Run Merge first.\n");
                    }
                    getch();
                }
            }
        }
    }

    clear();
    set_color(14, 0);
    printf("\nProgram terminated gracefully.\n");
    set_color(7, 0);  
    return 0;
}