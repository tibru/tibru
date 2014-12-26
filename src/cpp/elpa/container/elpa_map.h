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
	
	elem_t _at( K key ) const
	{
		return *_items.at(key);
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
	
	elem_t at( const K& key ) const
	{
		return this->_at( key );
	}
	
	typedef std::pair<K,elem_t> value_type;
	
	class const_iterator
	{
		typedef typename std::map<K,elem_t*>::const_iterator CIter;
		
		CIter _i;
	public:
		const_iterator( CIter i )
			: _i( i ) {}
			
		const_iterator operator++() { ++_i; return *this; }
		value_type operator*() const { return value_type( _i->first, *_i->second ); }
		
		bool operator!=( const const_iterator& i ) const { return _i != i._i; }
	};
	
	const_iterator begin() const { return this->_items.begin(); }
	const_iterator end() const { return this->_items.end(); }
};

} } //namespace

#endif // HEADER_ELPA_KSTACK