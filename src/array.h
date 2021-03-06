// array.h
// 

#ifndef ARRAY_H
#define ARRAY_H

//bvm_cache *cxr(bvm_cache *this_bvm);
bvm_cache *sfield(bvm_cache *this_bvm);
bvm_cache *arlen(bvm_cache *this_bvm);
bvm_cache *islf(bvm_cache *this_bvm);
bvm_cache *isinte(bvm_cache *this_bvm);
bvm_cache *slice(bvm_cache *this_bvm);
bvm_cache *arlen8(bvm_cache *this_bvm);
bvm_cache *cut(bvm_cache *this_bvm);
bvm_cache *newin(bvm_cache *this_bvm);
bvm_cache *newlf(bvm_cache *this_bvm);
bvm_cache *trunc(bvm_cache *this_bvm);
//bvm_cache *cxr(bvm_cache *this_bvm);
bvm_cache *arcat(bvm_cache *this_bvm);
bvm_cache *arcmp(bvm_cache *this_bvm);
bvm_cache *ar2ls(bvm_cache *this_bvm);
bvm_cache *arcat8(bvm_cache *this_bvm);
bvm_cache *slice8(bvm_cache *this_bvm);
mword array8_size(mword size8);
bvm_cache *th(bvm_cache *this_bvm);

//mword *_th(mword *bs, mword entry);
mword *_th(bvm_cache *this_bvm, mword *bs, mword entry);

//XXX DEPRECATED
//bvm_cache *w(bvm_cache *this_bvm);
//bvm_cache *paste(bvm_cache *this_bvm);
//bvm_cache *save(bvm_cache *this_bvm);

mword *_newlf(mword size);
mword *_newin(mword size);
//mword *_newlfz(mword size);
mword *_newlfi(mword size, mword init);
mword *_newtptr(void);

inline mword* val(mword *leaf, mword index);
mword *_newva(mword value);

mword _arlen8(mword *string);
mword  alignment_word8(mword size8);
mword  dec_alignment_word8(mword alignment_word);
void   _trunc(mword *operand, mword new_size);
int    _arcmp(mword *left, mword *right);
mword  _cxr1(mword *val, mword bit);
mword _cxr8(mword *val, mword bit);
//mword *_ar2ls(mword *arr);
mword *_ar2ls(bvm_cache *this_bvm, mword *arr);
//mword *_cxr(mword *array, mword offset);

bvm_cache *perm(bvm_cache *this_bvm);
void _perm(mword *src, mword *dest, mword *perm_matrix);



#endif //ARRAY_H

// Clayton Bauman 2011

