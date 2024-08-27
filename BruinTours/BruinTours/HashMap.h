//
//  HashMap.h
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#ifndef HashMap_h
#define HashMap_h

#include "base_classes.h"
#include <list>
#include <functional>
#include <string>

using std::list;

template <typename T> class HashMap {
public:
    HashMap(double max_load = 0.75); // constructor
    ~HashMap();       // destructor; deletes all of the items in the hashmap
    int size() const; // return the number of associations in the hashmap
    
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string &key, const T &value);
    
    // Defines the bracket operator for HashMap, so you can use your map like
    // this: your_map["david"] = 2.99; If the key does not exist in the hashmap,
    // this will create a new entry in the hashmap and map it to the default value
    // of type T (0 for builtin types). It returns a reference to the newly
    // created value in the map.
    T &operator[](const std::string &key);
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value within the map.
    const T *find(const std::string &key) const;
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T *find(const std::string &key) {
        const auto &hm = *this;
        return const_cast<T *>(hm.find(key));
    }
    
    HashMap(const HashMap<T> &map) = delete;
    HashMap &operator=(const HashMap<T> &map) = delete;
    
private:
    struct Associate {
        Associate(std::string str, T value) : key(str), val(value) {}
        std::string key;
        T val;
    };
    
    unsigned int hash(const std::string &key) const;
    void createEmptyArray(list<Associate> **&map, int size);
    void rehash();
    
    int m_size;
    double m_maxLoad;
    list<Associate> **m_map;
    int m_buckets;
};

template <typename T>
HashMap<T>::HashMap(double max_load) : m_size(0), m_buckets(10) {
    if (max_load <= 0)
        m_maxLoad = 0.75;
    else
        m_maxLoad = max_load;
    
    createEmptyArray(m_map, m_buckets);
}

template <typename T>
void HashMap<T>::createEmptyArray(list<Associate>**& map, int size) {
    map = new list<Associate> *[size];
    
    for (list<Associate> **i = map; i != map + size; i++)
        (*i) = nullptr;
}

template <typename T> int HashMap<T>::size() const { return m_size; }

template <typename T> unsigned int HashMap<T>::hash(const std::string& hashMe) const {
    return std::hash<std::string>()(hashMe) % m_buckets;
}

template <typename T> const T *HashMap<T>::find(const std::string &key) const {
    list<Associate> *bucket = m_map[hash(key)];
    if (bucket == nullptr)
        return nullptr;
    
    typename list<Associate>::iterator i = bucket->begin();
    for (; i != bucket->end(); i++)
        if (i->key == key)
            return &(i->val);
    
    return nullptr;
};

template <typename T> void HashMap<T>::insert(const std::string &key, const T &value) {
    T *val = find(key);
    if (val != nullptr) {
        *val = value;
        return;
    }
    
    if ((m_size + 1.0) / m_buckets > m_maxLoad) {
        rehash();
    }
    
    list<Associate> *&listAtIndex = m_map[hash(key)];
    if (listAtIndex == nullptr) {
        listAtIndex = new list<Associate>;
    }
    
    m_size++;
    listAtIndex->push_front(Associate(key, value));
}

template <typename T> HashMap<T>::~HashMap() {
    for (list<Associate> **i = m_map; i != m_map + m_buckets; i++) {
        if (*i != nullptr) {
            (*i)->clear();
            delete *i;
        }
    }
    
    delete[] m_map;
};

template <typename T> T& HashMap<T>::operator[](const std::string &key) {
    T *val = find(key);
    if (val == nullptr)
        insert(key, T());
    
    return *find(key);
}

//Associates
template <typename T> void HashMap<T>::rehash() {
    int oldBuckets = m_buckets;
    m_buckets *= 2;
    list<Associate>** newMap;
    createEmptyArray(newMap, m_buckets);
    
    //This loop is still going to be O(oldBuckets + size).
    for (list<Associate> **i = m_map; i != m_map + oldBuckets; i++) {
        if ((*i) != nullptr) {
            typename list<Associate>::iterator item = (*i)->begin();
            while (item != (*i)->end()) {
                list<Associate> *&newMapList =
                newMap[hash(item->key)];
                if (newMapList == nullptr)
                    newMapList = new list<Associate>;
                
                // Splice, like erase(), invalidates iterators after using them. I'm incrementing the old iterator before using it to make sure it stays valid.
                typename list<Associate>::iterator oldItem = item++;
                newMapList->splice(newMapList->begin(), **i, oldItem);
            }
            
            delete *i;
        }
    }
    delete[] m_map;
    m_map = newMap;
}


#endif /* HashMap_h */
