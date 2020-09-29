#pragma once
#include <vector>    
#include <string>
#include <fstream>
#include <iostream>
#include "Entry.h"
using namespace std;
// Entries are stored directly in the cells of the bucket array itself.
template <typename K, typename V, typename W>
class LinearProbeHashing
{
public:
	typedef Entry<K, V, W> Entry;							// a (key,value) pair
	class Iterator;									// iterator/position
public:
	LinearProbeHashing(string filename, int capacity);			// constructor
	int size() const;									// num of entries
	bool empty() const;									// is map empty?
	Iterator find(K k);									// find entry with key k
	Iterator put(K k, V v, W w);							// insert/replace (k,v)
	void erase(K k);									// remove entry with key k
	void erase(Iterator p);								// erase entry at p
	Iterator begin();									// iterator to first entry
	Iterator end();									// iterator to end entry
	void printRecords();								// prints all records
	void printReport();									// prints report of records
	void printPos(Iterator p);							// print the record in given position
	int hash(K k)								// hash function to map values to key
	{
		int i = k % B.size();							// get hash index i
		return i;
	}
protected:
	Entry avail = { -1, -1, "-1" };						// special “available” marker object
	typedef std::vector<Entry> BktArray;					// bucket array
	Iterator finder(K k);								// find utility
	Iterator inserter(Iterator p, Entry e);					// insert utility
	void eraser(Iterator p);								// remove utility
	typedef typename BktArray::iterator BItor;				// bucket iterator

private:
	int n;											// num of entries
	BktArray B;										// bucket array
	bool initProbeCount = false;			// determines if probe count is displayed
	int numProbes;										// number of probes
	int totalProbes = 0;								// total num of probes done
	int maxProbes;							// max num of probes for worst case
	int tmp;										// used to keep track of index to insert entry
public:
	class Iterator {										// iterator (& pos)
	private:
		BItor ent;										// which entry
		const BktArray* ba;									// bucket array
	public:
		Iterator(const BItor e, BktArray& a) : ent(e), ba(&a) {}	// constructor
		Entry& operator*() const;							// get entry
		bool operator==(Iterator p) const;						// are iterator equal?
		bool operator!=(const Iterator& p) const;
		Iterator& operator++();								// advance to next entry
		friend class LinearProbeHashing;						// give class acceess
	};
};

template <typename K, typename V, typename W>
LinearProbeHashing<K, V, W>::LinearProbeHashing(string filename, int capacity) : n(0), B(capacity) // constructor
{
	int code;			// state/county code
	int population;	// state/county populatiom
	string name;		// state/county name
	char comma;
	int ignore;
	fstream p4;     // open file
	p4.open(filename);
	if (!p4)			// display error msg if file not found
	{
		cout << "Error in opening file.\n";
		exit(1);
	}
	p4 >> ignore;
	while (!p4.eof())	// get all records from file
	{
		p4 >> code >> comma >> population >> comma;
		getline(p4, name);
		// make true so it does not display probe count in put()
		initProbeCount = true;
		put(code, population, name);
		// change back to false so later when user inserts new record it displays probe count
		initProbeCount = false;
	}
	p4.close();	// close file
}

template <typename K, typename V, typename W>
int LinearProbeHashing<K, V, W>::size() const { return n; }	// number of entries

template <typename K, typename V, typename W>
bool LinearProbeHashing<K, V, W>::empty() const { return size() == 0; } // is map empty?

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Entry& LinearProbeHashing<K, V, W>::Iterator::operator*() const
{
	return *ent;	// get entry
}

template <typename K, typename V, typename W> // are iterators equal?
bool LinearProbeHashing<K, V, W>::Iterator::operator==(Iterator p) const
{
	if (ba != p.ba) return false;				// ba differ?
	else if (ent == ba->end()) return true;		// both at the end?
	else return (ent == p.ent);				// else use entry to decide
}

template <typename K, typename V, typename W>
bool LinearProbeHashing<K, V, W>::Iterator::operator!=(const Iterator& p) const
{
	if (ba != p.ba) return true;				// ba differ
	else if (ent == ba->end()) return false;	// both at the end?
	else return ent != p.ent;				// else use entry to decide
}

