#pragma once
template <typename K, typename V, typename W>
class Entry {						// a (key, value) pair
public:						// public functions
    Entry(const K& k = K(), const V& v = V(), const W& w = W())		// constructor
        : _key(k), _value1(v), _value2(w) { }
    const K& key() const { return _key; }		// get key
    const V& value1() const { return _value1; }		// get value
    const W& value2() const { return _value2; }
    void setKey(const K& k) { _key = k; }		// set key
    void setValue1(const V& v) { _value1 = v; }		// set value
    void setValue2(const W& w) { _value2 = w; }
private:						// private data
    K _key;						// key
    V _value1;						// value
    W _value2;
}; 
