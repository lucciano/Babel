// XXX STACK FIX DONE
// eval.c
//

#include "babel.h"
#include "eval.h"
#include "stack.h"
#include "bstruct.h"
#include "debug.h"
#include "bvm_opcodes.h"
#include "array.h"
#include "bvm.h"
#include "list.h"

void eval(void){

//    d(car(car(car(TOS_0))))
//        die

    mword temp_TOS_0 = TOS_0;
    zap();

    push_alloc_rstack((mword*)cdr(code_ptr), EVAL);

    code_ptr = temp_TOS_0;
//    car(code_ptr) = TOS_0;
//    zap();

}

void gotoop(void){

    code_ptr = car(car(stack_ptr)); // PRE STACK FIX
//    code_ptr = car(stack_ptr);
    zap();

}

//void call(void){
//
//    push_alloc_rstack((mword*)cdr(code_ptr), CALL);
//    code_ptr = car(car(stack_ptr)); // PRE STACK FIX
////    code_ptr = car(stack_ptr);
//    zap();
//
//}
//
//void ret(void){
//
//    // FIXME: CHECK FOR EMPTY RSTACK
//    code_ptr = car(pop_rstack());
//
//}

void loop(void){

//    push_alloc_rstack((mword*)code_ptr, LOOP);

//    push_rstack((mword*)TOS_1);
//    push_rstack((mword*)TOS_0);
//    zap();
//    zap();
//
//    push_rstack((mword*)cdr(code_ptr));

    // return RTOS-1
    // body   RTOS-0

    mword temp_TOS_0 = TOS_0;
    zap();

    mword *temp = cons_alloc((mword*)cdr(code_ptr), (mword*)nil);
    temp = cons_alloc((mword*)temp_TOS_0,temp);
//    push_alloc_rstack((mword*)cdr(code_ptr), LOOP);
//    push_alloc_rstack((mword*)temp_TOS_0, LOOP);

    push_alloc_rstack(temp, LOOP);

    code_ptr = temp_TOS_0;

}

//void last(void){
//    mword *discard = (mword*)car(pop_rstack());
//    code_ptr = nil;
//}

mword _end_of_code(void){

    mword* discard;

    if(!rstack_empty){
        while(  alloc_type(RTOS_0) == DOWN ||
                alloc_type(RTOS_0) == NEST){
            up();
        }
        if(alloc_type(RTOS_0) == TIMES){
            if(car(car(car(RTOS_0))) > 1){
                c((mword*)car(car(RTOS_0)),0) = car(car(car(RTOS_0))) - 1;
                (mword*)code_ptr = (mword*)car(cdr(car((RTOS_0))));
            }
            else{
                (mword*)code_ptr = (mword*)car(cdr(cdr(car(pop_rstack()))));
            }
        }
        else if(alloc_type(RTOS_0) == WHILEOP){
            if(car(car(car(RTOS_0)))){ // goto cond_block
                (mword*)code_ptr = (mword*)car(cdr(cdr(car(RTOS_0))));
                c((mword*)car(car(RTOS_0)),0) = 0;
            }
            else{ // check TOS; if true, goto loop body, else last
                if(!is_false((mword*)TOS_0)){
                    zap();
                    (mword*)code_ptr = (mword*)car(cdr(car(RTOS_0)));
                    c((mword*)car(car(RTOS_0)),0) = 1;
                    //push_alloc_rstack((mword*)car(RTOS_2), 0);
                }
                else{
                    (mword*)code_ptr = (mword*)car(cdr(cdr(cdr(car(RTOS_0)))));
                    discard = pop_rstack();
                }
            }
        }
        else if(alloc_type(RTOS_0) == EACH){

            if(cdr(car(car(RTOS_0))) != nil){
                c((mword*)car(RTOS_0),0) = cdr(car(car(RTOS_0)));
                push_alloc((mword*)car(car(car(RTOS_0))), EACH);
                code_ptr = car(cdr(car(RTOS_0)));
            }
            else{
                code_ptr = car(cdr(cdr(car(RTOS_0))));
                discard = pop_rstack();
            }
        }
        else if(alloc_type(RTOS_0) == EACHAR){

            if(car(car(car(RTOS_0))) < size((mword*)car(cdr(car(RTOS_0))))-1){

                c((mword*)car(car(RTOS_0)),0) = car(car(car(RTOS_0))) + 1;
                push_alloc((mword*)c((mword*)car(cdr(car(RTOS_0))),car(car(car(RTOS_0)))),EACHAR);
                code_ptr = car(cdr(cdr(car(RTOS_0))));

            }
            else{
                code_ptr = car(cdr(cdr(cdr(car(RTOS_0)))));
                discard = pop_rstack();
            }

        }
        else if(alloc_type(RTOS_0) == LOOP){
            (mword*)code_ptr = (mword*)car(car((RTOS_0)));
        }
        else{ //FIXME: Make this an exception
            (mword*)code_ptr = (mword*)car(pop_rstack());
        }
        return 1;
    }

    return 0;

}

