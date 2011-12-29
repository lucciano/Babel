// XXX STACK FIX DONE
// pearson16.c
//

#include "babel.h"
#include "pearson16.h"
#include "list.h"
#include "stack.h"
#include "bvm_opcodes.h"
#include "array.h"
#include "except.h"

// init_hash key hash8
void hash8(void){

    mword* result = _pearson16((mword*)TOS_1, (mword*)TOS_0, _arlen8((mword*)TOS_0));

    zap();
    zap();
    push_alloc((mword*)result, HASH8);

}

//
void hash(void){

    mword* result = _pearson16((mword*)TOS_1, (mword*)TOS_0, size((mword*)TOS_0)*MWORD_SIZE);

    zap();
    zap();
    push_alloc((mword*)result, HASH);

}


// This is a 16-byte version of Pearson's hash
// It does NOT follow Pearson's recommendation for building wider hashes from 
// the paper on his 8-bit hash function.
mword *_pearson16(mword* sinit, mword *skey, mword strlen){

    // sinit = TOS_1
    // skey = TOS_0
    char *init = (char*)(sinit); 
    char *key  = (char*)(skey);

//    mword strlen = _arlen8((mword*)skey);

    char *result = (char*)new_hash();

    int i;
    char temp;

    for(i=0; i<HASH_SIZE*MWORD_SIZE; i++){
        result[i] = init[i];
    }

    for(i=0;i<strlen;i++){
        temp = result[0];
        result[0 ] = pearson16_byte_perm[ (1 *256) + ((result[1 ] ^ key[i]) & 0xff) ];
        result[1 ] = pearson16_byte_perm[ (2 *256) + ((result[2 ] ^ key[i]) & 0xff) ];
        result[2 ] = pearson16_byte_perm[ (3 *256) + ((result[3 ] ^ key[i]) & 0xff) ];
        result[3 ] = pearson16_byte_perm[ (4 *256) + ((result[4 ] ^ key[i]) & 0xff) ];
        result[4 ] = pearson16_byte_perm[ (5 *256) + ((result[5 ] ^ key[i]) & 0xff) ];
        result[5 ] = pearson16_byte_perm[ (6 *256) + ((result[6 ] ^ key[i]) & 0xff) ];
        result[6 ] = pearson16_byte_perm[ (7 *256) + ((result[7 ] ^ key[i]) & 0xff) ];
        result[7 ] = pearson16_byte_perm[ (8 *256) + ((result[8 ] ^ key[i]) & 0xff) ];
        result[8 ] = pearson16_byte_perm[ (9 *256) + ((result[9 ] ^ key[i]) & 0xff) ];
        result[9 ] = pearson16_byte_perm[ (10*256) + ((result[10] ^ key[i]) & 0xff) ];
        result[10] = pearson16_byte_perm[ (11*256) + ((result[11] ^ key[i]) & 0xff) ];
        result[11] = pearson16_byte_perm[ (12*256) + ((result[12] ^ key[i]) & 0xff) ];
        result[12] = pearson16_byte_perm[ (13*256) + ((result[13] ^ key[i]) & 0xff) ];
        result[13] = pearson16_byte_perm[ (14*256) + ((result[14] ^ key[i]) & 0xff) ];
        result[14] = pearson16_byte_perm[ (15*256) + ((result[15] ^ key[i]) & 0xff) ];
        result[15] = pearson16_byte_perm[ (0 *256) + ((temp       ^ key[i]) & 0xff) ];
    }

//    for(i=0;i<16;i++){
//         printf( "%02x", (unsigned char)result[i]);
//    }

//    mword *temp_cons = new_cons();
//    cons(temp_cons, (mword*)result, nil);

    return (mword*)result;

}

mword *new_hash(void){

    mword *ptr = malloc( MWORDS(HASH_SIZE+1) );
    if(ptr == NULL){
        except("new_hash: malloc returned NULL", __FILE__, __LINE__);
    }

    ptr[0] = HASH_SIZE * MWORD_SIZE;
    return ptr+1;

}

