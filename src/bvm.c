// bvm.c
//

#include "babel.h"
#include "bvm.h"
#include "load.h"
#include "stack.h"
#include "bvm_opcodes.h"
#include "bvm_interp.h"
#include "arith.h"
#include "io.h"
#include "array.h"
#include "eval.h"
#include "list.h"
#include "debug.h"
#include "pearson16.h"
#include "except.h"
#include "cmp.h"
#include "string.h"
#include "shift.h"
#include "logic.h"
#include "util.h"
#include <time.h>
#include "mt19937ar.h"
#include "hash.h"
#include "bstruct.h"
#include "alloc.h"
#include "tptr.h"

/* bvm operator
**code**  
**stack**  
**rstack**  
**bvmstep**  
**mkbvm**  
**bvmkill**  
**halt**  
**fork**  
**system**  
> equivalent to Perl's system()  

**shell**  
> equivalent to Perl's back-tick quotes  

**exec**  
> does a fork/exec in Linux; CreateProcess in Windows
*/

////
//// babel_operator
//bvm_cache *bvm_interp(bvm_cache *this_bvm){
//
//    bvm_cache *discard;
//    babel_op op_ptr;
//
////    printf("%08x", car(this_bvm->steps));
////
////    die;
//
//    while( car(this_bvm->steps) ){//FIXME: This is not correct long-term   
//
//        if(is_nil((mword*)car(this_bvm->code_ptr))){
//            if(!is_nil(this_bvm->rstack_ptr)){
//                next(this_bvm);
//                continue;
//            }
//            break; // XXX An unexpected nil in code-stream can cause an exit... may be bad
//        }
//
//        if( is_inte(car(this_bvm->code_ptr)) ){
//            die;
////            push_alloc(this_bvm, (mword*)car(car(this_bvm->code_ptr)), IMMORTAL);
//            push_alloc(this_bvm, (mword*)car(this_bvm->code_ptr), IMMORTAL);
//        }
//        else if( is_leaf(car(this_bvm->code_ptr)) ){
//            mword opcode = car(car(this_bvm->code_ptr));
//
////            d(car(car(this_bvm->code_ptr)))
//
//            op_ptr = (babel_op)this_bvm->jump_table[ opcode % NUM_INTERP_OPCODES ];
//            discard = op_ptr(this_bvm);
//        }
////        else if( is_href(car(this_bvm->code_ptr)) ){ //TODO: Implement href operator calls!
////            error("bvm_interp: hash-reference detected in code");
////            die;
////        }
//        else{
//            fatal("error detected during execution");
//        }
//
//        if(car(this_bvm->advance_type) == BVM_ADVANCE){
//            this_bvm->code_ptr = (mword*)cdr(this_bvm->code_ptr);
//        }
//        else if(car(this_bvm->advance_type) == BVM_RETURN){
//            break;
//        }
//        else{
//            icar(this_bvm->advance_type) = BVM_ADVANCE;
//        }
//
//        icar(this_bvm->steps)--;
//
//    }
//
//    flush_bvm_cache(this_bvm);
//
//    return this_bvm;
//
//}
//