void last(void){

    mword* discard;

    if(!rstack_empty){
        while(  alloc_type(RTOS_0) == DOWN ||
                alloc_type(RTOS_0) == NEST){
            up();
        }
        if(alloc_type(RTOS_0) == TIMES){
//            if(car(car(car(RTOS_0))) > 1){
//                c((mword*)car(car(RTOS_0)),0) = car(car(car(RTOS_0))) - 1;
//                (mword*)code_ptr = (mword*)car(cdr(car((RTOS_0))));
//            }
//            else{
                (mword*)code_ptr = (mword*)car(cdr(cdr(car(pop_rstack()))));
//            }
        }
        else if(alloc_type(RTOS_0) == WHILEOP){
//            if(car(car(car(RTOS_0)))){ // goto cond_block
//                (mword*)code_ptr = (mword*)car(cdr(cdr(car(RTOS_0))));
//                c((mword*)car(car(RTOS_0)),0) = 0;
//            }
//            else{ // check TOS; if true, goto loop body, else last
//                if(!is_false((mword*)TOS_0)){
//                    zap();
//                    (mword*)code_ptr = (mword*)car(cdr(car(RTOS_0)));
//                    c((mword*)car(car(RTOS_0)),0) = 1;
//                    //push_alloc_rstack((mword*)car(RTOS_2), 0);
//                }
//                else{
                    (mword*)code_ptr = (mword*)car(cdr(cdr(cdr(car(RTOS_0)))));
                    discard = pop_rstack();
//                }
//            }
        }
        else if(alloc_type(RTOS_0) == EACH){

//            if(cdr(car(car(RTOS_0))) != nil){
//                c((mword*)car(RTOS_0),0) = cdr(car(car(RTOS_0)));
//                push_alloc((mword*)car(car(car(RTOS_0))), EACH);
//                code_ptr = car(cdr(car(RTOS_0)));
//            }
//            else{
                code_ptr = car(cdr(cdr(car(RTOS_0))));
                discard = pop_rstack();
//            }
        }
        else if(alloc_type(RTOS_0) == EACHAR){

//            if(car(car(car(RTOS_0))) < size((mword*)car(cdr(car(RTOS_0))))-1){
//
//                c((mword*)car(car(RTOS_0)),0) = car(car(car(RTOS_0))) + 1;
//                push_alloc((mword*)c((mword*)car(cdr(car(RTOS_0))),car(car(car(RTOS_0)))),EACHAR);
//                code_ptr = car(cdr(cdr(car(RTOS_0))));
//
//            }
//            else{
                code_ptr = car(cdr(cdr(cdr(car(RTOS_0)))));
                discard = pop_rstack();
//            }

        }
        else if(alloc_type(RTOS_0) == LOOP){
            die
            (mword*)code_ptr = (mword*)car(cdr(car((RTOS_0))));
            discard = pop_rstack();
        }
        else if(alloc_type(RTOS_0) == EVAL){
            (mword*)code_ptr = (mword*)car(pop_rstack());
            last(); //This is needed for conditional loop breaks
            // You don't need last to break out of an eval, just close the code list and you'll break out
        }
        else{ //FIXME: Make this an exception
            (mword*)code_ptr = (mword*)car(pop_rstack());
        }
    }
    else{ //XXX break from interpreter?
        code_ptr = cdr(code_ptr); //Do nothing
    }
    die

}

void next(void){
}

// (body) (cond) while
void whileop(void){

//    //body   RTOS-0
//    //return RTOS-1
//    //cond   RTOS-2
//
//    mword *cond_block = (mword*)TOS_0;
//    push_alloc_rstack((mword*)cond_block,    WHILEOP);
//    push_alloc_rstack((mword*)cdr(code_ptr), WHILEOP);
//
//    zap();
//
//    push_alloc_rstack((mword*)TOS_0, WHILEOP); //loop body
//
//    zap();
//
//    (mword*)code_ptr = cond_block;

    // return     -> RTOS-3
    // cond_block -> RTOS-2
    // body       -> RTOS-1
    // next       -> RTOS

    mword *temp;
    mword *cond_block = (mword*)TOS_0;

    temp = cons_alloc((mword*)cdr(code_ptr), (mword *)nil);
    temp = cons_alloc(cond_block, temp);

    zap();
    temp = cons_alloc((mword*)TOS_0, temp);

    zap();
    mword *block_sel = new_atom();
    *block_sel = WHILE_BODY;
    temp = cons_alloc(block_sel, temp);

    push_alloc_rstack(temp,WHILEOP);
    (mword*)code_ptr = cond_block;

}

