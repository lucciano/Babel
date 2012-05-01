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
#include "ref.h"
#include "bstruct.h"

//main
//
int main(int argc, char **argv){

    bvm_cache root_bvm;

    interp_init(&root_bvm, argc, argv);

    bvm_interp(&root_bvm);

//    printf("%s\n",_bs2gv(root_bvm.stack_ptr));

    //If this is the root instance, the stack can be sent to STDOUT. Each
    //element on the stack will be pop'd and then sent as UTF-8 text (i.e.
    //stdout8). The user must request this behavior with a switch.

    return 0;

}

//interp_init()
//  Initializes the root Babel Virtual Machine (BVM)
//
bvm_cache *interp_init(bvm_cache *root_bvm, int argc, char **argv){

    #include "rt.pb.c"

    pearson16_init();    //Babel hash-function init

    time_t rawtime;
    char time_string[30];
    time( &rawtime );    
    strcpy( time_string, ctime(&rawtime) );
    mword *time_string_key = _c2b(time_string, 30);

    // FIXME: strcpy and strlen... get rid
    // This needs to be enhanced to look in the hidden section for a 
    // pre-defined seed, it should also save the value it used in the
    // hidden section
    mword *time_hash = new_hash();
    mword *hash_init = new_hash();
    time_hash = _pearson16(hash_init, time_string_key, (mword)strlen((char*)time_string_key));
    init_by_array(time_hash, HASH_SIZE*(sizeof(mword)/sizeof(unsigned long)));

    //initialize nil
    mword *hash_init  = new_hash();
    mword *nil_string = C2B("nil");
    nil               = _pearson16(hash_init, nil_string, (mword)strlen((char*)nil_string));
    nil               = _newref(nil);

    //initialize root_bvm
    root_bvm->hidden        = nil;
    root_bvm->sym_table     = nil;
    root_bvm->code_ptr      = _load((mword*)bbl,sizeof(bbl)/MWORD_SIZE);
    root_bvm->stack_ptr     = nil;
    root_bvm->rstack_ptr    = nil;
    root_bvm->jump_table    = nil;
    root_bvm->thread_id     = ROOT_INTERP_THREAD;
    root_bvm->steps         = (mword)-1;

    //initialize argv
    //XXX This will change when we add CLI processing:
    #define NUM_BABEL_INTERP_ARGS 1 
    if(argc <= NUM_BABEL_INTERP_ARGS){
        root_bvm->argv = nil;
    }
    else{

        root_bvm->argv = _newin(argc-NUM_BABEL_INTERP_ARGS);

        //Note: Each argument can be up to 64KB in size... this is unreasonably large
        //but could never reasonably become a design limitation in the future yet it is
        //still finite to protect against overflow attacks
        #define MAX_ARG_SIZE (1<<16)
        int i;
        for( i = NUM_BABEL_INTERP_ARGS; i < argc; i++ ){
            (mword*)c((mword*)root_bvm->argv, i-NUM_BABEL_INTERP_ARGS)
                = _c2b(argv[i], 100);
        }
    }

    //TODO
    //- Check stdin
    //    - if non-empty:
    //        read and place lines on stack
    //        unless user uses a switch to say "don't do this"
    //    - If empty, check command-line
    //        perform CLI auto-parse:
    //            TOS contains a list of all default arguments - if no default
    //            arguments, nil
    //            TOS-1 contains a hash with each CLI parameter and the 
    //            argument following it, if any. Can handle --switch,
    //            -switch=1, etc. Can't handle flags but those can be 
    //            decoded by hand.
    //        unless user uses a switch to say "don't do this"

    //TODO: Configure root BVM resource limits (hidden)

    //    time_t rawtime;
    //    char time_string[30];
    //    time( &rawtime );    
    //    strcpy( time_string, ctime(&rawtime) );
    //    mword *time_string_key = _c2b(time_string, 30);
    //
    //    // FIXME: strcpy and strlen... get rid
    //    // This needs to be enhanced to look in the hidden section for a 
    //    // pre-defined seed, it should also save the value it used in the
    //    // hidden section
    //    mword *time_hash = new_hash();
    //    mword *hash_init = new_hash();
    //    time_hash = _pearson16(hash_init, time_string_key, (mword)strlen((char*)time_string_key));
    //    init_by_array(time_hash, HASH_SIZE*(sizeof(mword)/sizeof(unsigned long)));

}

// Clayton Bauman 2011

