#pragma once

#include <map>
using namespace std;

/**
 * @brief MMap is a template class that provides Map of Maps container functionality.
 * The following functions are supported:
 * MMap allow to insert and remove key/values pairs like regular map.
 * MMap allow to insert duplications of same keys with different values.
 * MMap allow to iterate all inserted values belongs to a specific key.
 *
 * Map of maps representation is achieved by constructing a outer map which maintains pair
 * of Key/Map elements. Where each inner map maintains pair of Value/Value elements.
 *
 * template<class Key, class Value> limitations:
 * Key must support comparable operators i.e. ==, !=, <, <=, >, >=
 * Value must be a pointer to some type
 *
 * Usage example:
 * typedef MMap<Index, Client*> IndexClientMMap;              // define specific type of MMap
 * typedef MMap<IfIndex, Client*>::Iterator IndexClientMMapI; // define specific type of MMap::Iterator 
 * IndexClientMMap mMmap;                                     // declare MMap instance
 * mMmap.Insert(index, client);                               // insert a pair
 * mMmap.Remove(index, client);                               // remove a pair
 * IndexClientMMapI end = mMmap.End(index);                   // create ends iterator
 * IndexClientMMapI i = mMmap.Begin(index);                   // create begin iterator
 * for(; i != end ; ++i)                                      // iterate values stored for speic key
 *
 */
template<class Key, class Value>
class MMap
{
private:
	// type definition of inner map
	typedef typename pair<Value, Value> InnerMapPair;
	typedef typename map<Value, Value> InnerMap;
	typedef typename map<Value, Value>::iterator InnerMapIterator;

	// type definition of outer map
	typedef typename pair<Key, InnerMap> OuterMapPair;
	typedef typename map<Key, InnerMap> OuterMap;
	typedef typename map<Key, InnerMap>::iterator OuterMapIterator;

public:
	
	/**
	 * MMap Iterator.
	 * Used to iterate MMap collections.
	 * Example:
	 * for(MMap::Iterator i = MMap::Begin(key); i != MMap::End(); ++i;)
	 */
	class Iterator
	{
	public:
		/**
		 * default ctor. Initialized with iterator that points to end()
		 */
		Iterator() : mIterator() {}
		Iterator(const Iterator& other) : mIterator(other.mIterator) {}
		~Iterator(){}

		Iterator& operator=(const Iterator& other) {mIterator = other.mIterator; return *this;}
		Iterator& operator++() {++mIterator; return *this;}
		bool operator!=(const Iterator& other) const {return mIterator != other.mIterator;}
		bool operator==(const Iterator& other) const {return mIterator == other.mIterator;}
		
		/**
		 * TODO: block null reference operations when mIterator==end()
		 */
		Value operator->() const {return mIterator->second;}

	private:
		// MMap initialize this iterator according to its internal representation
		// Used within MMap::Begin() and NNap::End()
		// Shall not be accessed by MMap users
		Iterator(const InnerMapIterator& i) : mIterator(i) {}
		void SetIterator(InnerMapIterator& i) {mIterator = i;}

		InnerMapIterator mIterator;

		friend class MMap<Key,Value>;
	};

	/*
	 * ctor
	 */
	MMap(){} 
	/*
	 * dtor
	 * @brief Removes all exist pairs.
	 * WARNNING: only MMap element's resources are disposed, thus additional resources occupied 
	 * by Key or Value contained elements are not disposed.
	 */
	~MMap();
	/**
	 * @brief Insert a key/val pair
	 * Multiple pairs with different val on same key are allowed to be inserted.
	 * MMap blocks pair insertion if both key and val already exist in the collection.
	 * @param key The index.
	 * @param val The value.
	 * @return Return true in case the insertion succeeded, and false otherwise.
	 */
	bool Insert(Key key, Value val);
	/**
	 * @brief Remove a key val pair
	 * 
	 * @param key The index.
	 * @param val The value.
	 * @return Return true in case the removal succeeded, and false otherwise.
	 */
	bool Remove(Key key, Value val);
	/**
	 * @brief Remove all values related to given key
	 * TODO: re-implement this method
	 */
	bool Remove(Key key) {return true;}
	/**
	 * @brief Create MMap::Iterator that points to the beggining of the collection
	 * @ return MMap::Iterator that points to the start
	 */
	typename MMap::Iterator Begin(Key key);
	/**
	 * @brief Create MMap::Iterator that points to the end of the collection
	 * @return MMap::Iterator that points to the end
	 */
	typename MMap::Iterator End(Key key);

private:

	 /**
	  *
	  */
	 OuterMap mOuterMap;      ///< map of maps container
	 /**
	  * Dummy map used when MMap::Begin and/or MMap::End request innerIter 
	  * for key that is not exist in the outerMap. In this case we need single 
	  * innerMap to retrive same end::iterator.
	  */
	 InnerMap mDummyInnerMap; 
};

/**
 * public destructor
 * Removes all pairs exist in the maps
 */
template<class Key, class Value>
MMap<Key,Value>::~MMap()
{
    // iterate exist inner maps
	for(OuterMapIterator i = mOuterMap.begin(); i != mOuterMap.end(); ++i)
	{
		i->second.clear();// clear inner map from exist elements
	}
	mOuterMap.clear(); // clear outer map from exist elements
}

