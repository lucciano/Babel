// mem.c
//

// XXX Might have to perform a special memory check before doing a babelop
// due to potential cache-conflicts with mc_alloc if anything goes wrong...
// basically, anything that uses flush_bvm_cache or update_bvm_cache has
// to be careful that a memory error cannot occur.

/* memory guts

This is the interpreter's memory management subsystem and should not be 
confused with alloc.c which implements dynamic memory allocation operators
for use by the Babel programmer.

The allocator begins by allocating a bank of size MEM\_SIZE/2. MEM\_SIZE
indicates the largest amount of memory that the interpreter is allowed to
use. The alloc_ptr is set to the end of the allocated bank. Allocations 
occur by subtracting from alloc_ptr because, in certain cases, we can free
memory just by adding to alloc_ptr the size of the current elemented pointed
to by alloc_ptr.

Once the bank has been filled, the allocator allocates another bank of 
size MEM\_SIZE/2 and performs an "unload-load" on the entire BVM to move it
from the present bank to the new bank, where all unused memory will have
been released and the entire BVM will be defragmented, all in one step. The
old bank is freed back to the system. This is the only form of garbage-
collection.

This allocator/GC is *not* the final draft allocator because it's an old-
fashioned "stop-and-copy" GC. It will cause intermittent, awkward pauses 
during program execution (which can lead to bugs when dealing with I/O 
systems). It is being used only because it is very easy to ensure the 
implementation is bug-free. Gradually, the GC will be made more 
"incremental".

*/

#include "mem.h"
#include "load.h"


/////////////////////////////////////////////////////////////////////////////
// Babel> grep -n _newva src/* | grep FIXME
//
// Accepts a data value and returns a leaf-array
// of size 1 containing that data value
//
mword *_new2va(bvm_cache *this_bvm, mword value){

    mword *ptr = mc_2alloc(this_bvm, MWORDS(1));

    c(ptr,0) = value;
    return ptr;

}


// creates a new leaf-array of given size
//
mword *_new2lf(bvm_cache *this_bvm, mword size){

    mword *ptr = mc_2alloc( this_bvm, MWORDS(size) );

    return ptr;

}


// same as _newlf but with mem initialization (byte-wise)
//
mword *_new2lfi(bvm_cache *this_bvm, mword size, mword init){

    //mword *ptr = mc_2alloc( this_bvm, MWORDS(size) );
    mword *ptr = _new2lf(this_bvm, size);

    memset((char*)ptr,init,MWORDS(size));

    return ptr;

}


//
//
mword *_new2in(bvm_cache *this_bvm, mword size){

    mword *ptr = mc_2alloc( this_bvm, -1*MWORDS(size) );

    int i;
    for(i = 0; i<size; i++){ // All pointers must be valid - initialize to nil
        ptr[i] = (mword)nil;
    }

    return ptr;

}


//
mword *_new2tptr(bvm_cache *this_bvm){

    mword *ptr = mc_2alloc( this_bvm, 0 );

    //FIXME: 32-bit specific and UGLY
    ptr[0] = 0xdeadbeef;
    ptr[1] = 0xdeadbeef;
    ptr[2] = 0xdeadbeef;
    ptr[3] = 0xdeadbeef;
    ptr[4] = (mword)(-1*MWORD_SIZE);
    ptr[5] = (mword)nil;

    return ptr;

}


// Allocating cons
//
mword *consa2(bvm_cache *this_bvm, mword *car_field, mword *cdr_field){

    mword *temp_cons = _new2in(this_bvm, 2);

    cons(temp_cons, car_field, cdr_field);

    return temp_cons;

}


// mc_init
//
void mc_init(void){

    mem_context *m = malloc(sizeof(mem_context));

    m->primary   = malloc(sizeof(alloc_bank));
    m->secondary = malloc(sizeof(alloc_bank));

    m->primary->base_ptr   = malloc(MWORDS(MEM_SIZE>>1));
    m->secondary->base_ptr = malloc(MWORDS(MEM_SIZE>>1));

    m->primary->alloc_ptr   = TOP_OF_ALLOC_BANK(m->primary  );
    m->secondary->alloc_ptr = TOP_OF_ALLOC_BANK(m->secondary);

    mem = m; // Sets the global memory context

}


// mc_destroy
//
// Undoes mc_init
//
void mc_destroy(void){

    free(mem->primary->base_ptr  );
    free(mem->secondary->base_ptr);

    free(mem->primary);
    free(mem->secondary);

    free(mem);

}


