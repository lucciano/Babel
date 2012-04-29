// io.h
// 

#ifndef IO_H
#define IO_H

bvm_cache *stdoutop8(bvm_cache *this_bvm);
void _stdoutop8(mword *string);

mword *_slurp(mword *filename);
bvm_cache *slurp(bvm_cache *this_bvm);
bvm_cache *slurp_mword(bvm_cache *this_bvm);

//#include "list.h"
//
//void cprintf(void);
//void cprints(void);

//void slurp(void);
//mword *_b2c(mword *string);
//void spit(void);
//void _spit(char *filename, mword *fileout);
//void journal(void);
//void _journal(char *filename, mword *fileout);
//void stdoutop(void);
//void stdoutop8(void);
//void stdinln(void);
//void spit_mword(void);
//void slurp_mword(void);
//
#endif //IO_H

// Clayton Bauman 2011

