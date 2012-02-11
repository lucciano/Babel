// ref.c
//

#include "babel.h"
#include "ref.h"
#include "pearson16.h"
#include "except.h"
#include "bvm_opcodes.h"
#include "stack.h"
#include "bvm.h"
#include "list.h"

void newref(void){

    mword *result = _newref((mword*)TOS_0);
    zap();
    push_alloc(result, NEWREF);

}

mword *_newref(mword *hash){

    if(size(hash) != HASH_SIZE)
        except("_newref: size(hash) != HASH_SIZE", __FILE__, __LINE__);

    mword *ptr = malloc( MWORDS(HASH_SIZE+1) );
    if(ptr == NULL){
        except("_newlf: malloc returned NULL", __FILE__, __LINE__);
    }

    ptr[0] = 0;
    mword *result = (ptr+1);

    memcpy(result, hash, MWORDS(HASH_SIZE) );

    return result;

}

void ishref(void){

    mword *result    = new_atom();
    
    *result = is_href((mword*)TOS_0);

    zap();
    push_alloc(result, ISHREF);

}

// Clayton Bauman 2012

