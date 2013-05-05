#include "../MMap.h"
#include "gtest/gtest.h"

namespace MMapTesting {

typedef unsigned int IndexType; // IndexType used as a key
class Client                    // Client class used as a value
{
public:
	Client() : mData(0) {}
	Client(int data) : mData(data) {}
	int Get() {return mData;}
private:
	int mData;
};
typedef MMap<IndexType, Client*> IndexClientMapType;                   // define specific key/value MMap
typedef MMap<IndexType, Client*>::Iterator IndexClientMapIteratorType; // defined specific key/value MMap::Iterator
/**
 * @brief Unit test to verifies MMap::Insert operations
 */
TEST(MMap, insertions)
{ 
IndexClientMapType _map;
Client c1, c2;
bool isOk=false;

    // insertion first val for not exist key succeeded
	isOk = _map.Insert(1,&c1);
	EXPECT_EQ(true, isOk);
	// insertion of second val for exist key succeeded
	isOk = _map.Insert(1,&c2);
	EXPECT_EQ(true, isOk);
	// insertion of exist val for exist key failed
	isOk = _map.Insert(1,&c1);
	EXPECT_EQ(false, isOk);
	// insertuib if exist val for not exist key succeeded
	isOk = _map.Insert(2,&c1);
	EXPECT_EQ(true, isOk);
}
/**
 * @brief Unit test to verifies MMap::Remove operations
 */
TEST(MMap, removals)
{
IndexClientMapType _map;
Client c1, c2;
bool isOk=false;

    // removal failed dueto key 1 isn't registered
	isOk = _map.Remove(1,&c1);
	EXPECT_EQ(false, isOk);
	// insertion succeeded
	isOk = _map.Insert(1, &c1);
	EXPECT_EQ(true, isOk);
	// removal failed due to value c2 isn't registered for key 1
	isOk = _map.Remove(1,&c2);
	EXPECT_EQ(false, isOk);
	// removal succeeded
	isOk = _map.Remove(1,&c1);
	EXPECT_EQ(true, isOk);
}
/**
 * @brief Unit test to verifies MMap::Begin & MMap::End operations
 */
TEST(MMap, begin_end)
{
IndexClientMapType _map;
bool isOk=false;
Client c1,c2;

IndexClientMapIteratorType begin;
IndexClientMapIteratorType end;

    // both begin and end are equals when MMap is empty
	begin = _map.Begin(1);
	end = _map.End(1);
	EXPECT_EQ(true,begin == end); // verify begin == end

	// begin and end are not equal when value is exist for given key
	isOk = _map.Insert(1,&c1);
	EXPECT_EQ(true, isOk);// verify insertion OK
	begin = _map.Begin(1);
	end   = _map.End(1);
	EXPECT_EQ(true,begin != end); // verify begin != end

	// begin and end are not equal when more then one value is exist for given key
	isOk = _map.Insert(1,&c2);
	EXPECT_EQ(true, isOk); // verify insertion OK
    begin = _map.Begin(1);
	end = _map.End(1);
	EXPECT_EQ(true,begin != end); // verify begin != end

	// begin and end are equals when key is not exist in the MMap
	begin = _map.Begin(2);
	end = _map.End(2);
	EXPECT_EQ(true, begin == end); // verify begin == end

	// begin and end are equals even when comparing between two not exist keys
	// WARNING: In general it's not allowed to compare between iterators of two map instances.
	//          Such operation triggers exception in map STL code.
	//          In this case although we are requesting iterators for differnt keys, due to the 
	//          fact both keys are not exist in the MMap, we are receiving the same default iterator
	//          for both Begin(2) and End(3) requests.
	//          General coding guideline is always use pair of Begin and End on the same key!
	begin = _map.Begin(2);
	end = _map.End(3);
	EXPECT_EQ(true, begin == end);
}

//
TEST(MMap, iterators)
{
IndexClientMapType _map;
bool _isOk=false;
Client c1(5);
Client c2(23);
IndexClientMapIteratorType _begin;
IndexClientMapIteratorType _end;
IndexClientMapIteratorType _i;
int _result=0;

    // insert one pair and verify Begin != End
	_isOk = _map.Insert(5, &c1);
	EXPECT_EQ(true, _isOk); // verify insertion OK
	_begin = _map.Begin(5);
	_end = _map.End(5);
	EXPECT_EQ(true, _begin != _end); // verify begin != end
	_result = _begin->Get();
	EXPECT_EQ(5,_result); // verify Begin iterator points on the correct Value

	// increament iterator when only one pair exist and verify Begin==End
	_i = _map.Begin(5); // restart iterator
	++_i; // increament iterator
	EXPECT_EQ(true, _i == _map.End(5) ); // verify Begin == End

	// Insert additional Value for same key and ...
	_isOk = _map.Insert(5, &c2); // insert new value for exist key
	EXPECT_EQ(true, _isOk); // verify insertion
	_i = _map.Begin(5); // restart iterator to the beginning
	EXPECT_EQ(true, _i != _map.End(5)); // check iterator points on valid Value
	_result = _i->Get(); // get value
	EXPECT_EQ(23,_result); // verify this is the first value 
	++_i; // increament iterator to the next element
	EXPECT_EQ(true, _i != _map.End(5)); // verify iterator points on valid Value
	_result = _i->Get(); // get value
	EXPECT_EQ(5,_result); // verify this is the second Value
	++_i; // increament iterator
	EXPECT_EQ(true, _i == _map.End(5)); // verify iterator got to the end of the map
}

}; // end of namespace MMapTesting