/**
 * @brief Insert a key/val pair.
 * If key is new than new pair is inserted into the outerMap.
 * If key already exist than val is inserted into the innerMap
 * only if not exist in it.
 * @param key The key index.
 * @param val The value to be inserted.
 * @return Return false if insertion succeeded, and true otherwise.
 */
template<class Key, class Value>
bool MMap<Key,Value>::Insert(Key key, Value val)
{
	bool _isSucceeded=false;                                          // will be set to true only if val will be registerd successfuly
	OuterMapIterator _outerIter = mOuterMap.find(key);                // look for the key

	if(_outerIter == mOuterMap.end() )                                // given key is not register yet
	{
		mOuterMap.insert(OuterMapPair(key, InnerMap() ));             // first create new key entry in the collection
		mOuterMap[key].insert( InnerMapPair(val, val) );	          // and than register the val under that key
		_isSucceeded = true;
	}
	else                                                              // given key is already registered
	{
		InnerMapIterator innerIter = _outerIter->second.find(val);    // the key entry exist, check if given val is already registered for that key
		if( innerIter == _outerIter->second.end() )                   // val is not registerd for that key yet
		{
			_outerIter->second.insert( InnerMapPair(val, val) );      // register it (the val)
			_isSucceeded = true;                                            // true shall be returned to the caller
		}
		else                                                          // given val is already registerd for given key, sorry cannot insert such duplication!
		{
			//cout << "ERROR: Val " << val 
			//	 << " insertion failed. Value already registed for Key " 
			//	 << key << endl;
			_isSucceeded = false;                                      // return false to the caller
		}
	}

return _isSucceeded;
}
/**
 * @brief Remove a key val pair.
 * Key is automatically removed as soon as no val are mapped to it.
 * @param key The key index.
 * @param val The value to be removed.
 * @return Return false if removal succeeded, and true otherwise.
 */
template<class Key, class Value> 
bool MMap<Key,Value>::Remove(Key key, Value val)
{
	bool _isSucceeded = false;                                         // return whether the removal operation succeeded or failed        
	OuterMapIterator _outerIter = mOuterMap.find(key);                 // look for the key and return a outer map iterator

	if(_outerIter != mOuterMap.end() )                                 // if key is found
	{
		InnerMapIterator innerIter = _outerIter->second.find(val);     // look for a val and return a inner map iterator
		if( innerIter != _outerIter->second.end() )                    // val is found for key
		{
			_outerIter->second.erase(val);                             // remove val from the inner map	
			if(_outerIter->second.begin() == _outerIter->second.end()) // if val is the last element in the inner map
			{
				mOuterMap.erase(key);                                  // remove key from the outer map
			}
			_isSucceeded = true;                                       // val removed successfully from collection
		}
		else                                                           // else if val is not found for key
		{
			//cout << "ERROR: Value " << val 
			//	 << " removal failed. Value is not registerd for Key " 
			//	 << key << endl;
			_isSucceeded = false;                                      // val removal from collection was failed
		}
	}
	else                                                               // else key is not found
	{
		//cout << "ERROR: Key " << key 
		//	 << " removal failed. Key is not registerd" << endl;
		_isSucceeded = false;                                          // val removal from collection was failed
	}

return _isSucceeded;
}
/**
 * @brief Provide MMap::Iterator that points to the begining of values collection accorfing to a given key
 * to allow iterating on these values.
 * @param key The key for which we need the iterator for.
 * @return MMap::Iterator object initialized to the begining of the values collection.
 * If key is not exist or no values exist for that key, the MMap::Iterator will point to 
 * the end of a dummyInnerMap. 
 */
template<class Key, class Value> 
typename MMap<Key,Value>::Iterator MMap<Key,Value>::Begin(Key key)
{
MMap<Key,Value>::Iterator i;                                     // default MMap::Iterator

	OuterMapIterator _outerIter = mOuterMap.find(key);           // get key
	if(_outerIter != mOuterMap.end() )                           // if key exist
	{
		InnerMapIterator innerIter = _outerIter->second.begin(); // retrieve its map's iterator
		i.SetIterator(innerIter);                                // init MMap::Iterator with that iterator
	}                                                          
	else                                                         // if key not exist
	{
		i.SetIterator(mDummyInnerMap.end());                     // set MMap::Iterator to point end
	}

return i;                                                        // return initialized MMap::Iterator
}
/**
 * @brief Provide MMap::Iterator that points to the end of values collection accorfing to a given key
 * to allow iteration on values related to that key.
 * @param key The key on for which we need the iterator for.
 * @return MMap::Iterator object initialized to the end of the values collection.
 * If key is not exist or no values exist for that key, the MMap::Iterator will point to 
 * the end of a dummyInnerMap. 
 */
template<class Key, class Value> 
typename MMap<Key,Value>::Iterator MMap<Key,Value>::End(Key key)
{
MMap<Key,Value>::Iterator i;                                    // default non initialized MMap::Iterator

	OuterMapIterator _outerIter = mOuterMap.find(key);          // get key
	if(_outerIter != mOuterMap.end() )                          // if key exist
	{
		InnerMapIterator innerIter = _outerIter->second.end();  // retrieve its map's iterator and set it to end
		MMap<Key,Value>::Iterator tmpI(innerIter);              // init MMap::Iterator with that iterator
		i = tmpI;                                               
	}  
	else                                                        // if key not exist
	{
		i.SetIterator(mDummyInnerMap.end());                    // set MMap::Iterator to point end
	}

return i;
}