#include "vec.h"

#ifndef VEC_TYPED_TYPE
# define VEC_TYPED_TYPE void *
# warning "typed_vec with included no VEC_TYPED_TYPE defined"
#endif

#ifndef VEC_TYPED_NAME
# error "typed_vec included with no VEC_TYPED_NAME defined"
#endif

#define VEC_TYPED_VNAME vec_ ## VEC_TYPED_NAME;
#define VEC_TYPED_FNAME(name) VEC_TYPED_VNAME ## _ ## name

typedef vec VEC_TYPED_VNAME;

static int VEC_TYPED_FNAME(create)(void *mem)
{
    return vec_create(sizeof(VEC_TYPED_TYPE), mem);
}

static VEC_TYPED_VNAME *VEC_TYPED_FNAME(init)(void)
{
    return vec_init(sizeof(VEC_TYPED_TYPE));
}

static void *VEC_TYPED_FNAME(free)(VEC_TYPED_VNAME *v)
{
    vec_free(v);
}

static VEC_TYPED_TYPE *VEC_TYPED_VNAME ## _item(VEC_TYPED_VNAME *v, size_t index)
{
    return vec_item(v, index);
}

static int VEC_TYPED_FNAME(delete)(VEC_TYPED_VNAME *v, size_t index, size_t n)
{
    return vec_delete(v, index, n);
}

static int VEC_TYPED_FNAME(insert)(VEC_TYPED_VNAME *v, size_t index, size_t n, const VEC_TYPED_TYPE *new)
{
    return vec_insert(v, index, n, new);
}

static size_t VEC_TYPED_FNAME(len)(VEC_TYPED_VNAME *v)
{
    return vec_len(v);
}

static size_t VEC_TYPED_FNAME(find)(VEC_TYPED_VNAME *v, const VEC_TYPED_TYPE *new)
{
    return vec_find(v, item);
}

static size_t VEC_TYPED_FNAME(rfind)(VEC_TYPED_VNAME *v, const VEC_TYPED_TYPE *new)
{
    return vec_rfind(v, item);
}

static VEC_TYPED_VNAME *VEC_TYPED_FNAME(cut)(VEC_TYPED_VNAME *v, size_t index, size_t n)
{
    return vec_cut(v, index, n);
}

static VEC_TYPED_TYPE VEC_TYPED_FNAME(get)(VEC_TYPED_VNAME *v, size_t index)
{
    VEC_TYPED_TYPE *rtnptr;

    TRACE_PTR(rtnptr = VEC_TYPED_FNAME(item)(v, index),
              return 0);

    return *rtnptr;
}

static int VEC_TYPED_FNAME(set)(VEC_TYPED_VNAME *v, size_t index, VEC_TYPED_TYPE value)
{
    return vec_insert(v, index, 1, &value);
}

#undef VEC_TYPED_TYPE
#undef VEC_TYPED_NAME