//
//
bvm_cache *bvm_interp(bvm_cache *this_bvm){ // bvm_interp#

    bvm_cache *discard;
    babel_op op_ptr;

    while( car(this_bvm->steps) ){//FIXME: This is not correct long-term   

        if(is_nil((mword*)car(this_bvm->code_ptr))){

            //popr(this_bvm);
            //_dump(this_bvm->rstack_ptr);
            //printf("%d\n", rstack_empty(this_bvm));
            //die;

            if(!rstack_empty(this_bvm)){

#define eval_return(x) _ith((mword*)icar(x),0)
#define eval_type(x)   _ith((mword*)icar(x),1)

                mword *temp = popr(this_bvm);

                //printf("%d\n",tageq((mword*)eval_type(temp),BABEL_TAG_EVAL));
                //die;

                if(tageq(eval_type(temp),BABEL_TAG_EVAL)){
                    _dump(eval_return(temp));
                    die;
                    this_bvm->code_ptr = eval_return(temp);
                }

            }

            break; // XXX An unexpected nil in code-stream can cause an exit... may be bad
        }

        if( is_inte(car(car(this_bvm->code_ptr))) ){
//            die;
//            push_alloc(this_bvm, (mword*)car(this_bvm->code_ptr), IMMORTAL);
            pushd( this_bvm, (mword*)car(car(car(this_bvm->code_ptr))), IMMORTAL );
//            flush_bvm_cache(this_bvm);
//            _dump(this_bvm->self);
//            die;
        }
        else if( is_leaf(car(car(this_bvm->code_ptr))) ){
            mword opcode = c(car(car(this_bvm->code_ptr)),0);

//            d(car(car(this_bvm->code_ptr)))

            op_ptr = (babel_op)this_bvm->jump_table[ opcode % NUM_INTERP_OPCODES ];
            discard = op_ptr(this_bvm);
        }
//        else if( is_href(car(this_bvm->code_ptr)) ){ //TODO: Implement href operator calls!
//            error("bvm_interp: hash-reference detected in code");
//            die;
//        }
        else{
            fatal("error detected during execution");
        }

        if(car(this_bvm->advance_type) == BVM_ADVANCE){
            icar(this_bvm->code_ptr) = cdr(car(this_bvm->code_ptr));
        }
        else if(car(this_bvm->advance_type) == BVM_RETURN){
            die;
            break;
        }
        else{
//            _dump(this_bvm->code_ptr);
//            die;
            icar(this_bvm->advance_type) = BVM_ADVANCE;
        }

        icar(this_bvm->steps)--;

    }

    flush_bvm_cache(this_bvm);

    return this_bvm;

}


/* bvm operator
**babel**
> This operator "execs" a loaded BVM on TOS
*/
bvm_cache *babelop(bvm_cache *this_bvm){ // babelop#

    bvm_cache new_bvm;

    new_bvm.self = TOS_0(this_bvm);
    hard_zap(this_bvm);

    load_bvm_cache(&new_bvm);

    new_bvm.jump_table     = this_bvm->jump_table;

    mword *temp = new_atom;
    *temp = car(this_bvm->thread_id) + 1;

    new_bvm.thread_id      = temp;

    new_bvm.argv           = this_bvm->argv; //FIXME: shift off 0th argv

    flush_bvm_cache(this_bvm);
    flush_bvm_cache(&new_bvm);

    bvm_interp(&new_bvm);

    flush_bvm_cache(&new_bvm);
    load_bvm_cache(this_bvm); // Technically, this is not necessary
                                // but it doesn't hurt

    //FIXME - push stack of new_bvm onto this_bvm when bvm_interp returns
    //push_alloc(this_bvm, new_bvm.stack_ptr, BVMEXEC);

    return this_bvm;

}


//
// babel_operator
bvm_cache *bvmcode(bvm_cache *this_bvm){

   fatal("stack fix not done");
   push_alloc(this_bvm, this_bvm->code_ptr, IMMORTAL); 
    // XXX: These operators are inherently dangerous in 
    // combination with the mortal operator

    return this_bvm;

}

//
// babel_operator
bvm_cache *bvmstack(bvm_cache *this_bvm){

    fatal("stack fix not done");
    push_alloc(this_bvm, this_bvm->dstack_ptr, IMMORTAL);

    return this_bvm;

}

//
// babel_operator
bvm_cache *bvmustack(bvm_cache *this_bvm){

    fatal("stack fix not done");
    push_alloc(this_bvm, this_bvm->ustack_ptr, IMMORTAL);

    return this_bvm;

}

//
// babel_operator
bvm_cache *self(bvm_cache *this_bvm){

    fatal("stack fix not done");
    flush_bvm_cache(this_bvm);

//    mword *result = _bs2gv(this_bvm->self);
//    push_alloc(this_bvm, result, IMMORTAL);

    push_alloc(this_bvm, this_bvm->self, IMMORTAL);

    return this_bvm;

}

//
// babel_operator
bvm_cache *rsvd(bvm_cache *this_bvm){

    d(car(car(this_bvm->code_ptr)))

    error("An attempt was made to execute a reserved opcode");
    die;

}

//
// babel_operator
bvm_cache *boilerplate(bvm_cache *this_bvm){

//    printf(
//"Babel %s\n\
//    Built %s %s\n\
//    Clayton Bauman (c) 2012, see LICENSE for details\n\n", 
//    BABEL_VERSION, __DATE__, __TIME__);

    fatal("stack fix not done");
    char msg[256];

    sprintf(msg, 
"Babel %s\n\
    Built %s %s\n\
    Clayton Bauman (c) 2012, see LICENSE for details\n\n", 
    BABEL_VERSION, __DATE__, __TIME__);

    push_alloc( this_bvm, C2B(msg), MORTAL );

}