// mc_alloc
//
mword *mc_alloc(mword sfield){

    alloc_bank *b = mem->primary;

    mword mc_size = mc_alloc_size(sfield)+1;

    if(b->alloc_ptr-mc_size < b->base_ptr){
        // copy-collect
        fatal("mc_copy_collect");
        // mc_copy_collect(this_bvm);
    }

    b->alloc_ptr -= mc_size;

    mword *return_ptr = b->alloc_ptr+1;

    //r(return_ptr) = mc_size;
    s(return_ptr) = sfield;

    return return_ptr;

}

// mc_2alloc
//
mword *mc_2alloc(bvm_cache *this_bvm, mword sfield){

    alloc_bank *b = mem->primary;

    mword mc_size = mc_alloc_size(sfield)+1;

    if(b->alloc_ptr-mc_size < b->base_ptr){
        // copy-collect
        fatal("mc_copy_collect");
        // mc_copy_collect(this_bvm);
    }

    b->alloc_ptr -= mc_size;

    mword *return_ptr = b->alloc_ptr+1;

    //r(return_ptr) = mc_size;
    s(return_ptr) = sfield;

    return return_ptr;

}


//// mc_alloc
////
//mword *mc_alloc(mword sfield){
//
//    alloc_bank *b = mem->primary;
//
//    mword mc_size = mc_alloc_size(sfield)+2;
//
////    if(b->alloc_ptr-mc_size < b->base_ptr){
////        // copy-collect
////        fatal("mc_copy_collect");
////    }
//
//    b->alloc_ptr -= mc_size;
//
//    mword *return_ptr = b->alloc_ptr+3;
//
//    //r(return_ptr) = mc_size;
//    r(return_ptr) = mc_size;
//    s(return_ptr) = sfield;
//
//    return return_ptr;
//
//}


// mc_free
//
void mc_free(mword *p){

////trace;
//
//#define MARK_NOT_IN_USE(x) (-1*abs(x))
//
//    r(p) = MARK_NOT_IN_USE(r(p));
////    r(p) = -1*r(p);
////    d(r(p));
//
////    d(p);
////    d(mem->primary->alloc_ptr);
//
//    mc_reclamate();

}

// mc_reclamate
//
void mc_reclamate(void){

    alloc_bank *b = mem->primary;

#define is_free(x) (*(x)<0)
#define reclamate_elem(x) x->alloc_ptr += abs(*(x->alloc_ptr+1))

    while( b->alloc_ptr < TOP_OF_ALLOC_BANK(b) 
            && is_free(b->alloc_ptr+1) ){

        reclamate_elem(b);
trace;

    }

}


// mc_reset_bank
//
void mc_reset_bank(alloc_bank *b){

    b->alloc_ptr = TOP_OF_ALLOC_BANK(b);

}


// mc_bank_usage
//
mword mc_bank_usage(alloc_bank *b){

    return (TOP_OF_ALLOC_BANK(b) - b->alloc_ptr);

}


// mc_copy_collect
//
mword mc_copy_collect(bvm_cache *this_bvm){

#define swap_banks                      \
alloc_bank *temp    = mem->primary;     \
mem->primary        = mem->secondary;   \
mem->secondary      = mem->primary;     \

    static float mc_compaction_ratio=0;

    flush_bvm_cache(this_bvm);

    mc_reset_bank(mem->secondary);

    if(0){
//    if(mc_compaction_ratio!=0){

        if( (mc_compaction_ratio >= (1/3)) //FIXME: naked constants
                && (mc_compaction_ratio <= (2/3))){
            swap_banks;
        }
        else{
            if(mc_compaction_ratio < (1/3)){
                //set MEM_SIZE=MEM_SIZE/2
            }
            else{ //mc_compaction_ratio > (2/3)
                //set MEM_SIZE=MEM_SIZE*2
            }
            //free old secondary bank and allocate a new one
        }

    }
    else{
        swap_banks;
    }

    //now call _cp
    mword *temp_bvm = _cp(this_bvm->self);

    this_bvm->self = temp_bvm;

    update_bvm_cache(this_bvm);

    // swap primary<->secondary alloc banks
    // _unload this_bvm

    //mc_compaction_ratio = mc_bank_usage(mem->primary);

    return mc_compaction_ratio;

}

// mc_collect_or_expand
// mc_copy_expand

// Clayton Bauman 2013