template <typename K, typename V, typename W> // increment to next nonempty bucket
typename LinearProbeHashing<K, V, W>::Iterator& LinearProbeHashing<K, V, W>::Iterator::operator++()
{
	++ent;								// next entry in bucket array
	while (ent->key() == NULL && ent != ba->end() || ent->key() == -1)
	{									// find nonempty bucket
		++ent;
		if (ent == ba->end()) return *this;	// end of array?
	}
	return *this;							// return self
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::end()
{
	return Iterator(B.end(), B);	// iterator to end
}

template<typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::printReport()
{
	// display report for initial records from file
	cout << "Table size: " << B.size() << endl;
	cout << "Average number of probes: " << (float)(totalProbes / n) << endl;
	cout << "Maximum number of probes for the worst case: " << maxProbes << endl;
	//cout << "Total Probes: " << totalProbes << endl;
}

template<typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::printPos(Iterator p)
{
	if (p != end())
		cout << (*p).key() << ", " << (*p).value1() << ", " << (*p).value2() << endl;
}


template <typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::printRecords()	// display all records
{

	for (auto i = B.begin(); i != B.end(); ++i) {
		if ((*i).key() != NULL && (*i).key() != -1)
			cout << (*i).key() << ", " << (*i).value1() << ", " << (*i).value2() << endl;
	}
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::begin()
{
	if (empty()) return end();			// if empty return end
	BItor ent = B.begin();				// else seach for an entry
	while ((*ent).key() == NULL) ++ent;	// find nonempty bucket
	return Iterator(ent, B);				// return first of bucket
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::finder(K k)// find utility
{
	numProbes = 1;	// when finder() is called, it checks at least one location
	int i = hash(k);			// get hash index
	int initIndex = i;			// stores initial index to avoid infinite loop
	tmp = i;					// keep track of index 
	BItor ent = B.begin() + i;	// the ith bucket
	Iterator p(ent, B);			// start the ith bucket
	Iterator temp(ent, B);		// iterator holds position of first bucket with special marker
	int count = 0;				// used to keep track of first bucket with special marker
	bool flag = false;			
	while ((*p).key() != NULL && (*p).key() != k || (*p).key() == -1) { // search for k
		i = (i + 1) % B.size();		// get next index to search
		tmp = i;					// keep track of index to use for inserter or eraser

		if (initIndex == i) return p; // if already went through whole array, exit.

		if ((*p).key() == -1) count++;// if bucket has special marker increment count
		if (count == 1) {			// save position of first bucket with special marker
			temp = p;				// to insert entry if did not find k in array
			flag = true;			// flag if there was encounter w/ special marker during search 
		}

		ent = B.begin() + i;	// the ith bucket
		Iterator v(ent, B);
		p = v;				// start the ith bucket
		numProbes++;			// increment number of probes
	}
	// once reached an empty bucket and did not find k - if it encountered
	// a bucket w/ special marker then return its pos to insert entry
	if (flag && (*p).key() == NULL) return temp; 
	totalProbes += numProbes;				// calc total num of probes
	if (numProbes > maxProbes)				// get max num of probe 
		maxProbes = numProbes;
	return p;					// return final position
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::find(K k)// find key
{
	Iterator p = finder(k);						// look for k
	cout << "Number of probes: " << numProbes << endl;
	if ((*p).key() == NULL && (*p).key() == k) return p; // for existing record with key zero
	if ((*p).key() == NULL || (*p).key() != k) {		// didnt find it?
		cout << "Record not found.\n";
		return end();							// return end iterator
	}
	else
		return p;								// return its position
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::inserter(Iterator p, Entry e)
{
	B.at(tmp) = e;						// insert at p
	n++;								// one more entry
	return Iterator(p.ent, B);			// return this position
}

template <typename K, typename V, typename W>
typename LinearProbeHashing<K, V, W>::Iterator LinearProbeHashing<K, V, W>::put(K k, V v, W w)
{
	Iterator p = finder(k);						// search for k
	// if put() is not called from constructor, display count
	if (!initProbeCount)
		cout << "Number of probes: " << numProbes << endl;
	if ((*p).key() == NULL || (*p).key() == -1)		// k not found?
	{
		// if put() is not called from constructor, display
		if (!initProbeCount) cout << "Record inserted.\n";
		return inserter(p, Entry(k, v, w));		// insert at  bucket
	}
	else
	{										// found it?
		p.ent->setValue1(v);					// replace value with v
		p.ent->setValue2(w);					// replace value with w
		// if put() is not called from constructor, display
		if (!initProbeCount) cout << "Record updated.\n";
		return p;			// return this position
	}
}

template <typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::eraser(Iterator p)
{
	B.at(tmp) = avail;	// replace deleted entry with available marker to keep vector size
	n--;				// one fewer entry
}

template <typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::erase(Iterator p)
{
	eraser(p);	// remove entry at p
}

template <typename K, typename V, typename W>
void LinearProbeHashing<K, V, W>::erase(K k) {	// remove entry with key k
	Iterator p = finder(k);					// find k 
	cout << "Number of probes: " << numProbes << endl;
	if ((*p).key() == NULL && (*p).key() == k) {	// for existing record with key zero
		eraser(p);
		cout << "Record deleted.\n";
		return;
	}
	if ((*p).key() == NULL || (*p).key() != k) {	// not found>?
		cout << "Erase of nonexistent\n";		// error
		return;
	}
	eraser(p);							// remove it
	cout << "Record deleted.\n";
}

