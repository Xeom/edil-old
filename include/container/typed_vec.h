#include "vec.h"
#include "err.h"

#ifndef VEC_TYPED_TYPE
# define VEC_TYPED_TYPE void *
# warning "typed_vec with included no VEC_TYPED_TYPE defined"
#endif

#ifndef VEC_TYPED_NAME
# error "typed_vec included with no VEC_TYPED_NAME defined"
#endif

#define VEC_TYPED_VNAME CONCAT(vec_,VEC_TYPED_NAME)
#define VEC_TYPED_FNAME(name) CONCAT(VEC_TYPED_VNAME, _ ## name)

typedef vec VEC_TYPED_VNAME;

static inline VEC_TYPED_VNAME *VEC_TYPED_FNAME(create)(void *mem)
{
    return vec_create(sizeof(VEC_TYPED_TYPE), mem);
}

static inline VEC_TYPED_VNAME *VEC_TYPED_FNAME(init)(void)
{
    return vec_init(sizeof(VEC_TYPED_TYPE));
}

static inline void VEC_TYPED_FNAME(free)(VEC_TYPED_VNAME *v)
{
    vec_free(v);
}

static inline VEC_TYPED_TYPE *VEC_TYPED_FNAME(item)(VEC_TYPED_VNAME *v, size_t index)
{
    return vec_item(v, index);
}

static inline int VEC_TYPED_FNAME(delete)(VEC_TYPED_VNAME *v, size_t index, size_t n)
{
    return vec_delete(v, index, n);
}

static inline int VEC_TYPED_FNAME(insert)(VEC_TYPED_VNAME *v, size_t index, size_t n, VEC_TYPED_TYPE *new)
{
    return vec_insert(v, index, n, new);
}

static inline size_t VEC_TYPED_FNAME(len)(VEC_TYPED_VNAME *v)
{
    return vec_len(v);
}

static inline size_t VEC_TYPED_FNAME(find)(VEC_TYPED_VNAME *v, VEC_TYPED_TYPE *item)
{
    return vec_find(v, item);
}

static inline size_t VEC_TYPED_FNAME(rfind)(VEC_TYPED_VNAME *v, VEC_TYPED_TYPE *item)
{
    return vec_rfind(v, item);
}

static inline VEC_TYPED_VNAME *VEC_TYPED_FNAME(cut)(VEC_TYPED_VNAME *v, size_t index, size_t n)
{
    return vec_cut(v, index, n);
}

#ifdef VEC_TYPED_GETSET
# undef VEC_TYPED_GETSET
  static inline VEC_TYPED_TYPE VEC_TYPED_FNAME(get)(VEC_TYPED_VNAME *v, size_t index)
  {
      VEC_TYPED_TYPE *rtnptr;
      rtnptr = VEC_TYPED_FNAME(item)(v, index);
      return *rtnptr;
  }

  static inline int VEC_TYPED_FNAME(set)(VEC_TYPED_VNAME *v, size_t index, VEC_TYPED_TYPE value)
  {
      return vec_insert(v, index, 1, &value);
  }
#endif

#undef VEC_TYPED_TYPE
#undef VEC_TYPED_NAME
