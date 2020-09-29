#pragma once
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Entry.h"
using namespace std;
// Requires the use of a list to hold entries with colliding keys
template <typename K, typename V, typename W>
class ChainHashing
{
public:
	typedef Entry<K, V, W> Entry;							// a (key,value) pair
	class Iterator;									// iterator/position
public:
	ChainHashing(string filename, int capacity);				// constructor
	int size() const;									// num of entries
	bool empty() const;									// is map empty?
	Iterator find(K k);									// find entry with key k
	Iterator put(K k, V v, W w);							// insert/replace (k,v)
	void erase(K k);									// remove entry with key k
	void erase(Iterator p);								// erase entry at p
	Iterator begin();									// iterator to first entry
	Iterator end();									// iterator to end entry
	void printReport();									// prints report of records
	void printPos(Iterator p);							// print the record in given position
	int hash(K k)								// hash function to map values to key
	{
		int i = k % B.size();							// get hash index i
		return i;
	}
protected:
	typedef std::list<Entry> Bucket;						// bucket of entries
	typedef std::vector<Bucket> BktArray;					// bucket array
	Iterator finder(K k);								// find utility
	Iterator inserter(Iterator p, Entry e);					// insert utility
	void eraser(Iterator p);								// remove utility
	typedef typename BktArray::iterator BItor;				// bucket iterator
	typedef typename Bucket::iterator EItor;				// entry iterator
	static void nextEntry(Iterator p) { ++p.ent; }			// entry iterator
	static bool endOfBkt(Iterator p)						// bucket's next entry
	{
		return p.ent == p.bkt->end();						// end of bucket?
	}
private:
	int n;											// num of entries
	BktArray B;										// bucket array
	bool initProbeCount = false;			// determines if probe count is displayed
	int numProbes;										// number of probes
	int totalProbes = 0;
	int maxProbes;										// max num of probes for worst case
public:
	class Iterator {									// iterator (& pos)
	private:
		EItor ent;									// which entry
		BItor bkt;									// which bucket
		const BktArray* ba;								// which bucket array
	public:
		Iterator(BktArray& a, BItor b, EItor q = EItor())
			: ent(q), bkt(b), ba(&a) {}
		Entry& operator*() const;						// get entry
		bool operator==(Iterator p) const;					// are iterator equal?
		bool operator!=(const Iterator& p) const;
		Iterator& operator++();							// advance to next entry
		friend class ChainHashing;						// give class acceess
	};
};

template <typename K, typename V, typename W>
ChainHashing<K, V, W>::ChainHashing(string filename, int capacity) : n(0), B(capacity) // constructor
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
int ChainHashing<K, V, W>::size() const { return n; }	// number of entries

template <typename K, typename V, typename W>
bool ChainHashing<K, V, W>::empty() const { return size() == 0; } // is map empty?

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Entry& ChainHashing<K, V, W>::Iterator::operator*() const
{
	return *ent;	// get entry
}

template <typename K, typename V, typename W>
bool ChainHashing<K, V, W>::Iterator::operator==(Iterator p) const
{
	if (ba != p.ba || bkt != p.bkt) return false;	// ba or bkt differ?
	else if (bkt == ba->end()) return true;			// both at the end?
	else return (ent == p.ent);					// else use entry to decide
}

