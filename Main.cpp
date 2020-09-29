/*  Program: Project 1
    Author: Idenia Ayala
    Class: CSCI 230
    Date: 03/16/2020
    Description:

    I certify that the code below is my own work.

    Exception(s): N/A

*/
#include "ChainHashing.h"
#include "DoubleHashing.h"
#include "LinearProbeHashing.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool isPrime(int n);    // returns true if prime, false otherwise
int nextPrime(int p);   // returns nearest prime int greater than table size

int main()
{

    cout << "Author: Idenia Ayala\n";
    int op;	     // holds user option from menu
    int code, pop;  // county/state code and population
    string name;    // county/state name
    string file;    // input data file name
    string scheme;  // hashing scheme
    float load;     // load factor
    int numRecords; // number of records in file
    int tableSize;	// size of hash table

	cout << "Enter file name: ";     // get user input
	cin >> file;
	cout << "Enter hashing scheme (chain, linear, double): ";
	cin >> scheme;
	cout << "Enter load factor: ";
	cin >> load;
     cout << endl;

    fstream data(file, ios::in);        // open file
    data >> numRecords;		          // first value in file will tell you how many records there is
    int n = (int)(numRecords / load);	// calc hash table size
    tableSize = nextPrime(n);           // get nearest prime int for table size

    // initilize schemes
    ChainHashing<int, int, string> a(file, tableSize);
    ChainHashing<int, int, string>::Iterator p1 = a.begin();

    LinearProbeHashing<int, int, string> b(file, tableSize);
    LinearProbeHashing<int, int, string>::Iterator p2 = b.begin();

    DoubleHashing<int, int, string> c(file, tableSize);
    DoubleHashing<int, int, string>::Iterator p3 = c.begin();

    if (scheme == "chain")               // prints scheme report
         a.printReport();
    if (scheme == "linear") 
         b.printReport();
    if (scheme == "double") 
         c.printReport();
                                           
    cout << "\nChoose an option from the menu: " << endl;          // display menu
    cout << "1. Search for a record\n";
    cout << "2. Insert a record\n";
    cout << "3. Delete a record\n";
    cout << "4. List all record\n";
    cout << "5. Exit\n";
	do
	{
          cout << "\nEnter an option: ";                             // get user menu selection
	     cin >> op;
          cout << endl;
		switch (op)
		{
		case 1: cout << "Enter the county/state code to search: "; // search for state code
            cin >> code; 
            if (scheme == "chain") {
                 p1 = a.find(code);
                 a.printPos(p1);
            }
            if (scheme == "linear") {
                 p2 = b.find(code);
                 b.printPos(p2);
            }
            if (scheme == "double") {
                 p3 = c.find(code);
                 c.printPos(p3);
            }
            break;
		case 2: cout << "Enter the county/state code, population, and county/state name to insert: ";
            cin >> code >> pop; getline(cin, name);                  // insert state info
            if (scheme == "chain")             
                 a.put(code, pop, name);
            if (scheme == "linear")
                 b.put(code, pop, name);
            if (scheme == "double")
                 c.put(code, pop, name);
            break;
		case 3: cout << "Enter the county/state code to delete: "; // delete state
            cin >> code;
            if (scheme == "chain")
                 a.erase(code);
            if (scheme == "linear")
                 b.erase(code);
            if (scheme == "double")
                 c.erase(code);
            break;
          case 4: if (scheme == "chain")                               // display all records
                     for (p1 = a.begin(); p1 != a.end(); ++p1)
                          cout << (*p1).key() << ", " << (*p1).value1() << ", " << (*p1).value2() << endl;
                  if (scheme == "linear")
                    b.printRecords();
                  if (scheme == "double")
                    c.printRecords();                     
            break;
		default: if (op != 5) cout << "Invalid option. Enter an optiont between 1 and 5.\n";
            break;                                                   // display error mssg if option invalid
		}
	} while (op != 5);                                              // exit menu if user enters 5
    
     cout << "***Final results*** " << endl;
     cout << "Hashing scheme: " << scheme << endl;
     cout << "Load factor: " << load << endl;
     cout << "Input file: " << file << endl;
     if (scheme == "chain")
          a.printReport();
     if (scheme == "linear")
          b.printReport();
     if (scheme == "double")
          c.printReport();                                             // print report

    cout << "\nThanks for using my program!\n";                      // display exit mssg and quit

    return 0;  
}

int nextPrime(int p)
{
    if (p <= 1)                // base case
        return 2;
    bool found = false;
    while (!found) 
    {
        p++;
        if (isPrime(p))       // if prime num found make true
            found = true;
    }
    return p;                  // return prime num
}

bool isPrime(int n)           // find prime number
{
    if (n <= 1) 
        return false;
    if (n <= 3) 
        return true;
    if (n % 2 == 0 || n % 3 == 0) 
        return false;
    for (int i = 5; i * i <= n; i = i + 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}