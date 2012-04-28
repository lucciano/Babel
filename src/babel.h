// babel.h
// Every .c file in the Babel project should include this file

#ifndef BABEL_H
#define BABEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <setjmp.h>

// TYPEDEFS
typedef void(*std_fn_ptr)(void);
typedef unsigned mword;
typedef signed   smword;

typedef struct {
    mword *alloc_atom;
    mword *alloc_cons;
    mword *alloc_hash;
    mword *alloc_stack;
    mword *alloc_rstack;
    mword *alloc_unmanaged;
} alloc;

typedef struct {
    mword *hidden;
    mword *sym_table;
    mword *code_ptr;
    mword *stack_ptr;
    mword *rstack_ptr;
    mword *jump_table;
    mword thread_id;
    mword *argv;
    mword steps;
    alloc machine;
} bvm_cache;

mword*      nil;

bvm_cache *interp_init(bvm_cache *root_bvm, int argc, char **argv);

// GLOBALS
//mword*      internal_global_VM; //Interpreter-visible machine pointer
//mword*      global_VM;          //Machine pointer
////mword       global_machine_page_size;
//jmp_buf     exception_env;
//int         exception_type;

// CONSTANTS
#define MWORD_SIZE sizeof(mword)
#define MWORD_BIT_SIZE (MWORD_SIZE << 3)
#define MWORDS(x) ((x)*MWORD_SIZE)

#define STRLEN(s) (sizeof(s)-1)
#define C2B(x)    (_c2b(x, STRLEN(x)))

#define HASH_BIT_SIZE 128
#define HASH_SIZE (HASH_BIT_SIZE/MWORD_BIT_SIZE)

// IA32 uses 4k pages and multiples...
#define ALLOC_PAGE_SIZE 4096

#define BITS_PER_BYTE 8

#define ROOT_INTERP_THREAD 0

#define WINDOWS
//#define STAR_NIX

// XXX This should be set to 3 for 64-bit compilation...
// I can't find a simple/direct way to connect this to MWORD_SIZE
// so it has to be defined independently
#define LOG_MWORD_SIZE 2

// UTILITIES

#define s(x)         (*((mword*)x-1))
#define is_leaf(x)   ((int)s((mword*)x) >  0)
#define is_inte(x)   ((int)s((mword*)x) <  0)
#define is_href(x)   ((int)s((mword*)x) == 0)

#define size(x)      (is_href(x)?HASH_SIZE:(abs(s(x))/MWORD_SIZE))
#define c(x,y)       (*(y + x))

#define is_nil(x)   ( is_href(x) ? (memcmp((x), nil, HASH_SIZE) == 0) : 0 )

#define is_false(x) (    is_leaf(x) && car(x) == 0 \
                     || !is_leaf(x) && is_nil(scar(x)) )

#define car(x)      c((mword*)x,0)
#define cdr(x)      c((mword*)x,1)

//nil-safe car/cdr:
#define scar(x)     (is_nil(x) ? nil : car(x))
#define scdr(x)     (is_nil(x) ? nil : cdr(x))

#define cons(a,b,c) car(a) = (mword)(b); cdr(a) = (mword)(c);

//XXX we may yet need a generally hash-ref-safe car/cdr

//#define cadr(x)     car(cdr(x))
//#define caddr(x)    car(cdr(cdr(x)))
//
////#define TOS_0         car(car(cdr(car(stack_ptr))))
////#define TOS_1     car(car(cdr(car(cdr(stack_ptr)))))
////#define TOS_2 car(car(cdr(car(cdr(cdr(stack_ptr))))))

#define TOS_0(x)             car(car(x->stack_ptr))
#define TOS_1(x)         car(car(cdr(x->stack_ptr)))
#define TOS_2(x)     car(car(cdr(cdr(x->stack_ptr))))

#define alloc_type(x) car(car(cdr(x))) 

//#define RTOS_0             car(rstack_ptr)
//#define RTOS_1         car(cdr(rstack_ptr))
//#define RTOS_2     car(cdr(cdr(rstack_ptr)))
//#define RTOS_3 car(cdr(cdr(cdr(rstack_ptr))))
//
//#define global_argv car(internal_global_VM)
//
//#define code_list   global_VM
//#define data_list   cdr(code_list)
//#define stack_list  cdr(data_list)
//#define rstack_list cdr(stack_list)
//#define jump_table  cdr(rstack_list)
//#define sym_table   cdr(jump_table)
//#define nada        cdr(sym_table)
////#define nil         cdr(nada)
//#define null        car(nada)
//
//#define code_ptr    car(code_list)
//#define stack_ptr   car(stack_list)
//#define rstack_ptr  car(rstack_list)
//#define jump_ptr    car(jump_table)
//#define sym_ptr     car(sym_table)
//
//#define code_empty   ((mword)  code_ptr == nil)
//#define stack_empty  ((mword) stack_ptr == nil)
//#define rstack_empty ((mword)rstack_ptr == nil)
//#define jmp_empty    ((mword)  jump_ptr == nil)
//#define sym_empty    ((mword)   sym_ptr == nil)
//
//#define machine_empty (        code_empty   \
//                            && stack_empty  \
//                            && rstack_empty \
//                            && jmp_empty    \
//                            && sym_empty        )

// DEBUG
#define DIE_CODE 1
#define QUOTEME(x) #x
#define d(x) printf("%s %08x\n", QUOTEME(x), x);
#define die  fprintf(stderr, "Died at %s line %d\n", __FILE__, __LINE__); exit(DIE_CODE);
#define warn(x) fprintf(stderr, "WARNING: %s at %s line %d\n", x, __FILE__, __LINE__);
#define error(x) fprintf(stderr, "ERROR: %s at %s line %d\n", x, __FILE__, __LINE__);

#endif //BABEL_H

// Clayton Bauman 2011