// pearson16_init
//
void pearson16_init(void){

    // Security note: This hash IS NOT cryptographically secure. The first 256 bytes were copied from
    // Pearson's original paper and the rest were retrieved from www.random.org but these are NOT
    // "nothing-up-my-sleeve" numbers and should not be treated as such. For security purposes, the 
    // hash should always be treated as plain-text. It should not be treated as noise or random data, 
    // even though it appears random.
    static char byte_array[4096] = 
    {

        0x01, 0x57, 0x31, 0x0c, 0xb0, 0xb2, 0x66, 0xa6, 0x79, 0xc1, 0x06, 0x54, 0xf9, 0xe6, 0x2c, 0xa3, 
        0x0e, 0xc5, 0xd5, 0xb5, 0xa1, 0x55, 0xda, 0x50, 0x40, 0xef, 0x18, 0xe2, 0xec, 0x8e, 0x26, 0xc8, 
        0x6e, 0xb1, 0x68, 0x67, 0x8d, 0xfd, 0xff, 0x32, 0x4d, 0x65, 0x51, 0x12, 0x2d, 0x60, 0x1f, 0xde, 
        0x19, 0x6b, 0xbe, 0x46, 0x56, 0xed, 0xf0, 0x22, 0x48, 0xf2, 0x14, 0xd6, 0xf4, 0xe3, 0x95, 0xeb, 
        0x61, 0xea, 0x39, 0x16, 0x3c, 0xfa, 0x52, 0xaf, 0xd0, 0x05, 0x7f, 0xc7, 0x6f, 0x3e, 0x87, 0xf8, 
        0xae, 0xa9, 0xd3, 0x3a, 0x42, 0x9a, 0x6a, 0xc3, 0xf5, 0xab, 0x11, 0xbb, 0xb6, 0xb3, 0x00, 0xf3, 
        0x84, 0x38, 0x94, 0x4b, 0x80, 0x85, 0x9e, 0x64, 0x82, 0x7e, 0x5b, 0x0d, 0x99, 0xf6, 0xd8, 0xdb, 
        0x77, 0x44, 0xdf, 0x4e, 0x53, 0x58, 0xc9, 0x63, 0x7a, 0x0b, 0x5c, 0x20, 0x88, 0x72, 0x34, 0x0a, 
        0x8a, 0x1e, 0x30, 0xb7, 0x9c, 0x23, 0x3d, 0x1a, 0x8f, 0x4a, 0xfb, 0x5e, 0x81, 0xa2, 0x3f, 0x98,
        0xaa, 0x07, 0x73, 0xa7, 0xf1, 0xce, 0x03, 0x96, 0x37, 0x3b, 0x97, 0xdc, 0x5a, 0x35, 0x17, 0x83, 
        0x7d, 0xad, 0x0f, 0xee, 0x4f, 0x5f, 0x59, 0x10, 0x69, 0x89, 0xe1, 0xe0, 0xd9, 0xa0, 0x25, 0x7b, 
        0x76, 0x49, 0x02, 0x9d, 0x2e, 0x74, 0x09, 0x91, 0x86, 0xe4, 0xcf, 0xd4, 0xca, 0xd7, 0x45, 0xe5, 
        0x1b, 0xbc, 0x43, 0x7c, 0xa8, 0xfc, 0x2a, 0x04, 0x1d, 0x6c, 0x15, 0xf7, 0x13, 0xcd, 0x27, 0xcb, 
        0xe9, 0x28, 0xba, 0x93, 0xc6, 0xc0, 0x9b, 0x21, 0xa4, 0xbf, 0x62, 0xcc, 0xa5, 0xb4, 0x75, 0x4c, 
        0x8c, 0x24, 0xd2, 0xac, 0x29, 0x36, 0x9f, 0x08, 0xb9, 0xe8, 0x71, 0xc4, 0xe7, 0x2f, 0x92, 0x78, 
        0x33, 0x41, 0x1c, 0x90, 0xfe, 0xdd, 0x5d, 0xbd, 0xc2, 0x8b, 0x70, 0x2b, 0x47, 0x6d, 0xb8, 0xd1,

        0x5b, 0x32, 0x81, 0x52, 0x37, 0xfe, 0xec, 0xa9, 0x49, 0x29, 0xfd, 0x4a, 0xe4, 0x20, 0x19, 0xd4,
        0x6c, 0xe9, 0x70, 0x4d, 0x99, 0x13, 0x95, 0xb2, 0xb4, 0x72, 0xbb, 0x7b, 0x5e, 0x77, 0x11, 0x31, 
        0xa8, 0xef, 0x3d, 0xaf, 0xc6, 0x98, 0xc7, 0x92, 0x5c, 0x39, 0xa2, 0xc4, 0x7d, 0xf6, 0x8a, 0x0a, 
        0xd8, 0xb0, 0xca, 0xf7, 0x47, 0xa3, 0x56, 0xea, 0x75, 0x9d, 0x59, 0x93, 0x44, 0x8c, 0xf9, 0xad, 
        0x21, 0xfc, 0x7e, 0xe6, 0x40, 0xe1, 0x3b, 0xed, 0x60, 0x06, 0x25, 0x73, 0x2d, 0x3e, 0x09, 0x76,
        0xda, 0x5d, 0x48, 0xe0, 0xbf, 0x0d, 0x03, 0x1a, 0x9f, 0xb6, 0xdb, 0x7f, 0x91, 0x94, 0xc9, 0xde, 
        0xeb, 0xd3, 0x10, 0x84, 0x7c, 0x36, 0xb3, 0xcb, 0x22, 0xb1, 0x14, 0x61, 0x01, 0xe2, 0xac, 0x74,
        0xa1, 0xd5, 0xa4, 0xf0, 0x3c, 0x53, 0x1b, 0xba, 0x6f, 0x0e, 0xae, 0x78, 0x08, 0x88, 0xd0, 0xdf,
        0xc1, 0x12, 0x85, 0x05, 0x86, 0xf1, 0x97, 0x0c, 0x5f, 0x38, 0x18, 0xee, 0xbe, 0xe5, 0x9b, 0x68, 
        0xcf, 0x2e, 0x4e, 0x1c, 0xfb, 0x1f, 0x16, 0x3f, 0x1e, 0x54, 0xc8, 0xd7, 0xaa, 0x8d, 0x6b, 0xf5, 
        0x00, 0xab, 0x28, 0xc2, 0x2c, 0xa6, 0x4b, 0xa5, 0x2a, 0x3a, 0xe7, 0xa7, 0x8e, 0xe8, 0x58, 0xf2, 
        0x80, 0x87, 0xbc, 0x6a, 0x33, 0x82, 0x9a, 0x67, 0x4f, 0xb8, 0x43, 0xf8, 0x27, 0x9e, 0x41, 0xc5, 
        0x65, 0xf4, 0xd1, 0x1d, 0x63, 0x45, 0x8b, 0x9c, 0x90, 0xcd, 0x34, 0x51, 0x17, 0xc3, 0x24, 0x46, 
        0xcc, 0x07, 0xdc, 0x15, 0xd9, 0xa0, 0xd6, 0x71, 0xd2, 0x66, 0xce, 0x79, 0x42, 0x6d, 0x04, 0x0b, 
        0xc0, 0x4c, 0x96, 0x30, 0xfa, 0x83, 0x69, 0xb7, 0xbd, 0xf3, 0xb5, 0x8f, 0xb9, 0x2f, 0x02, 0x7a, 
        0x23, 0x64, 0x35, 0x0f, 0x6e, 0x55, 0x89, 0xe3, 0x5a, 0x62, 0x50, 0xdd, 0x2b, 0x57, 0x26, 0xff, 

        0x04, 0xb4, 0x6f, 0xef, 0x9f, 0x21, 0x43, 0xbb, 0x35, 0x48, 0x26, 0xc9, 0x0d, 0x6c, 0xd6, 0xeb, 
        0x92, 0x94, 0x69, 0x29, 0x88, 0xee, 0x3d, 0x89, 0x8d, 0x23, 0xf6, 0x08, 0xe2, 0x3c, 0x05, 0x3b,
        0x7c, 0x83, 0x79, 0x5a, 0x0b, 0x96, 0x6d, 0x77, 0x00, 0x4f, 0xa9, 0xc5, 0xb9, 0x8f, 0xc3, 0x31, 
        0xff, 0xf3, 0x87, 0xa6, 0xac, 0x6e, 0x9a, 0x11, 0xbc, 0x76, 0xb0, 0x22, 0xbd, 0xc4, 0xae, 0x64, 
        0xdd, 0x28, 0xea, 0x7e, 0xb5, 0x49, 0xd9, 0x1a, 0x0f, 0xf8, 0x7d, 0xb6, 0x95, 0x4c, 0xfe, 0x38, 
        0x4d, 0xa2, 0x70, 0x12, 0xc1, 0x51, 0x75, 0x1e, 0x56, 0x4a, 0x8e, 0x1f, 0xe3, 0xcf, 0x2b, 0x5b, 
        0x67, 0x45, 0x42, 0xe1, 0x3e, 0x90, 0x33, 0x46, 0xaf, 0x41, 0x8b, 0x1b, 0x81, 0x40, 0xdb, 0x7a, 
        0xd5, 0x9d, 0xa4, 0xbe, 0x61, 0x09, 0xd3, 0xbf, 0x6a, 0xa7, 0x4b, 0xf1, 0x68, 0x03, 0x54, 0x17, 
        0x2d, 0x6b, 0xba, 0xe0, 0x8a, 0x3f, 0xa3, 0xcc, 0x80, 0xa1, 0xfa, 0x93, 0x2a, 0xd4, 0xe7, 0xb2, 
        0xfc, 0xa0, 0x1d, 0x27, 0x58, 0x59, 0xe8, 0x85, 0xf9, 0x47, 0x55, 0x65, 0x4e, 0x2e, 0x72, 0x32, 
        0x19, 0xaa, 0x37, 0x0c, 0xdc, 0xd0, 0x8c, 0x36, 0xc6, 0x62, 0xa5, 0x5f, 0x99, 0x2c, 0xe5, 0xf4, 
        0xb8, 0x5d, 0x24, 0x3a, 0x84, 0xb1, 0x5e, 0x20, 0x07, 0xd7, 0x91, 0xd1, 0x82, 0xed, 0xec, 0x66, 
        0x30, 0x5c, 0x25, 0xcd, 0x44, 0xab, 0xc0, 0x2f, 0x7b, 0xa8, 0x10, 0xc7, 0x13, 0xfd, 0xc8, 0x7f, 
        0xe6, 0xf0, 0x0a, 0x39, 0xd8, 0x60, 0xf7, 0x52, 0x78, 0x14, 0x86, 0x9c, 0xf5, 0xd2, 0xcb, 0x06, 
        0xfb, 0xda, 0x57, 0x50, 0xb7, 0xe9, 0x18, 0x15, 0x74, 0x34, 0x1c, 0xc2, 0xca, 0x71, 0x9e, 0x73, 
        0x01, 0xb3, 0x63, 0xdf, 0xe4, 0xf2, 0x16, 0x53, 0x98, 0x02, 0x97, 0xce, 0x9b, 0xad, 0x0e, 0xde, 

        0xe9, 0x4d, 0x0c, 0xd8, 0x43, 0x17, 0x8e, 0x6f, 0xcb, 0x09, 0x85, 0xb7, 0xf9, 0xdc, 0x70, 0xc0, 
        0x42, 0x7f, 0x62, 0x8a, 0x01, 0xa0, 0xa7, 0xfd, 0xf3, 0xc7, 0x05, 0x90, 0xe4, 0x82, 0x84, 0x7b, 
        0x69, 0xe2, 0xd3, 0x31, 0xb4, 0x9e, 0x12, 0xee, 0x5c, 0xb9, 0x83, 0x20, 0x23, 0xb0, 0x55, 0x88, 
        0xdd, 0x8f, 0x66, 0x46, 0xf4, 0x1b, 0x36, 0x48, 0x2f, 0xf6, 0xf0, 0xda, 0xf5, 0x51, 0xd4, 0x77, 
        0x7e, 0xde, 0x3c, 0xd2, 0x39, 0x5d, 0x0b, 0x97, 0xae, 0x56, 0xa5, 0x76, 0x60, 0xed, 0x6e, 0xad, 
        0x0e, 0x4c, 0x4e, 0x64, 0x63, 0x58, 0x08, 0x57, 0xbe, 0x53, 0x89, 0x4a, 0x75, 0x5e, 0xa9, 0xb1, 
        0x47, 0x98, 0x93, 0xcf, 0xac, 0x67, 0xc6, 0xa2, 0x4b, 0xf1, 0xfa, 0x1a, 0x40, 0xb8, 0xc1, 0x3e,
        0x0f, 0xd5, 0xd6, 0x22, 0x92, 0x6c, 0x03, 0x24, 0xd9, 0xfc, 0xc4, 0xab, 0x28, 0x9c, 0xc9, 0xa1, 
        0x1e, 0xd0, 0x1f, 0x11, 0x33, 0x71, 0x61, 0x3a, 0xbb, 0x54, 0x87, 0xa4, 0xe0, 0xdb, 0xce, 0x7c, 
        0x1d, 0x2d, 0x04, 0xbd, 0x32, 0x18, 0x9f, 0x14, 0x45, 0xc5, 0x0a, 0x38, 0xcc, 0xc8, 0xa3, 0xe8, 
        0x8d, 0xe6, 0xec, 0x78, 0x8c, 0xbc, 0x86, 0xe3, 0x4f, 0x9d, 0xfe, 0x02, 0xbf, 0x16, 0xaa, 0x94, 
        0x96, 0x3d, 0x74, 0x3b, 0x52, 0xef, 0xb2, 0xa6, 0x19, 0x6b, 0x34, 0x5a, 0xb6, 0x07, 0xaf, 0xea, 
        0x06, 0x30, 0xca, 0x8b, 0x50, 0x65, 0xba, 0xc2, 0x7d, 0x68, 0xeb, 0x29, 0x41, 0x81, 0x7a, 0x80, 
        0x9b, 0x1c, 0x59, 0x79, 0xd1, 0x27, 0x49, 0xf8, 0x2e, 0xe5, 0x25, 0xd7, 0x21, 0xf7, 0x99, 0x3f, 
        0x26, 0x91, 0xe1, 0x2b, 0xc3, 0x44, 0x5f, 0x15, 0x9a, 0x10, 0x6a, 0xa8, 0xff, 0x00, 0x2c, 0xe7, 
        0x0d, 0x35, 0x5b, 0xb3, 0x73, 0xcd, 0x37, 0xfb, 0xb5, 0x95, 0xdf, 0x72, 0xf2, 0x6d, 0x13, 0x2a, 

        0x88, 0x4a, 0x00, 0x39, 0xa9, 0xbe, 0xe6, 0x8e, 0x2b, 0x76, 0x27, 0x69, 0x30, 0xd8, 0x79, 0x7d, 
        0xea, 0x50, 0xb2, 0x16, 0xd0, 0xaf, 0x98, 0x1c, 0x45, 0x97, 0xa7, 0x12, 0x65, 0x3e, 0x5b, 0xc5, 
        0x23, 0x01, 0xcd, 0xeb, 0x1d, 0x0c, 0xcf, 0xba, 0x08, 0x2d, 0xbf, 0x1f, 0x8f, 0xec, 0x63, 0x8c, 
        0xd5, 0x58, 0x51, 0x28, 0x66, 0xa2, 0xdd, 0x04, 0xc7, 0xd2, 0x4d, 0x92, 0xf7, 0xbd, 0x43, 0x6d, 
        0x59, 0xef, 0xb4, 0x93, 0x0b, 0xe9, 0xb7, 0xdf, 0x25, 0xc1, 0x3c, 0xfa, 0x86, 0x75, 0xf0, 0x62, 
        0x54, 0x26, 0xed, 0x29, 0xc4, 0x09, 0xab, 0x14, 0x05, 0x41, 0x5c, 0xf1, 0x32, 0x5e, 0xd3, 0x5d, 
        0xc2, 0xa3, 0x2a, 0x52, 0xcb, 0x7f, 0xac, 0x3b, 0x5a, 0x44, 0x24, 0x03, 0xc3, 0x3a, 0x8d, 0x74, 
        0x07, 0x1b, 0x53, 0x1a, 0x82, 0x13, 0x57, 0x02, 0x7e, 0x9d, 0x1e, 0xd4, 0x83, 0xf8, 0xe8, 0x85, 
        0x78, 0xc8, 0x11, 0xe0, 0x96, 0x48, 0xad, 0xbc, 0x49, 0x6b, 0x22, 0x35, 0xf4, 0xd7, 0x38, 0xb5, 
        0xd9, 0xd6, 0x6f, 0x42, 0x6e, 0xe5, 0xda, 0xb3, 0x61, 0x15, 0xc9, 0x47, 0xfb, 0x3d, 0x77, 0x70, 
        0xbb, 0xa0, 0xf2, 0xca, 0x2f, 0xa5, 0x5f, 0x72, 0x6a, 0xd1, 0xa1, 0x18, 0xf9, 0x7b, 0x31, 0x34, 
        0x4e, 0x19, 0xfe, 0x9a, 0x95, 0xa4, 0x94, 0x46, 0x60, 0x9f, 0x8b, 0x0e, 0x55, 0xaa, 0x10, 0xcc, 
        0x4c, 0x40, 0x71, 0xdb, 0x21, 0x9e, 0xe4, 0x56, 0x0d, 0x81, 0xb1, 0x20, 0x3f, 0x9c, 0xe2, 0xe1, 
        0x8a, 0x89, 0x87, 0xce, 0x0a, 0x99, 0x36, 0x17, 0x37, 0xb9, 0xc6, 0x91, 0x2e, 0xf5, 0x73, 0xf3, 
        0xff, 0x7c, 0x4b, 0x68, 0x84, 0xb8, 0xc0, 0xe3, 0x2c, 0x67, 0xde, 0xdc, 0x80, 0x9b, 0x64, 0x90, 
        0x4f, 0x33, 0xee, 0xa8, 0xfd, 0xa6, 0xf6, 0x06, 0xb0, 0xfc, 0x0f, 0x7a, 0xb6, 0xe7, 0x6c, 0xae, 

        0xaf, 0x26, 0x69, 0x96, 0x1a, 0xc3, 0x97, 0x05, 0x85, 0x41, 0xea, 0x68, 0x36, 0x94, 0x2e, 0x0d, 
        0xaa, 0xa9, 0xb4, 0xe4, 0x9f, 0xde, 0x9e, 0x35, 0x3c, 0x3d, 0xd3, 0x8c, 0x46, 0x86, 0x4e, 0xc5, 
        0xf7, 0xfb, 0x8b, 0x9a, 0x16, 0x81, 0xb2, 0xa4, 0x13, 0xf6, 0x75, 0xd4, 0x39, 0x8f, 0xd8, 0x4f, 
        0x89, 0xc4, 0x72, 0x07, 0xb0, 0xe1, 0xc8, 0x00, 0xe2, 0x65, 0xb1, 0x6e, 0xf5, 0x45, 0xc0, 0x21, 
        0xe3, 0x2d, 0x91, 0xf9, 0xef, 0x2a, 0xb5, 0xe5, 0x7b, 0x7e, 0xd6, 0x64, 0x18, 0xb9, 0x77, 0x93, 
        0x56, 0xee, 0xbc, 0x29, 0x78, 0xd2, 0x66, 0x20, 0xd0, 0x11, 0xe9, 0x09, 0x31, 0x99, 0xd9, 0x4c, 
        0xfc, 0xe6, 0xa6, 0xb3, 0x12, 0x25, 0xe8, 0x08, 0xac, 0x6f, 0xc1, 0xda, 0xa7, 0xd1, 0xa8, 0x24, 
        0x0e, 0x42, 0x54, 0x60, 0x27, 0x30, 0x73, 0x61, 0x2c, 0xf0, 0xa0, 0x2f, 0x48, 0xcc, 0x1d, 0x1e, 
        0x0c, 0xcb, 0xf1, 0x38, 0x1c, 0x28, 0xf8, 0x71, 0x95, 0x0f, 0xd7, 0xeb, 0x57, 0xbf, 0x02, 0x44, 
        0x34, 0x70, 0x4d, 0x3b, 0xad, 0x2b, 0x33, 0x9c, 0x79, 0x80, 0xae, 0x7a, 0x50, 0x47, 0xec, 0x53, 
        0x43, 0xa2, 0x06, 0xe7, 0xce, 0x49, 0xca, 0xdb, 0x52, 0x6d, 0x82, 0x62, 0x4a, 0x23, 0x3a, 0xfd, 
        0xbe, 0xfe, 0x37, 0xbb, 0xa1, 0x4b, 0xfa, 0x19, 0x5f, 0x17, 0x40, 0xc6, 0xa5, 0x7c, 0x51, 0xba, 
        0x83, 0x9b, 0x9d, 0x5a, 0xdf, 0x5c, 0x1f, 0x15, 0x3f, 0xb8, 0x59, 0xff, 0x6a, 0x3e, 0xb6, 0xbd, 
        0x8e, 0xc9, 0x6c, 0xe0, 0x7f, 0xcd, 0x03, 0x67, 0x63, 0xdd, 0x5d, 0x22, 0x14, 0x87, 0xcf, 0xf2, 
        0xc7, 0xd5, 0xdc, 0x7d, 0x0b, 0xb7, 0xf3, 0xab, 0x90, 0x98, 0x1b, 0x04, 0x5e, 0x92, 0x74, 0xa3, 
        0x5b, 0x0a, 0xf4, 0x76, 0x8a, 0x32, 0x10, 0x58, 0x88, 0x8d, 0x6b, 0xed, 0x84, 0xc2, 0x01, 0x55, 

        0x30, 0x57, 0xe5, 0xed, 0x68, 0x59, 0xd5, 0xf7, 0x4a, 0xc9, 0x00, 0xbe, 0x7a, 0xcc, 0x6e, 0x39, 
        0x1c, 0x7d, 0x2e, 0x65, 0x66, 0x42, 0x22, 0x95, 0x60, 0xcd, 0xcf, 0x05, 0x5e, 0xbf, 0x16, 0x0e, 
        0xdb, 0xa7, 0xf8, 0xd3, 0x8e, 0x13, 0x01, 0x50, 0xab, 0xf3, 0x35, 0x72, 0x63, 0x06, 0xde, 0x9e, 
        0x08, 0x40, 0x09, 0xec, 0xbc, 0xf6, 0xa1, 0x55, 0xee, 0x8c, 0x15, 0x97, 0x74, 0xb8, 0x48, 0x81, 
        0x69, 0xd7, 0x4f, 0x1b, 0x0b, 0x1f, 0xa9, 0x9c, 0xf5, 0x71, 0x84, 0x12, 0xb6, 0x1d, 0x3a, 0x36, 
        0x41, 0x32, 0x83, 0x2c, 0x6d, 0x52, 0xf0, 0xae, 0xad, 0x4b, 0x58, 0xe9, 0xf9, 0x17, 0x7e, 0xc1, 
        0x6f, 0x0f, 0xeb, 0x9d, 0x14, 0x91, 0x04, 0xa8, 0x8d, 0xa3, 0x11, 0x96, 0xdc, 0x0a, 0x1a, 0x47, 
        0x4d, 0xb7, 0xa5, 0xc7, 0xef, 0xb9, 0xe2, 0xd0, 0x9f, 0x28, 0x4e, 0x21, 0x87, 0xf2, 0x3e, 0xc2, 
        0x37, 0xc5, 0x9a, 0x88, 0xfa, 0x2d, 0xe6, 0x3f, 0xe0, 0xac, 0x0d, 0x6a, 0x61, 0x51, 0x10, 0xfb, 
        0xe8, 0x31, 0xe7, 0x94, 0x38, 0xdd, 0xa0, 0xd1, 0x80, 0xd2, 0xd4, 0x93, 0x34, 0xba, 0x5d, 0x2a, 
        0x8a, 0x90, 0x76, 0x89, 0x6b, 0xc4, 0xd9, 0xfe, 0x73, 0xb5, 0x78, 0x29, 0xd6, 0x77, 0x1e, 0x5c, 
        0xb1, 0x44, 0xd8, 0xa6, 0x54, 0xe1, 0x46, 0x9b, 0x24, 0xe4, 0xea, 0x6c, 0x8f, 0x07, 0x43, 0x25, 
        0xa4, 0xf4, 0x26, 0x67, 0xff, 0x98, 0x5a, 0x70, 0x02, 0xe3, 0xc6, 0x49, 0x45, 0xc8, 0xc0, 0x2b, 
        0xca, 0x3b, 0x86, 0x5b, 0x18, 0xcb, 0x2f, 0x53, 0xa2, 0xaf, 0x62, 0xbd, 0xaa, 0x5f, 0x7b, 0xb0, 
        0xdf, 0x03, 0x19, 0x92, 0x27, 0x23, 0xbb, 0xb3, 0x75, 0xb2, 0xf1, 0x3d, 0xce, 0xc3, 0x79, 0xfd, 
        0xfc, 0x82, 0x56, 0x7c, 0x0c, 0x8b, 0x3c, 0xb4, 0x85, 0x4c, 0x33, 0x20, 0x99, 0x64, 0x7f, 0xda, 

        0x7c, 0x52, 0xab, 0x7f, 0x6c, 0xd9, 0x30, 0x1d, 0x6b, 0xf3, 0x69, 0x26, 0xd3, 0x37, 0x8b, 0xda, 
        0xe3, 0xec, 0x5e, 0x1f, 0x02, 0x44, 0x0c, 0x13, 0x7e, 0x5c, 0x32, 0x40, 0xaf, 0x49, 0xf4, 0xa8, 
        0xc8, 0x94, 0xb2, 0x25, 0x5a, 0x2b, 0x0d, 0x56, 0xc0, 0x72, 0x03, 0xd0, 0xbe, 0xa1, 0x7d, 0xc4, 
        0xb0, 0x21, 0xfe, 0x8e, 0x51, 0x9c, 0x9f, 0x89, 0x97, 0xde, 0x38, 0x10, 0x3a, 0x3b, 0xdc, 0xfa, 
        0x54, 0x81, 0x0b, 0xee, 0xed, 0x93, 0x3e, 0xac, 0x12, 0xff, 0x2d, 0xf1, 0x5d, 0x33, 0xdd, 0x08, 
        0xd8, 0x4a, 0xdf, 0x2e, 0x66, 0x79, 0x80, 0xf7, 0x6f, 0x95, 0xb9, 0xa6, 0x91, 0x7a, 0x35, 0xd4, 
        0x06, 0xb8, 0xc2, 0x15, 0x00, 0xb3, 0x2a, 0x1b, 0xdb, 0x41, 0xb7, 0x60, 0x17, 0x6a, 0xce, 0x43, 
        0x61, 0x8d, 0x14, 0xaa, 0x05, 0xa5, 0x57, 0xcc, 0xa2, 0x9b, 0x92, 0xbf, 0x84, 0x1c, 0xa4, 0x4b, 
        0x58, 0x1a, 0xc3, 0x68, 0xef, 0xfd, 0x50, 0xe6, 0x5b, 0x7b, 0xe8, 0x8f, 0x70, 0x36, 0x76, 0xa9, 
        0x53, 0x8c, 0x85, 0xc5, 0x67, 0x1e, 0x45, 0xad, 0xe7, 0x87, 0xe9, 0xfc, 0xae, 0x47, 0x3d, 0x59, 
        0x77, 0xcd, 0xe1, 0x28, 0x18, 0xf9, 0x29, 0xf2, 0x34, 0xe0, 0xe2, 0xd7, 0x86, 0x2f, 0x4e, 0x0e, 
        0x20, 0xf5, 0x65, 0xa7, 0x78, 0x63, 0xeb, 0xbd, 0x71, 0x27, 0x11, 0x9a, 0xb1, 0xd1, 0x0f, 0x6e, 
        0x75, 0x16, 0x74, 0xbb, 0xd6, 0x42, 0x48, 0x62, 0x4c, 0x3c, 0x64, 0xb5, 0x83, 0xb6, 0x55, 0xe5, 
        0x96, 0xc7, 0x8a, 0xcf, 0x88, 0x99, 0xc9, 0x3f, 0xca, 0x09, 0x04, 0xb4, 0xfb, 0x98, 0x22, 0x73, 
        0xba, 0x82, 0xd5, 0x19, 0xc6, 0xc1, 0xbc, 0x2c, 0xf6, 0x46, 0x9e, 0x4f, 0x31, 0x07, 0x24, 0x90, 
        0x4d, 0xf8, 0x9d, 0x6d, 0xea, 0xcb, 0xd2, 0xa0, 0x01, 0x23, 0xa3, 0x5f, 0x0a, 0xf0, 0xe4, 0x39, 

        0x0b, 0x61, 0x81, 0x25, 0x8e, 0x3a, 0xcb, 0x88, 0x43, 0xc7, 0x31, 0x69, 0x3e, 0x9b, 0xcc, 0xd9, 
        0x6c, 0x9e, 0x64, 0x78, 0xbb, 0xeb, 0xd2, 0xcd, 0x4c, 0x21, 0xbe, 0x16, 0x51, 0x23, 0xd0, 0x2e, 
        0x75, 0x4d, 0x5e, 0x67, 0xb9, 0xff, 0xcf, 0x2c, 0x9d, 0xfd, 0xd1, 0x66, 0x8f, 0xfe, 0xbf, 0x5a, 
        0x9f, 0x22, 0xb3, 0x85, 0x12, 0x6b, 0xec, 0x3c, 0x1a, 0x26, 0x47, 0x01, 0x13, 0xba, 0xe2, 0x9c, 
        0xef, 0x55, 0x56, 0x7a, 0x54, 0x68, 0x94, 0x49, 0x0d, 0x98, 0x71, 0xda, 0xf1, 0x07, 0x8d, 0xd4, 
        0xfb, 0x92, 0x74, 0x50, 0x1b, 0x5d, 0x53, 0x8a, 0x62, 0x20, 0xae, 0x48, 0xd7, 0xe4, 0x5b, 0x0a, 
        0x2f, 0x18, 0xac, 0x97, 0x3f, 0x34, 0x63, 0x79, 0xa0, 0xb8, 0x46, 0x90, 0x6d, 0x09, 0x35, 0xaa, 
        0x4b, 0xa9, 0x60, 0x4e, 0xfc, 0x4f, 0x57, 0x05, 0xf4, 0xab, 0x58, 0x76, 0xb7, 0x83, 0xb6, 0xc0, 
        0xd5, 0xe6, 0x28, 0xed, 0xf8, 0xb1, 0x00, 0x02, 0xf3, 0x0c, 0x30, 0xbd, 0xa1, 0xa8, 0xd8, 0x2d, 
        0x6a, 0x72, 0x87, 0xa6, 0xa2, 0x36, 0xdc, 0xc6, 0xa4, 0x95, 0xa7, 0xe8, 0xf5, 0x42, 0x06, 0xdd, 
        0x8b, 0x7c, 0xdf, 0x38, 0x84, 0xb0, 0x11, 0x65, 0x73, 0x80, 0x6e, 0x6f, 0x82, 0xa3, 0xb4, 0x7e, 
        0x77, 0x2a, 0xc1, 0xc3, 0x89, 0xe9, 0x17, 0x1e, 0x2b, 0x1d, 0xe5, 0x37, 0x04, 0x10, 0x4a, 0x59, 
        0xf9, 0xd6, 0x41, 0xb2, 0xad, 0x08, 0xe3, 0x45, 0xe1, 0x1f, 0x27, 0xce, 0x29, 0x52, 0x96, 0xbc, 
        0x03, 0xc4, 0x0e, 0x9a, 0xf0, 0xc2, 0x86, 0x24, 0x14, 0xf2, 0x7b, 0x91, 0x33, 0x15, 0xc5, 0xea, 
        0x99, 0xfa, 0xc9, 0x39, 0xa5, 0xdb, 0xf7, 0xaf, 0x3d, 0xde, 0x40, 0x32, 0xe0, 0xd3, 0x5c, 0x93, 
        0x5f, 0x1c, 0x7d, 0x19, 0xee, 0x8c, 0xc8, 0x0f, 0xb5, 0xf6, 0x44, 0xca, 0x7f, 0xe7, 0x3b, 0x70, 

        0xc3, 0x8d, 0xc5, 0x4f, 0x81, 0x48, 0xf2, 0x51, 0x21, 0x67, 0xf7, 0x59, 0xff, 0x38, 0x7f, 0xa5, 
        0xfc, 0x8a, 0xc0, 0x5d, 0x72, 0xc8, 0x78, 0xea, 0x80, 0x24, 0x75, 0x8f, 0x42, 0x7a, 0x4c, 0x19, 
        0x85, 0xec, 0x31, 0x62, 0x13, 0x33, 0x6e, 0x0a, 0xee, 0xa8, 0xc9, 0x1d, 0xd1, 0xd0, 0xeb, 0xe6, 
        0x52, 0xf6, 0xd7, 0xac, 0xd4, 0x89, 0xe4, 0x22, 0x56, 0x20, 0x95, 0x16, 0x45, 0xa3, 0xe9, 0xc4, 
        0xf5, 0x7b, 0x1b, 0x54, 0x83, 0xd9, 0xf3, 0xe3, 0xde, 0xd2, 0x82, 0x9b, 0x0e, 0xb2, 0xb5, 0x08, 
        0x87, 0x63, 0xb9, 0x55, 0x91, 0x36, 0xbe, 0xe5, 0xbb, 0xbd, 0x3e, 0x84, 0x6b, 0x7d, 0x94, 0x76, 
        0x2b, 0x64, 0x06, 0xb6, 0xce, 0xb0, 0xd8, 0xc6, 0x4a, 0x69, 0x0b, 0xf4, 0x43, 0x39, 0xdb, 0x09, 
        0x8c, 0xf0, 0xaa, 0x6a, 0x9d, 0x30, 0x6f, 0x26, 0x32, 0x04, 0x65, 0xe1, 0x07, 0x10, 0x3f, 0xa4, 
        0x98, 0x05, 0x00, 0x34, 0xfb, 0xbc, 0xcb, 0xdd, 0xdc, 0x40, 0xcf, 0xa0, 0x41, 0x74, 0x50, 0x9a, 
        0xba, 0x15, 0xab, 0xda, 0xaf, 0xa7, 0xe0, 0x6d, 0x5e, 0xc7, 0x9c, 0xa2, 0x1c, 0xe8, 0x53, 0xb8, 
        0x02, 0xed, 0x4b, 0x9e, 0x99, 0xcc, 0x12, 0x3c, 0x66, 0x1a, 0x71, 0xa6, 0x6c, 0xd3, 0x79, 0xa9, 
        0xf1, 0x7c, 0x2d, 0x3d, 0x9f, 0x7e, 0xe7, 0x8e, 0xb4, 0x23, 0x01, 0x96, 0x5f, 0xef, 0x28, 0xbf, 
        0xca, 0xc1, 0x90, 0x60, 0x0d, 0x5a, 0x61, 0xb1, 0x47, 0x17, 0x88, 0xf9, 0xcd, 0x8b, 0x57, 0x1e, 
        0x1f, 0x86, 0x49, 0x70, 0xad, 0x2f, 0xd5, 0x0c, 0x2c, 0x2e, 0xb3, 0x46, 0x3a, 0x29, 0x27, 0x73, 
        0x14, 0x58, 0x77, 0x5b, 0x4d, 0x2a, 0x92, 0x11, 0x25, 0x18, 0x03, 0x93, 0xa1, 0xfa, 0x5c, 0xf8, 
        0x68, 0x0f, 0xfe, 0x44, 0xae, 0x35, 0xdf, 0xfd, 0x3b, 0xc2, 0x97, 0xe2, 0x37, 0xd6, 0xb7, 0x4e, 

        0x22, 0x5a, 0x0d, 0xee, 0xa6, 0x5b, 0x6d, 0x52, 0x03, 0x23, 0x21, 0x3d, 0xc2, 0x95, 0x36, 0xfe, 
        0x41, 0x31, 0x1b, 0x24, 0x86, 0xc5, 0x49, 0x01, 0xeb, 0x43, 0x8b, 0xbd, 0x8d, 0xdd, 0x63, 0x6c, 
        0x26, 0xf7, 0x97, 0x60, 0xb0, 0x30, 0x27, 0xac, 0x14, 0xcc, 0xdb, 0x1c, 0xc4, 0x02, 0xf3, 0xfd, 
        0x98, 0x8a, 0x0e, 0x13, 0x68, 0x6e, 0x5d, 0xd7, 0x7b, 0x1f, 0x72, 0x47, 0x4f, 0x3c, 0x88, 0x99, 
        0x4c, 0x69, 0xc3, 0x84, 0xdc, 0x1a, 0x9f, 0x81, 0x06, 0xed, 0xad, 0x07, 0x08, 0x7a, 0x4b, 0x4a, 
        0x6b, 0x0f, 0x4d, 0xea, 0xf1, 0x9a, 0x93, 0x73, 0x92, 0xb5, 0x7d, 0xa1, 0x17, 0x38, 0x5c, 0xe0, 
        0xe6, 0x89, 0x10, 0xf9, 0x82, 0x33, 0x85, 0x8c, 0xf6, 0x80, 0x7f, 0x74, 0xae, 0x9d, 0xcd, 0x11, 
        0x58, 0xfa, 0x3b, 0x90, 0x44, 0x64, 0x53, 0x45, 0x05, 0xa7, 0xd0, 0xb3, 0xb2, 0x04, 0xd8, 0x55, 
        0xb4, 0x3e, 0x0b, 0x34, 0x91, 0x2e, 0xcb, 0x77, 0xf0, 0xdf, 0xbe, 0xd9, 0xa0, 0x78, 0x1d, 0xbc, 
        0x18, 0xec, 0xaa, 0x70, 0xc7, 0x25, 0x9b, 0x4e, 0x87, 0xbb, 0x39, 0x3a, 0x42, 0xff, 0xe1, 0xa9, 
        0x2b, 0xe7, 0xaf, 0x76, 0xc9, 0x75, 0xd2, 0x51, 0x2d, 0xd3, 0x1e, 0xbf, 0xa2, 0xa3, 0x16, 0x0c, 
        0x6a, 0x19, 0xc1, 0x67, 0x83, 0xe4, 0x9e, 0x00, 0x50, 0x8f, 0x57, 0x2a, 0x79, 0x35, 0xfc, 0x8e, 
        0xb6, 0xde, 0xfb, 0x12, 0x0a, 0x96, 0x59, 0xc8, 0x5f, 0xab, 0xe3, 0xa5, 0x54, 0x3f, 0x61, 0x65, 
        0x56, 0xb7, 0x46, 0x6f, 0xc0, 0xca, 0x2c, 0x28, 0xa8, 0x29, 0x2f, 0xcf, 0x40, 0xd5, 0xd4, 0xce, 
        0x94, 0xf2, 0x7e, 0xda, 0x32, 0xef, 0xf4, 0xf8, 0xb1, 0x15, 0xe8, 0x9c, 0xba, 0xe9, 0xb8, 0xa4, 
        0xd6, 0x62, 0xb9, 0xc6, 0xd1, 0xe2, 0xe5, 0x48, 0xf5, 0x20, 0x7c, 0x5e, 0x66, 0x37, 0x09, 0x71, 

        0x14, 0x68, 0xba, 0x92, 0xae, 0x32, 0x0d, 0x80, 0x13, 0x19, 0x56, 0x53, 0xc3, 0x49, 0xc6, 0x4a, 
        0xcf, 0xe5, 0x2b, 0x6a, 0x63, 0xca, 0x85, 0xa8, 0x2d, 0x9c, 0x83, 0x1e, 0xd1, 0x76, 0xb6, 0x6c, 
        0x3b, 0xa4, 0x9f, 0xd4, 0x60, 0x5f, 0xa7, 0x07, 0x75, 0x8c, 0xfd, 0xe0, 0x7b, 0xaf, 0xb9, 0x1a, 
        0xd2, 0xa9, 0xbf, 0xa2, 0x01, 0x0a, 0x78, 0x67, 0x72, 0x7f, 0x04, 0xa0, 0x16, 0x47, 0xf9, 0x69, 
        0xbc, 0x30, 0xf0, 0x39, 0xe8, 0xd0, 0x95, 0x2f, 0x8d, 0xe1, 0xda, 0x4f, 0x9e, 0xf7, 0x33, 0xc5, 
        0x9b, 0xee, 0xa6, 0x74, 0x41, 0xea, 0x10, 0xf3, 0xad, 0x71, 0xcc, 0x93, 0x09, 0xf1, 0xd7, 0xfc, 
        0x44, 0xce, 0x0b, 0xf6, 0x3f, 0x65, 0x7a, 0x38, 0x62, 0x90, 0xc9, 0xb5, 0x81, 0xc4, 0xc1, 0xd9,
        0x24, 0x89, 0x91, 0x57, 0x40, 0xdc, 0x20, 0x51, 0x22, 0xbb, 0xe3, 0x3e, 0x0f, 0x2c, 0x4c, 0xed,
        0x06, 0xfa, 0xab, 0x17, 0xcd, 0xec, 0x84, 0x82, 0x50, 0xfe, 0xb2, 0x54, 0x8a, 0xd6, 0x31, 0x46, 
        0x1b, 0x2e, 0xef, 0x8e, 0x97, 0x5b, 0x21, 0x86, 0x64, 0x27, 0xf8, 0xe7, 0x55, 0x66, 0xde, 0xd8, 
        0xb8, 0x37, 0x26, 0x87, 0x15, 0x2a, 0x02, 0x61, 0x35, 0x9d, 0xd3, 0xd5, 0x1f, 0xb1, 0x9a, 0x18, 
        0x70, 0xb0, 0x99, 0x4d, 0x03, 0x29, 0xe9, 0xc8, 0xf5, 0x42, 0x36, 0x48, 0xdf, 0x88, 0xe4, 0xaa, 
        0xa3, 0x6e, 0x6f, 0x58, 0x6d, 0x3d, 0x00, 0x28, 0x43, 0xc2, 0xb7, 0x7c, 0x3c, 0x59, 0xa5, 0x25, 
        0xc0, 0x7e, 0x11, 0xc7, 0x05, 0x4e, 0x6b, 0x1c, 0x34, 0xf4, 0x5e, 0x1d, 0xac, 0x96, 0x12, 0x94, 
        0x8b, 0xb3, 0xb4, 0x4b, 0xe2, 0xdb, 0xbe, 0x79, 0x5c, 0x98, 0x77, 0x7d, 0xa1, 0xff, 0xe6, 0xdd, 
        0x0c, 0xfb, 0xbd, 0xeb, 0x5a, 0x5d, 0x23, 0x08, 0x0e, 0xcb, 0x52, 0x45, 0x73, 0x8f, 0xf2, 0x3a, 

        0x83, 0x7d, 0x1e, 0xd7, 0x9b, 0xfb, 0x22, 0x6c, 0x00, 0xdc, 0x20, 0x8d, 0x33, 0x24, 0x40, 0x27, 
        0x3d, 0x0f, 0xac, 0xce, 0xb2, 0x58, 0x53, 0x71, 0x5e, 0xb6, 0xdd, 0xfe, 0x9e, 0x26, 0xf7, 0x23, 
        0x0a, 0x08, 0xb4, 0xc6, 0x2b, 0xa6, 0x2f, 0x36, 0xe3, 0xd4, 0xa0, 0x1f, 0x7f, 0xff, 0xd9, 0x63, 
        0xc0, 0xc7, 0xd5, 0xe0, 0x5a, 0x3b, 0x14, 0xf1, 0xf9, 0xd3, 0x72, 0x5d, 0x6f, 0xde, 0x03, 0x65, 
        0xa5, 0x52, 0xf5, 0x3a, 0x6a, 0x4a, 0x94, 0x47, 0x88, 0x9c, 0x46, 0x8c, 0xbf, 0xd1, 0xc8, 0x10, 
        0x85, 0xc4, 0xe1, 0x01, 0xdf, 0xef, 0x8e, 0x98, 0x25, 0x74, 0x49, 0xb5, 0xb0, 0xcf, 0x3e, 0x78, 
        0x37, 0xe4, 0x0d, 0x76, 0x62, 0x61, 0x04, 0xd6, 0xca, 0xc9, 0x99, 0xc2, 0x12, 0x4b, 0x60, 0x1b, 
        0xeb, 0xe5, 0x3f, 0x41, 0x30, 0x09, 0xbc, 0x0e, 0x2d, 0xf6, 0xa9, 0x55, 0x3c, 0x86, 0x73, 0x21, 
        0x0c, 0x81, 0x90, 0x7b, 0x75, 0x66, 0x1a, 0x11, 0x8b, 0x28, 0xfd, 0xaf, 0xb7, 0x38, 0xe7, 0x57, 
        0xb3, 0xbd, 0x4d, 0xee, 0x4c, 0x43, 0x51, 0x6d, 0xc1, 0xda, 0x45, 0x93, 0x80, 0x92, 0xd0, 0x50, 
        0xd2, 0x05, 0xba, 0x39, 0xae, 0x1c, 0x95, 0x6b, 0x84, 0x7c, 0x31, 0xa7, 0x16, 0xb9, 0x48, 0x6e, 
        0xcc, 0xb8, 0xa8, 0xe8, 0xcb, 0xc3, 0x68, 0xfa, 0xbb, 0xdb, 0x67, 0x9d, 0xf8, 0xaa, 0xab, 0x59, 
        0xc5, 0x18, 0xd8, 0x19, 0x4e, 0x42, 0xbe, 0x13, 0x5c, 0x7e, 0x87, 0x0b, 0xa3, 0x15, 0x06, 0xa4, 
        0xed, 0x8a, 0x70, 0x02, 0x91, 0xa2, 0x07, 0x89, 0x35, 0xf0, 0x7a, 0x9a, 0xf3, 0x2a, 0xfc, 0xea, 
        0x5b, 0xf2, 0x77, 0x96, 0xe2, 0x79, 0x64, 0x17, 0x54, 0xe6, 0xf4, 0xec, 0xe9, 0xcd, 0xad, 0x69, 
        0x9f, 0x2e, 0x29, 0x5f, 0x32, 0x44, 0x82, 0x56, 0x4f, 0xb1, 0x8f, 0x2c, 0x97, 0x34, 0xa1, 0x1d, 

        0xc1, 0xc3, 0xd5, 0xa5, 0x06, 0xce, 0x13, 0xeb, 0x71, 0xa8, 0x2a, 0x1b, 0xf3, 0x5c, 0x3a, 0x6b, 
        0x5e, 0x04, 0x50, 0xfd, 0xf1, 0x87, 0x17, 0x57, 0x92, 0xc2, 0x62, 0x28, 0xea, 0xb0, 0x8b, 0x5b, 
        0xb9, 0xa3, 0x0c, 0x4b, 0xb6, 0x11, 0x9d, 0x4a, 0x14, 0xf8, 0x39, 0x5d, 0x9c, 0x7e, 0xb7, 0xf4, 
        0xee, 0x12, 0xac, 0x86, 0x20, 0x44, 0xa2, 0xf7, 0x2e, 0xd4, 0x85, 0xdb, 0x89, 0x73, 0x97, 0x09, 
        0x24, 0x25, 0x27, 0x19, 0x7b, 0x61, 0x1a, 0x8a, 0x52, 0xec, 0x76, 0x9a, 0x66, 0x74, 0xef, 0x58, 
        0xe5, 0x1d, 0xf6, 0x75, 0x8d, 0x79, 0x1e, 0xe6, 0xb4, 0x9f, 0x37, 0xf2, 0xa4, 0xd0, 0xe8, 0xcf, 
        0x8f, 0x2f, 0x54, 0x21, 0xdd, 0x42, 0x8c, 0x6f, 0x5a, 0xff, 0xcc, 0xd9, 0x0d, 0xe1, 0xc0, 0x7d, 
        0x10, 0xe2, 0xa1, 0xca, 0xd7, 0x82, 0x08, 0x63, 0x83, 0x84, 0x9b, 0x16, 0xcd, 0x94, 0xe4, 0x47, 
        0x03, 0xb5, 0x38, 0x90, 0xa0, 0x45, 0xd3, 0x98, 0x43, 0x70, 0xc5, 0x02, 0xa9, 0x40, 0xc8, 0xf0, 
        0x91, 0xd1, 0xf5, 0x0f, 0x32, 0x15, 0x0b, 0x4f, 0x46, 0x22, 0xaa, 0xdf, 0x1c, 0x4c, 0x3b, 0xc6, 
        0x67, 0x36, 0xf9, 0x7a, 0x3f, 0xcb, 0x01, 0x64, 0x5f, 0xaf, 0x34, 0xa6, 0x6a, 0x35, 0xfc, 0x72, 
        0x05, 0xbc, 0xc7, 0x30, 0x8e, 0xe3, 0x4e, 0xbe, 0x2c, 0xfa, 0x99, 0x31, 0x56, 0xa7, 0x88, 0xb1, 
        0xda, 0x29, 0x7c, 0xfb, 0x0e, 0x81, 0x7f, 0xd8, 0xe9, 0xbf, 0x96, 0xb8, 0xd6, 0x07, 0xc4, 0x00, 
        0xab, 0x53, 0x0a, 0xae, 0x59, 0xde, 0x3c, 0x4d, 0xbb, 0x48, 0x93, 0xbd, 0xfe, 0x6d, 0xdc, 0x18, 
        0x69, 0xe7, 0x95, 0x6e, 0xad, 0x41, 0xb2, 0x49, 0xd2, 0xb3, 0x26, 0x6c, 0x68, 0x33, 0x60, 0x9e, 
        0xed, 0x51, 0x78, 0x77, 0x2d, 0x3e, 0x80, 0xba, 0x65, 0x23, 0x2b, 0x3d, 0x55, 0xe0, 0xc9, 0x1f, 

        0x0d, 0x11, 0x99, 0xb8, 0x31, 0xe3, 0x39, 0xd0, 0x3b, 0x02, 0x61, 0xb0, 0x4d, 0x71, 0xda, 0x28, 
        0x40, 0xbe, 0xdc, 0xe2, 0x06, 0xcd, 0x6c, 0xe5, 0xc7, 0xb7, 0xed, 0x07, 0x17, 0xa4, 0x25, 0x34, 
        0x74, 0x97, 0x20, 0x0b, 0xd2, 0xdb, 0x36, 0xce, 0x8a, 0x09, 0x44, 0xd1, 0x37, 0x2c, 0xba, 0xa6, 
        0x1f, 0x33, 0x5b, 0xfd, 0x9f, 0xa1, 0xb3, 0xef, 0x67, 0x8d, 0x22, 0x93, 0x12, 0x1b, 0xc8, 0xe8, 
        0x90, 0xa5, 0x26, 0xb5, 0xa2, 0x4e, 0xf7, 0x0c, 0xf4, 0x60, 0x0f, 0xa3, 0x75, 0xf0, 0x8c, 0x6a, 
        0x2f, 0x98, 0x10, 0x1e, 0xde, 0xf1, 0xf5, 0x86, 0xa7, 0x6d, 0x88, 0x81, 0xfb, 0x76, 0x9a, 0x38, 
        0x3c, 0xc4, 0x95, 0xdf, 0x68, 0xf2, 0x59, 0x78, 0xe0, 0xe9, 0x45, 0x42, 0x2b, 0xb2, 0x2d, 0x05, 
        0xe4, 0xc1, 0x1a, 0xaf, 0xae, 0xfa, 0x19, 0x9d, 0x3f, 0x3e, 0x49, 0x87, 0xc2, 0xf9, 0x2e, 0x55, 
        0x00, 0xf6, 0xca, 0x8b, 0xd4, 0x3d, 0xcb, 0x72, 0xfc, 0xab, 0x1c, 0xd5, 0x1d, 0xaa, 0x32, 0x7d, 
        0x52, 0x7b, 0xe1, 0x4b, 0x92, 0x18, 0xec, 0xc3, 0x13, 0xa0, 0x29, 0x65, 0x4a, 0x51, 0x69, 0x57, 
        0x50, 0xb4, 0x6e, 0x85, 0x9b, 0x2a, 0xa8, 0xbb, 0x47, 0xd6, 0x48, 0xcc, 0x89, 0x73, 0x30, 0x41, 
        0x5c, 0x5f, 0x80, 0xea, 0xb6, 0x7e, 0xc0, 0xac, 0x53, 0x04, 0x62, 0x01, 0x3a, 0x4c, 0xbc, 0x7a, 
        0x5a, 0x14, 0xb1, 0x91, 0xad, 0x6b, 0xc6, 0x35, 0x84, 0x24, 0x77, 0x6f, 0xe6, 0x9c, 0x82, 0xf3, 
        0xd9, 0x21, 0xc9, 0x0e, 0xff, 0x5e, 0xc5, 0x08, 0xd8, 0x23, 0xfe, 0x64, 0x7c, 0x4f, 0x94, 0x16, 
        0xd7, 0x66, 0x79, 0x63, 0xb9, 0x58, 0x03, 0xe7, 0x43, 0x7f, 0xcf, 0x46, 0x70, 0xeb, 0x9e, 0x0a, 
        0xee, 0xbd, 0xd3, 0xdd, 0x15, 0xf8, 0x27, 0x54, 0x56, 0xbf, 0x8f, 0x5d, 0xa9, 0x96, 0x8e, 0x83, 

        0xab, 0x2c, 0x44, 0xc6, 0x4c, 0x1d, 0xcf, 0x95, 0xe4, 0x57, 0xf0, 0xa4, 0xc4, 0xd0, 0xb7, 0x63, 
        0xc5, 0xd4, 0x21, 0x10, 0x55, 0x02, 0x0f, 0x3c, 0x0d, 0x53, 0xaf, 0xcd, 0x88, 0x93, 0x82, 0x86, 
        0xcc, 0xdb, 0xec, 0x39, 0xd6, 0xd9, 0xb9, 0xac, 0x5f, 0xc3, 0x70, 0xe1, 0x9a, 0x24, 0x2d, 0xb3, 
        0x32, 0xd2, 0x22, 0x4e, 0x9b, 0xff, 0xe2, 0xe6, 0x94, 0x60, 0xdf, 0x71, 0xfc, 0x43, 0x12, 0xe8, 
        0x2e, 0xba, 0x37, 0x7c, 0xdc, 0x30, 0x7b, 0xae, 0xcb, 0x1a, 0x41, 0x17, 0xf1, 0x5b, 0xd1, 0x65,
        0x23, 0x83, 0x08, 0x6d, 0x0e, 0x5d, 0x61, 0xd7, 0x18, 0x4b, 0xb8, 0x78, 0x54, 0x8c, 0x8f, 0x76,
        0x00, 0xc2, 0xf5, 0xe7, 0x49, 0x85, 0x09, 0xda, 0x89, 0x19, 0x72, 0x6e, 0xfb, 0xf4, 0xd3, 0x64, 
        0xa8, 0x90, 0xe0, 0xbc, 0x1e, 0x16, 0x6a, 0x36, 0x52, 0xa9, 0xe5, 0x13, 0x2f, 0x4d, 0xd8, 0xf2, 
        0x0a, 0x6c, 0x9c, 0xbf, 0x29, 0x79, 0xa6, 0xbe, 0x4a, 0xf6, 0x87, 0x80, 0x9f, 0xa3, 0x97, 0xb0, 
        0x74, 0xe3, 0xbb, 0xeb, 0x51, 0x2a, 0x75, 0x6b, 0x68, 0x3b, 0xc1, 0x81, 0x4f, 0x5e, 0x66, 0x3d, 
        0x3e, 0xea, 0x58, 0xa2, 0x73, 0x1c, 0x0b, 0xb5, 0x06, 0xfd, 0x34, 0xc8, 0xf9, 0x26, 0x11, 0x5a, 
        0x33, 0x01, 0xe9, 0x48, 0x8b, 0xdd, 0xb2, 0xc0, 0xa0, 0xaa, 0x8d, 0x59, 0x20, 0x9d, 0x46, 0x7d, 
        0x05, 0xb1, 0xa7, 0x15, 0x2b, 0xbd, 0x50, 0x92, 0x47, 0x8e, 0x7e, 0xde, 0x40, 0x45, 0xa5, 0x67, 
        0xc9, 0x6f, 0x5c, 0x14, 0xf3, 0xce, 0x7a, 0x25, 0x99, 0x8a, 0x27, 0x62, 0x0c, 0x07, 0xd5, 0xed, 
        0x3a, 0x31, 0x42, 0x56, 0x28, 0x84, 0xf8, 0x69, 0x1f, 0xee, 0xfe, 0x9e, 0x3f, 0xb6, 0x77, 0x1b, 
        0x38, 0xf7, 0xef, 0xfa, 0x7f, 0xad, 0x04, 0xa1, 0x03, 0x35, 0x98, 0xc7, 0xca, 0x96, 0x91, 0xb4, 

    };

    pearson16_byte_perm = byte_array;

}

// Clayton Bauman 2011

