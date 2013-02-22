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
#include "bstruct.h"
#include "stack.h"
#include "tags.h"
#include "hash.h"
#include "tlist.h"
#include "list.h"
#include <stdarg.h>
#include "ref.h"

//main
//
int main(int argc, char **argv){

//    pearson16_init();    //Babel hash-function init
//    init_nil();
//
//    mword *loaded_bbl = _load((mword*)bbl,BBL_SIZE);
//    _dump(loaded_bbl);
//    die;

    //loaded_bbl = _deref((mword*)car(cdr(bvm_code_ptr(loaded_bbl))), (mword*)car(bvm_code_ptr(loaded_bbl)));
//

    bvm_cache root_bvm;

    interp_init(&root_bvm, argc, argv);

    //fprintf(stderr, "%d\n", _exha( (mword*)car(root_bvm.sym_table) , _hash8(C2B("foo"))));

    //mword *temp = _luha( (mword*)car(root_bvm.sym_table) , _hash8(C2B("argv")));
    //_dump(temp);

    //mword *hash_table = new_hash_table();

    //hash_insert( root_bvm.sym_table, "foo", _newva(0xbade) );
    //hash_insert( root_bvm.sym_table, "foo", _newva(0xcede) );
    //_rmha( (mword*)car(root_bvm.sym_table) , _hash8(C2B("foo")));

    //_push(root_bvm.dstack_ptr, _newva(0xbead));

    flush_bvm_cache(&root_bvm);

    _dump(root_bvm.self);

    //If this is the root instance, the stack can be sent to STDOUT. Each
    //element on the stack will be pop'd and then sent as UTF-8 text (i.e.
    //stdout8). The user must request this behavior with a switch.

    return 0;

}



// Should be called only once per bvm instance
// FIXME: Everything about this is COMPLETELY busted...
void init_nil(void){

    mword *ptr = malloc( MWORDS( 3 + HASH_SIZE + 1 ) ); // 3 = s-field + car + cdr

    mword *nil_hash   = _hash8(C2B("nil"));

    ptr[0] = 0; // X.s = 0 -> tagged-list

    //FIXME: 32-bit specific... also, this whole thing is inelegant
    ptr[1] = nil_hash[0];
    ptr[2] = nil_hash[1];
    ptr[3] = nil_hash[2];
    ptr[4] = nil_hash[3];

    nil = (ptr + 1);

    ptr[5] = -2;
    ptr[6] = (mword)nil;
    ptr[7] = (mword)nil;

}


//  Initializes the root Babel Virtual Machine (BVM)
//
bvm_cache *interp_init(bvm_cache *this_bvm, int argc, char **argv){ // interp_init#
//bvm_cache *interp_init(int argc, char **argv){

    #include "construct.sp.c"

    pearson16_init();    //Babel hash-function init

    //initialize nil (global constant)
    init_nil();
    
    //load the root bvm
    this_bvm->self = _load((mword*)bbl,BBL_SIZE);
    this_bvm->sym_table = (mword*)bvm_sym_table(this_bvm->self);

    //initialize empty_string (global constant)
    empty_string = _newlfi(1,0);

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

    //initialize argv
    //XXX This will change when we add CLI processing:
    #define NUM_BABEL_INTERP_ARGS 1 
    if(argc <= NUM_BABEL_INTERP_ARGS){
        this_bvm->argv = nil;
    }
    else{

        this_bvm->argv = _newin(argc-NUM_BABEL_INTERP_ARGS);

        //Note: Each argument can be up to 64KB in size... this is unreasonably large
        //but could never reasonably become a design limitation in the future yet it is
        //still finite to protect against overflow attacks
        #define MAX_ARG_SIZE (1<<16)
        int i;
        for( i = NUM_BABEL_INTERP_ARGS; i < argc; i++ ){
            (mword*)c((mword*)this_bvm->argv, i-NUM_BABEL_INTERP_ARGS)
                = _c2b(argv[i], 100);
        }

    }

    set_sym("steps",            _newva((mword)-1) );
    set_sym("thread_id",        _newva(0) );
    set_sym("advance_type",     _newva((mword)BVM_ADVANCE) );
    set_sym("argv",             this_bvm->argv );
    set_sym("srand",            time_hash );
    set_sym("soft_root",        nil );

    //hash_insert( this_bvm->sym_table, "argv",  this_bvm->argv );
    //hash_insert( this_bvm->sym_table, "srand", time_hash );
    //hash_insert( this_bvm->sym_table, "steps", this_bvm->steps );
    //hash_insert( this_bvm->sym_table, "soft_root", nil );

    load_bvm_cache(this_bvm);

    init_interp_jump_table(this_bvm);

    return this_bvm;

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

}

//
void init_interp_jump_table(bvm_cache *this_bvm){

    #include "fixed_opcodes.h"

//    printf("%d\n",sizeof(interp_fixed_opcodes)/sizeof(babel_op));
//    die

    #define num_opcodes (sizeof(interp_fixed_opcodes)/sizeof(babel_op))

    this_bvm->jump_table = _newlf(num_opcodes);

    int i;
    for(i=0;i<num_opcodes;i++){
        (mword*)this_bvm->jump_table[i] = (mword*)interp_fixed_opcodes[i];
    }

}

//
// babel_operator
bvm_cache *endian(bvm_cache *this_bvm){

    fatal("stack fix not done");
    mword test = 1;
    char *test_addr = (char *)&test;
    mword *result = new_atom;

    if(*test_addr){
        *result = LITTLE_ENDIAN;
    }
    else{
        *result = BIG_ENDIAN;
    }

    push_alloc(this_bvm, result, IMMORTAL);

    return this_bvm;

}


// Clayton Bauman 2011


