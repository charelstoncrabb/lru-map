# lru-map
Templated synchronized unordered map as a least-recently-used cache 

The data structure interface is defined as follows
```
template<typename Key, typename Value>
class SynchronizedLRUCacheMap
{
public:
    SynchronizedLRUCacheMap(size_t max_elems = 10);

    // main modifier methods
    bool insert(Key, Value);
    bool update(Key, Value);
    bool has(Key);
    Value access(Key);
    void resize(size_t);
    

    // inspection/debugging methods
    size_t size() const;
    std::list<Key> cacheKeyState(void) const;
    void printState();

}
```

Implmementation is contained in `lru.hpp`, a small suite of test/examples is in `main.cpp`.

Possible main program output:
```
--- simpleTest() ---
Inserting {1, "foo"} ...
Inserting {2, "bar"} ...

Current cache state: {2: bar},  {1: foo},  

cache.access(1) = foo

Current cache state: {1: foo},  {2: bar},  

Inserting {3, "asdf"} ...
Accessing Key=2 ...
Inserting {4, "qwer"} ...

Current cache state: {4: qwer},  {2: bar},  {3: asdf},  

cache.access(3) = asdf
Resizing cache to max elems = 1 ...
Updating value keyed by 3 ...

Current cache state: {3: zxcv},  


--- ptrTest() ---
Inserting {"one", *SomeCachedObject(1)} ...
Inserting {"two", *SomeCachedObject(10)} ...
*cache.access("one") = SomeCachedObject<id=1>

Current cache state: {one: 0x600003fc0218},  {two: 0x600003fc01d8},  

Inserting {"three", *SomeCachedObject(11)} ...
Inserting {"four", *SomeCachedObject(100)} ...
*cache.access("one") = SomeCachedObject<id=1>
Key "two" no longer in cache!

Current cache state: {one: 0x600003fc0218},  {four: 0x600003fc0318},  {three: 0x600003fc0298},  


--- threadedTest() ---

Current cache state: {t2: 0x16b43f000},  {t3: 0x16b4cb000},  


Current cache state: {t2: 0x16b43f000},  {t3: 0x16b4cb000},  


```
