#ifndef HEADER_ELPA_CONTAINER_ELPA_MAP
#define HEADER_ELPA_CONTAINER_ELPA_MAP

#include "../types.h"
#include "../memory.h"

namespace elpa { namespace container {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class basic_elpa_map
{
protected:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator& _alloc;
    typename Allocator::template auto_root<elem_t> _items;

    basic_elpa_stack( Allocator& alloc )
        : _alloc( alloc ), _items( _alloc ) {}
public:
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class T>
struct elpa_map;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct elpa_mapk<System, SchemeT, AllocatorT, typename SchemeT<System>::elem_t> : basic_elpa_map<System, SchemeT, AllocatorT>
{
    typedef typename SchemeT<System>::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    elpa_map( Allocator& alloc )
        : basic_elpa_map<System, SchemeT, AllocatorT>( alloc ) {}
};

} } //namespace

#endif // HEADER_ELPA_KSTACK
