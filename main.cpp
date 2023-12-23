
#include <iostream>
#include <functional>
#include <string>
#include <memory>
#include <thread>

#include "lru.hpp"



/*
Basic {int,string} cache example
*/
void simpleTest()
{
    std::cout << std::endl << "--- simpleTest() ---" << std::endl;
    std::cout << "Initializing cache with max elems = 3" << std::endl;
    SynchronizedLRUCacheMap<int, std::string> cache(3);

    std::cout << "Inserting {1, \"foo\"} ..." << std::endl;
    cache.insert(1, "foo");

    std::cout << "Inserting {2, \"bar\"} ..." << std::endl;
    cache.insert(2, "bar");

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;

    std::cout << "cache.access(1) = " << cache.access(1) << std::endl;

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;

    std::cout << "Inserting {3, \"asdf\"} ..." << std::endl;
    cache.insert(3, "asdf");

    std::cout << "Accessing Key=2 ..." << std::endl;
    cache.access(2);

    std::cout << "Inserting {4, \"qwer\"} ..." << std::endl;
    cache.insert(4, "qwer");

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;

    std::cout << "cache.access(3) = " << cache.access(3) << std::endl;

    std::cout << "Resizing cache to max elems = 1 ..." << std::endl;
    cache.resize(1);

    std::cout << "Updating value keyed by 3 ..." << std::endl;
    cache.update(3, "zxcv");

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;
}


/*
Example for caching custom data structs
*/
class SomeCachedObject
{
    const int id;
    std::vector<double> data;

    friend std::ostream& operator<<(std::ostream& os, const SomeCachedObject& rhs);

public:
    SomeCachedObject(int id) : id(id) {};
    int getId() const {return id;};
    void someMethod() {};
};

std::ostream& operator<<(std::ostream& os, const SomeCachedObject& rhs)
{
    os << "SomeCachedObject<id=" << rhs.id << ">";
    return os;
}


void ptrTest()
{
    std::cout << std::endl << "--- ptrTest() ---" << std::endl;
    std::cout << "Initializing cache with max elems = 3" << std::endl;
    SynchronizedLRUCacheMap<std::string, std::shared_ptr<SomeCachedObject>> cache(3);

    std::function<std::shared_ptr<SomeCachedObject>(int)> make_obj = [](int id){
        return std::make_shared<SomeCachedObject>(id);
    };

    std::cout << "Inserting {\"one\", *SomeCachedObject(1)} ..." << std::endl;
    cache.insert("one", make_obj(1));

    std::cout << "Inserting {\"two\", *SomeCachedObject(10)} ..." << std::endl;
    cache.insert("two", make_obj(10));

    std::cout << "*cache.access(\"one\") = " << *cache.access("one") << std::endl;

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;

    std::cout << "Inserting {\"three\", *SomeCachedObject(11)} ..." << std::endl;
    cache.insert("three", make_obj(11));

    std::cout << "Inserting {\"four\", *SomeCachedObject(100)} ..." << std::endl;
    cache.insert("four", make_obj(11));

    std::cout << "*cache.access(\"one\") = " << *cache.access("one") << std::endl;

    if (cache.has("two"))
        std::cout << "*cache.access(\"two\") = " << *cache.access("two") << std::endl;
    else
        std::cout << "Key \"two\" no longer in cache!" << std::endl;

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;
}



/*
Example cache insert/access over multiple threads
*/
void threadedTest()
{
    std::cout << std::endl << "--- threadedTest() ---" << std::endl;
    std::cout << "Initializing cache with max elems = 2" << std::endl;
    SynchronizedLRUCacheMap<std::string, std::thread::id> cache(2);

    std::thread t1([&cache](){
        std::cout << "Inserting \"t1\" ..." << std::endl;
        cache.insert("t1", std::this_thread::get_id());
        std::this_thread::sleep_for (std::chrono::milliseconds(15));
    });
    std::thread t2([&cache](){
        std::cout << "Inserting \"t2\" ..." << std::endl;
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
        cache.insert("t2", std::this_thread::get_id());
    });
    std::thread t3([&cache](){
        std::cout << "Inserting \"t3\" ..." << std::endl;
        cache.insert("t3", std::this_thread::get_id());
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
    });

    t1.join();
    t2.join();
    t3.join();

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;

    std::thread t4([&cache](){
        cache.access("t3");
    });
    std::thread t5([&cache](){
        cache.access("t2");
    });

    t4.join();
    t5.join();

    std::cout << std::endl << "Current cache state: ";
    cache.printState();
    std::cout << std::endl;
}



int main()
{
    simpleTest();
    ptrTest();
    threadedTest();

    return 0;
}