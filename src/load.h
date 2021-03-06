// load.h
// 

#ifndef LOAD_H
#define LOAD_H

bvm_cache *load(bvm_cache *this_bvm);

bvm_cache *unload(bvm_cache *this_bvm);
mword *_unload(mword *bs);
mword _runload(mword *bs, mword *LUT_abs, mword *LUT_rel, mword *dest, mword *offset, mword *LUT_offset);
mword get_rel_offset(mword *LUT_abs, mword *LUT_rel, mword *entry);
//mword *_newtptr(void);

mword *_load(mword *bs, mword size);
void _rload(mword *tree, mword offset);

//void load(void);
////void load_tree(mword *tree, mword offset);
////void _load(mword *tree);//, mword offset);
//void dump(void);
//void dump_tree(mword *tree);
//void arser(void);
//void arser_tree(mword *tree);
////void unload(void);
////void unload_tree(mword base, mword* tree, mword* dest, mword offset);
//void unload(void);
//mword *_unload(mword *tree);
//mword unload_tree(
//        mword *tree, 
//        mword *LUT_abs, 
//        mword *LUT_rel, 
//        mword *dest, 
//        mword *offset,
//        mword *LUT_offset);
//mword get_rel_offset(mword *LUT_abs, mword *LUT_rel, mword *elem);
//

////void _load(mword *tree);
////void load_tree(mword *tree, mword offset);

#endif

// Clayton Bauman 2011

