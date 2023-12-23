#pragma once

#include <list>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

/*
Least-recently used cache, dict-style!

*/
template<typename Key, typename Value>
class SynchronizedLRUCacheMap
{
public:
    SynchronizedLRUCacheMap(size_t max_elems = 10) : _max_elems(max_elems) {};

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

private:
    size_t _max_elems;

    // Key-value map, also points to position in recently-used queue
    std::unordered_map<Key, std::tuple<Value, typename std::list<Key>::iterator>> _map;

    // recently-used queue; push to front, pop from back
    std::list<Key> _ruq;

    // Mutex for synchronization when used as static object
    std::mutex this_mutex;
};


template<typename Key, typename Value>
bool SynchronizedLRUCacheMap<Key, Value>::insert(Key k, Value v)
{
    // Lock this object during mutation for thread-safety
    std::lock_guard<std::mutex> lock(this_mutex);

    // if k,v is a new item
    if (_map.find(k) == _map.end())
    {
        // if we're at capacity, pop the least-recently used
        if (_map.size() == _max_elems)
        {
            Key lru_elem = _ruq.back();
            _ruq.pop_back();
            _map.erase(lru_elem);
        }
        // then do a basic insert
        _ruq.push_front(k);
        _map[k] = std::make_tuple(v, _ruq.begin());
        return true;
    }
    else // else update value
        return false;
}


template<typename Key, typename Value>
bool SynchronizedLRUCacheMap<Key, Value>::update(Key k, Value v)
{
    // Lock this object during mutation for thread-safety
    std::lock_guard<std::mutex> lock(this_mutex);

    if (_map.find(k) != _map.end())
    {
        auto _ruq_pos = std::get<1>(_map[k]);
        _ruq.erase(_ruq_pos);
        _ruq.push_front(k);
        _map.erase(k);
        _map[k] = std::make_tuple(v, _ruq.begin());
        return true;
    }
    else
        return false;
}


template<typename Key, typename Value>
bool SynchronizedLRUCacheMap<Key, Value>::has(Key k)
{
    return _map.find(k) != _map.end();
}


template<typename Key, typename Value>
Value SynchronizedLRUCacheMap<Key, Value>::access(Key k)
{
    // Lock this object during mutation for thread-safety
    std::lock_guard<std::mutex> lock(this_mutex);

    if (_map.find(k) != _map.end())
    {
        auto _ruq_pos = std::get<1>(_map[k]);
        _ruq.erase(_ruq_pos);
        _ruq.push_front(k);
        Value v = std::get<0>(_map[k]);
        _map[k] = std::make_tuple(v, _ruq.begin());
        return std::get<0>(_map[k]);
    }
    else
        throw std::runtime_error("Key error: key not found in map");
}


template<typename Key, typename Value>
void SynchronizedLRUCacheMap<Key, Value>::resize(size_t new_max_elems)
{
    if (new_max_elems < 1)
        throw std::runtime_error("cache size must be positive");

    // Lock this object during mutation for thread-safety
    std::lock_guard<std::mutex> lock(this_mutex);

    _max_elems = new_max_elems;
    while (_map.size() > _max_elems)
    {
        Key lru_elem = _ruq.back();
        _ruq.pop_back();
        _map.erase(lru_elem);
    }
}


template<typename Key, typename Value>
size_t SynchronizedLRUCacheMap<Key, Value>::size() const
{
    return _map.size();
}


template<typename Key, typename Value>
void SynchronizedLRUCacheMap<Key, Value>::printState()
{
    for (Key k : _ruq)
        std::cout << "{" << k << ": " << std::get<0>(_map[k]) << "},  ";
    std::cout << std::endl;
}

template<typename Key, typename Value>
std::list<Key> SynchronizedLRUCacheMap<Key, Value>::cacheKeyState(void) const
{
    return _ruq;
}
