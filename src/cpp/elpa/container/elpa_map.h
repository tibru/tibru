#ifndef HEADER_ELPA_CONTAINER_ELPA_MAP
#define HEADER_ELPA_CONTAINER_ELPA_MAP

#include "../types.h"
#include "../memory.h"
#include <map>

namespace elpa { namespace container {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class K>
class basic_elpa_map
{
protected:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator& _alloc;
	std::map<K,elem_t*> _items;
	
	elem_t& _get( K key )
	{
		auto i = _items.find( key );
		if( i == _items.end() )
		{
			i = _items.insert( std::make_pair( key, new elem_t ) ).first;
			_alloc.add_root( i->second );
		}
		
		return *i->second;
	}
	
    basic_elpa_map( Allocator& alloc )
        : _alloc( alloc ) {}
public:	
	~basic_elpa_map()
	{
		for( auto i : _items )
		{
			_alloc.del_root( i.second );
			delete i.second;
		}
	}
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class K, class V>
struct elpa_map;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class K>
struct elpa_map<System, SchemeT, AllocatorT, K, typename SchemeT<System>::elem_t> : basic_elpa_map<System, SchemeT, AllocatorT, K>
{
    typedef typename SchemeT<System>::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;
	
    elpa_map( Allocator& alloc )
        : basic_elpa_map<System, SchemeT, AllocatorT, K>( alloc ) {}

	elem_t& operator[]( const K& key )
	{
		return this->_get( key );
	}
};

} } //namespace

#endif // HEADER_ELPA_KSTACK