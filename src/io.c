// XXX STACK FIX DONE
// io.c
//

#include "babel.h"
#include "io.h"
#include "stack.h"
#include "count.h"
#include "except.h"
#include "list.h"
#include "bvm_opcodes.h"
#include "io.h"
#include "array.h"
#include "utf8.h"

//cprintf
//
void cprintf(void){

    printf((char*)(TOS_0), (mword)car(TOS_1));

    zap();
    zap();

}

//cprints
//
void cprints(void){

    printf("%s", ((char*)TOS_0));
    zap();

}

//_slurp
//
mword *_slurp(char *filename){ // FIXME: Lots of bad things in here...

    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    long file_mword_size;

    pFile = fopen ( filename , "rb" );
    if (pFile==NULL) {//fputs ("File error",stderr); exit (1);}
        printf("%s\n", filename);
        except("_slurp: file error", __FILE__, __LINE__);
    }

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    mword last_mword;

//    d(lSize)

    //FIXME: 64-bit!!!!!!!
    //FIXME: Similar pattern scattered through multiple fn's
    if(lSize % 4 == 0){
        file_mword_size = lSize;
        last_mword = 0xffffffff;
    }
    else if(lSize % 4 == 1){
        file_mword_size = lSize+3;
        last_mword = 0xffffff00;
    }
    else if(lSize % 4 == 2){
        file_mword_size = lSize+2;
        last_mword = 0xffff0000;
    }
    else if(lSize % 4 == 3){
        file_mword_size = lSize+1;
        last_mword = 0xff000000;
    }

    // allocate memory to contain the whole file
    buffer = (char*) malloc ( (sizeof(char)*file_mword_size) + 2*MWORD_SIZE );

    if (buffer == NULL) {
        except("_slurp: malloc returned NULL", __FILE__, __LINE__);
    }

    if ((mword)buffer & (MWORD_SIZE-1)) {
        except("_slurp: expected memory to be MWORD_SIZE aligned", __FILE__, __LINE__);
    }

    // copy the file into the buffer:
    result = fread ( buffer+MWORD_SIZE, 1, lSize, pFile );

    (mword)buffer[0] = (mword)file_mword_size + MWORD_SIZE;
//    printf("%08x\n", *(mword*)buffer);

    mword *last_mword_addr;
    (char*)last_mword_addr = (buffer+file_mword_size+MWORD_SIZE);
    *last_mword_addr = last_mword;

    if (result != lSize) { fputs ("Reading error",stderr); exit (3); }

    /* the whole file is now loaded in the memory buffer. */

    fclose (pFile);
//    return (mword *)(buffer+2*MWORD_SIZE);
    return (mword *)(buffer+MWORD_SIZE);

}

//slurp
//
void slurp(void){

//    mword *temp_cons = new_cons();
    mword *filename = _b2c((mword*)TOS_0);
    mword *result    = _slurp((char*)filename);

//    cons(temp_cons, result, nil);

    zap();
    push_alloc(result, SLURP8);

}

void spit(void){

    mword *filename = _b2c((mword*)TOS_0);
    _spit((char*)filename, (mword*)TOS_1);
    zap();
    zap();

}

//
//
void _spit(char *filename, mword *fileout){

    FILE * pFile;

    mword filesize   = _arlen8(fileout);

    pFile = fopen(filename , "wb");

    if (pFile==NULL) {//fputs ("File error",stderr); exit (1);}
        except("slurp: file error", __FILE__, __LINE__);
    }

    fwrite(fileout, 1, filesize, pFile);
    //Return Value
    //The total number of elements successfully written is returned as a size_t object, which is an integral data type.
    //If this number differs from the count parameter, it indicates an error.

    fclose (pFile);

}

//
//
void journal(void){

    mword *filename = _b2c((mword*)TOS_0);
    _journal((char*)filename, (mword*)TOS_1);
    zap();
    zap();

}

//
//
void _journal(char *filename, mword *fileout){

    FILE * pFile;

    mword filesize   = _arlen8(fileout);

    pFile = fopen(filename , "ab"); //First try

    if (pFile==NULL){ //First try fails if file didn't already exist
        pFile = fopen(filename, "wb");
    }

    if (pFile==NULL) {//fputs ("File error",stderr); exit (1);}
        except("_journal: file error", __FILE__, __LINE__);
    }

    fwrite(fileout, 1, filesize, pFile);
    //Return Value
    //The total number of elements successfully written is returned as a size_t object, which is an integral data type.
    //If this number differs from the count parameter, it indicates an error.

    fclose (pFile);

}

// FIXME: Make UTF-8 compliant...
//
void stdoutop(void){

    mword length = _arlen8((mword*)TOS_0);
    int i;

    for(i = 0; i<length; i++){
        putchar((int)(*(char*)(TOS_0+i)));
    }

    zap();

}

// FIXME: Make UTF-8 compliant...
//
void stdinln(void){

    int c, i=0;
    char buffer[(1<<16)]; //64K buffer (for now)

    while(1){
        c = fgetc(stdin);
        if(c == EOF || c == '\n'){
            break;
        }
        buffer[i] = c;        
        i++;
    }

    mword arlength = (i / 4) + 1;

    if(i % 4){
        arlength++;
    }

    mword *result = _newlf(arlength);
    memcpy(result, buffer, i);
    free(buffer);

    c(result,arlength-1) = alignment_word8(i);

    push_alloc(result, STDINLN);

}

// Clayton Bauman 2011