// (body) [x] times
void times(void){

    //times  RTOS-2
    //return RTOS-1
    //body   RTOS-0

//    if(car(TOS_0) > 0){
//        mword *times = new_atom();
//        *times = car(TOS_0);
//        push_alloc_rstack(times, TIMES);
//        push_alloc_rstack((mword*)cdr(code_ptr), TIMES);
//
//        zap();
//        mword temp_TOS_0 = TOS_0;
//        zap();
//        code_ptr = temp_TOS_0;
//        push_alloc_rstack((mword*)code_ptr, TIMES);
//    }
//    else{
//        code_ptr = cdr(code_ptr);
//    }

    mword *temp;
    mword *times;

    if(car(TOS_0) > 0){
        
        times = new_atom();
        *times = car(TOS_0);
//        mword *temp = cons_alloc(times, (mword *)nil);

        mword *return_ptr = (mword*)cdr(code_ptr);
        temp = cons_alloc(return_ptr, (mword*)nil);

        zap();
        mword temp_TOS_0 = TOS_0;
        zap();

        code_ptr = temp_TOS_0;
        temp = cons_alloc((mword*)code_ptr, temp);
        temp = cons_alloc(times, temp);

        push_alloc_rstack(temp, TIMES);

    }
    else{

        code_ptr = cdr(code_ptr);

    }

}

void dieop(void){
    fprintf(stderr, "Died.\n");
    exit(0);
}

void each(void){

//    //body   RTOS-0
//    //return RTOS-1
//    //list   RTOS-2
//
//    if(TOS_0 != nil){
//        push_alloc_rstack((mword*)TOS_0, EACH);
//        mword *temp_list = (mword*)TOS_0;
//        push_alloc_rstack((mword*)cdr(code_ptr), EACH);
//
//        zap();
//        mword temp_code_ptr = TOS_0;
//        zap();
//        code_ptr = temp_code_ptr;
//        push_alloc_rstack((mword*)code_ptr, EACH);
//        push_alloc((mword*)car(temp_list),EACH);
//
//    }
//    else{
//        code_ptr = cdr(code_ptr);
//    }

    //return RTOS-2
    //body   RTOS-1
    //list   RTOS-0

    mword *temp;
    if(TOS_0 != nil){

        mword *temp_list = (mword*)TOS_0;
        mword *temp_code_ptr = (mword*)cdr(code_ptr);

        zap();

        code_ptr = TOS_0;

        zap();

        temp = cons_alloc(temp_code_ptr, (mword*)nil);
        temp = cons_alloc((mword*)code_ptr, temp);
        temp = cons_alloc(temp_list, temp);
        
        push_alloc_rstack(temp, EACH);       
        push_alloc((mword*)car(temp_list),EACH);

    }
    else{

        code_ptr = cdr(code_ptr);

    }

}

void eachar(void){

    //body   RTOS-0
    //return RTOS-1
    //array  RTOS-2
    //count  RTOS-3

//    if(TOS_0 != nil){
//        mword *count = _newlf(1);
//        *count = 0;
//        push_alloc_rstack((mword*)count,EACHAR);
//        push_alloc_rstack((mword*)TOS_0, EACHAR);
//        mword *temp_array = (mword*)TOS_0;
//        push_alloc_rstack((mword*)cdr(code_ptr), EACHAR);
//
//        zap();
//        mword temp_code_ptr = TOS_0;
//        zap();
//        code_ptr = temp_code_ptr;
//        push_alloc_rstack((mword*)code_ptr, EACHAR);
//        push_alloc((mword*)c(temp_array,*count),EACHAR);
//    }
//    else{
//        code_ptr = cdr(code_ptr);
//    }

    //return RTOS-3
    //body   RTOS-2
    //array  RTOS-1
    //count  RTOS-0

    mword *temp;
    if(TOS_0 != nil){

        mword *count = _newlf(1);
        *count = 0;

        mword *temp_list = (mword*)TOS_0;
        mword *temp_code_ptr = (mword*)cdr(code_ptr);

        zap();

        code_ptr = TOS_0;

        zap();

        temp = cons_alloc(temp_code_ptr, (mword*)nil);
        temp = cons_alloc((mword*)code_ptr, temp);
        temp = cons_alloc(temp_list, temp);
        temp = cons_alloc(count, temp);
        
        push_alloc_rstack(temp, EACHAR);       
        push_alloc((mword*)c(temp_list,*count),EACHAR);

    }
    else{

        code_ptr = cdr(code_ptr);

    }


}

// Clayton Bauman 2011