//
//
bvm_cache *load_bvm_cache(bvm_cache *this_bvm){ // load_bvm_cache#

    mword *self = this_bvm->self;

    this_bvm->code_ptr      = (mword*)bvm_code_ptr(self);
    this_bvm->rstack_ptr    = (mword*)bvm_rstack_ptr(self);
    this_bvm->dstack_ptr    = (mword*)bvm_dstack_ptr(self);
    this_bvm->ustack_ptr    = (mword*)bvm_ustack_ptr(self);
    this_bvm->jump_table    = (mword*)bvm_jump_table(self);
    this_bvm->sym_table     = (mword*)bvm_sym_table(self);
    this_bvm->steps         = get_sym(this_bvm, "steps");
    this_bvm->advance_type  = get_sym(this_bvm, "advance_type");
    this_bvm->thread_id     = get_sym(this_bvm, "thread_id");

    return this_bvm;

}


//
//
bvm_cache *flush_bvm_cache(bvm_cache *this_bvm){ // flush_bvm_cache#

    mword *self = this_bvm->self;

    (mword*)bvm_code_ptr(self)      = this_bvm->code_ptr;
    (mword*)bvm_rstack_ptr(self)    = this_bvm->rstack_ptr;
    (mword*)bvm_dstack_ptr(self)    = this_bvm->dstack_ptr;
    (mword*)bvm_ustack_ptr(self)    = this_bvm->ustack_ptr;
    (mword*)bvm_jump_table(self)    = this_bvm->jump_table;
    (mword*)bvm_sym_table(self)     = this_bvm->sym_table;
    set_sym(this_bvm, "steps",        this_bvm->steps);
    set_sym(this_bvm, "advance_type", this_bvm->advance_type);
    set_sym(this_bvm, "thread_id",    this_bvm->thread_id);

    return this_bvm;

}

////#ifdef DEBUG
////
////void internal_bvmroot(void){
////
////    push_alloc(internal_global_VM, INTERNAL_BVMROOT);
////
////}
////
////#endif

////DEPRECATED
//void bbl2gv(mword *tree){
//
//    int i;
//
//    printf("digraph babel {\nnode [shape=record];\n");
//    printf("graph [rankdir = \"LR\"];\n");
//
//    tree_bbl2gv(tree);
//
//    printf("}\n");
//
//    rclean(tree);
//
//}
//
//
////  
////
//mword tree_bbl2gv(mword *tree){
//
//    int i;
////    mword count = 0;
//
//    if( s(tree) & (MWORD_SIZE-1) ){
//        return 0;
//    }
//
//    if( is_href(tree) ){
//        printf("s%08x [style=dashed,shape=record,label=\"", (mword)tree);
//        for(i=0; i<HASH_SIZE; i++){
//            printf("<f%d> %x", i, *(mword *)(tree+i));
//            if(i<(HASH_SIZE-1)){
//                printf("|");
//            }
//        }
//        printf("\"];\n");
//        s(tree) |= 0x1;
//        return 0;
//    }
//
//    int num_elem = size(tree);
//
////    count = num_elem + 1;
//
//    s(tree) |= 0x1;
//
//    if(is_inte(tree)){
//
//        printf("\"s%08x\" [shape=record,label=\"", (mword)tree);
//        for(i=0; i<num_elem; i++){
//            printf("<f%d> %d", i, i);
//            if(i<(num_elem-1)){
//                printf("|");
//            }
//        }
//        printf("\"];\n");
//
//        for(i=0; i<num_elem; i++){
////            if(*(mword *)(tree+i) == nil && tree != (mword*)nada){
//            if(is_nil((mword *)scar(tree+i))){// == nil && tree != (mword*)nada){
//                continue;
//            }
//            printf("\"s%08x\":f%d -> \"s%08x\":f0;\n", (mword)tree, i, *(mword *)(tree+i));
//            tree_bbl2gv((mword *)*(tree+i));
//        }
//
//    }
//    else{ // is_leaf
//        printf("s%08x [style=bold,shape=record,label=\"", (mword)tree);
//        for(i=0; i<num_elem; i++){
//            printf("<f%d> %x", i, *(mword *)(tree+i));
//            if(i<(num_elem-1)){
//                printf("|");
//            }
//        }
//        printf("\"];\n");
//    }
//
////    return count;
//
//}

// Clayton Bauman 2011

