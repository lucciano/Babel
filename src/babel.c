// babel.c
//

#include "babel.h"
#include "io.h"
#include "bvm.h"
#include "pearson16.h"
#include "debug.h"
#include <time.h>
#include "mt19937ar.h"
#include "array.h"
#include "string.h"
#include "util.h"
#include "load.h"

//main
//
int main(int argc, char **argv){

    int i;

    #include "rt.pb.c"
//    _load((mword*)bbl);
//    _bvm_init(bbl);

//    for(i=0;i<sizeof(bbl)/MWORD_SIZE;i++){
//        printf("%x ", i*MWORD_SIZE);
//        d(bbl[i]);
//    }
//
    _bvm_init(_load_at_reset((mword*)bbl,sizeof(bbl)/MWORD_SIZE));

    //Need to figure out what to do with this...
    init_global_argv(argc, argv);

//    bbl2gv();

    global_steps = (mword) -1;
    _bvmexec(internal_global_VM);

    printf("\n");
    return 0;

}

// Clayton Bauman 2011