template <typename K, typename V, typename W>		// are iterators equal?
bool ChainHashing<K, V, W>::Iterator::operator!=(const Iterator& p) const {
	if (ba != p.ba || bkt != p.bkt) return true;		// ba or bkt differ?
	else if (bkt == ba->end()) return false;		// both at the end?
	else return (ent != p.ent);					// else use entry to decide
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator& ChainHashing<K, V, W>::Iterator::operator++()
{
	++ent;									// next entry in bucket
	if (endOfBkt(*this))						// at the end of bucket?
	{	
		++bkt;								// go to next bucket
		while (bkt != ba->end() && bkt->empty())	// find nonempty bucket
			++bkt;
		if (bkt == ba->end()) return *this;		// end of bucket array?
		ent = bkt->begin();						// first nonempty entry
	}
	return *this;								// return self
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::end()
{
	return Iterator(B, B.end());	// iterator to end
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::begin()
{
	if (empty()) return end();			// if empty return end
	BItor bkt = B.begin();				// else seach for an entry
	while (bkt->empty()) ++bkt;			// find nonempty bucket
	return Iterator(B, bkt, bkt->begin());	// return first of bucket
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::finder(K k)// find utility
{
	numProbes = 1;	// when finder() is called, it checks at least one location
	int i = hash(k);				// get hash index
	int count = 0;					// prevents infinite loop in list
	BItor bkt = B.begin() + i;		// the ith bucket
	Iterator p(B, bkt, bkt->begin());	// start the ith bucket

	while (!endOfBkt(p) && (*p).key() != k && bkt->size() > count) {// search for k

		numProbes++;				// increment number of probes
		count++;
		nextEntry(p);				// go to next entry in bucket
	}
	totalProbes += numProbes;			// calc total num of probes
	if (numProbes > maxProbes)			// get max num of probe 
		maxProbes = numProbes;
	return p;						// return final position
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::find(K k)	// find key
{
	Iterator p = finder(k);				// look for k
	cout << "Number of probes: " << numProbes << endl;
	if (endOfBkt(p) || (*p).key() != k) {	// didnt find it?
		cout << "Record not found.\n";
		return end();					// return end iterator
	}
	else
		return p;						// return its position
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::inserter(Iterator p, Entry e)
{
	EItor ins = p.bkt->insert(p.ent, e);		// insert before p
	n++;									// one more entry
	return Iterator(B, p.bkt, ins);			// return this position
}

template <typename K, typename V, typename W>
typename ChainHashing<K, V, W>::Iterator ChainHashing<K, V, W>::put(K k, V v, W w)
{
	Iterator p = finder(k);					// search for k
	// if put() is not called from constructor, display count
	if (!initProbeCount)	
		cout << "Number of probes: " << numProbes << endl;
	if (endOfBkt(p))						// k not found?
	{	
		// if put() is not called from constructor, display
		if (!initProbeCount) cout << "Record inserted.\n"; 
		return inserter(p, Entry(k, v, w));	// insert at end of bucket
	}
	else
	{									// found it?
		p.ent->setValue1(v);				// replace value with v
		p.ent->setValue2(w);				// replace value with w
		// if put() is not called from constructor, display
		if (!initProbeCount) cout << "Record updated.\n";
		return p;							// return this position
	}
}

template <typename K, typename V, typename W>
void ChainHashing<K, V, W>::eraser(Iterator p)
{
	p.bkt->erase(p.ent);	// remove entry from bucket
	n--;					// one fewer entry
}

template <typename K, typename V, typename W>
void ChainHashing<K, V, W>::erase(Iterator p)
{
	eraser(p);	// remove entry at p
}

template <typename K, typename V, typename W>
void ChainHashing<K, V, W>::erase(K k)
{									// remove entry with key k
	Iterator p = finder(k);				// find k 
	cout << "Number of probes: " << numProbes << endl;
	if (endOfBkt(p) || (*p).key() != k) {	// not found>?
		cout << "Erase of nonexistent\n";	// error
		return;
	}
	eraser(p);						// remove it
	cout << "Record deleted.\n";
}

template<typename K, typename V, typename W>
void ChainHashing<K, V, W>::printPos(Iterator p)
{
	if (p != end())
		cout << (*p).key() << ", " << (*p).value1() << ", " << (*p).value2() << endl;
}


template <typename K, typename V, typename W>
void ChainHashing<K, V, W>::printReport()
{
	// display report for initial records from file
	cout << "Table size: " << B.size() << endl;
	cout << "Average number of probes: " << (float)(totalProbes / n) << endl;
	cout << "Maximum number of probes for the worst case: " << maxProbes << endl;
	//cout << "Total Probes: " << totalProbes << endl;
}

