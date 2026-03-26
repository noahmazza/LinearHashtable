// LinearProbeHashTable.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <iomanip>
#include <stdexcept>

// Bucket structure to allow checking for type of empty buckets
// Trying this out because it was shown in the Zybooks, and I wanted to see how it worked
template <typename T>
struct Bucket {
	T value{};  // Value stored in bucket

	Bucket() {}  // Default constructor

	// Constructor to initialize value with a given value
	Bucket(T item) {
		value = item;
	}

	// Static types of empty buckets for comparison of type of empty bucket
	static Bucket EMPTY_SINCE_START;
	static Bucket EMPTY_AFTER_REMOVAL;

	// Returns true if the bucket is empty, and if includeEmptyAfterRemoval is false, only returns true if the bucket has been empty since the start of the program
	bool empty() const {
		return  this == &EMPTY_SINCE_START || this == &EMPTY_AFTER_REMOVAL;
	}

	bool emptySinceStart() const {
		return this == &EMPTY_SINCE_START;
	}

	T& getValue() {
		if (value) {
			return value;  // Return the value stored in the bucket
		}
		throw std::runtime_error("Bucket is empty");  // Throw an error if the bucket is empty
	}

	// Returns true if the values in the buckets are equal
	bool operator==(const Bucket &other) const {
		return value == other.value;
	}

	// Copies value from other bucket
	Bucket& operator=(const Bucket& other) {
		if (this != &other) {
			value = other.value;  // Deep copy the value from the other bucket
		}
		return *this;
	}

};

// Declare the linear hash table so friend ostream operator overloading works
template<typename T>
class LinearHashtable;

template <typename T>
std::ostream& operator<<(std::ostream& out, LinearHashtable<T>& t);	// Overloaded friend function to be defined later

template<typename T>
class LinearHashtable {
public:
	LinearHashtable(int capacity = 499);// Constructor for the Hashtable.  Creates an empty list
	~LinearHashtable(); //Deconstructor for the Hashtable.  Deallocates memory 
	void add(T item);//Adds to the hashtable
	bool remove(T item);//Removes an item from the hashtable.  Returns true if the item was successfully removed
	bool contains(T item);//Determines if the value is in the hashtable
	bool empty();//Determines if the hashtable is empty
	int size();//Determines the number of items in the hashtable
	void clear();//Clears the hashtable of all items, deallocating each item
	bool full(); //Determines if the hash table has reached capacity

	T& operator[] (int ndx); //returns the value at position ndx

	template <typename U>
	friend std::ostream& operator<<(std::ostream& out, LinearHashtable<U>& t);//Returns an output stream for displaying the Hashtable

protected:
	int linearProbeFind(T item, int ind, int startInd = -1) const;// Returns the index of the item in the hashtable, or -1 if not found
	int linearProbeEmpty(int ind, int startInd = -1); // Returns the index of the first empty bucket found, or -1 if not found)
	void fullError() const { throw std::runtime_error("Table is full"); } // Helper function to throw a full table error
	void emptyError() const { throw std::runtime_error("Table is empty"); } // Helper function to throw an empty table error
	int getKey(T item) { return item % capacity; } // Simple hash function to get the key for an item
	Bucket<T>** hashtable;
	int sz;
	int capacity;
};

/* CONSTRUCTORS AND DESTRUCTORS */

template <typename T>
LinearHashtable<T>::LinearHashtable(int capacity) {
	sz = 0;
	this->capacity = capacity;
	hashtable = new Bucket<T>*[capacity];

	// Initialize all buckets to point to the empty bucket since start
	for (int i = 0; i < capacity; ++i) {
		hashtable[i] = &Bucket<T>::EMPTY_SINCE_START;
	}
}

template <typename T>
LinearHashtable<T>::~LinearHashtable() {

	// Deallocate all buckets, then deallocate the hashtable itself
	clear();
	delete[] hashtable;
}

/* MUTATORS */

template <typename T>
void LinearHashtable<T>::add(T item) {
	// Throw an error if the table is full
	fullError();

	int ind = linearProbeEmpty(getKey(item));  // Find the index of the first empty bucket

	hashtable[ind] = new Bucket<T>{ item };  // Create a new bucket with the item and assign it to the index of the first empty bucket
	++sz;  // Increment the size of the hashtable
}

template <typename T>
bool LinearHashtable<T>::remove(T item) {
	// If there's nothing to remove, throw an error
	emptyError();

	int ind = linearProbeFind(item, getKey(item));  // Find the index of the item to be removed

	// If the item was found in the bucket, deallocate the bucket and mark it as empty after removal
	if (ind != -1) {
		delete hashtable[ind];  // Deallocate the bucket at the index of the item to be removed
		hashtable[ind] = &Bucket<T>::EMPTY_AFTER_REMOVAL;  // Mark the bucket as empty after removal
		--sz;  // Decrement the size of the hashtable
		return true;  // Return true to indicate successful removal
	}

	// If the item was not found in the bucket, return false
	return false;
}

template <typename T>
void LinearHashtable<T>::clear() {

	// For each bucket, if it's not empty, deallocate it and point it to the empty bucket
	for (int i = 0; i < capacity; ++i) {
		if (!hashtable[i]->empty()) {  // If the bucket is not empty, deallocate the bucket and mark it as empty since start
			delete hashtable[i];  // Deallocate the bucket at the index of the item to be removed
			hashtable[i] = &Bucket<T>::EMPTY_SINCE_START;  // Mark the bucket as empty since start
		}
	}
}

template<typename T>
bool LinearHashtable<T>::full()
{
	return sz >= capacity;
}

template<typename T>
T& LinearHashtable<T>::operator[](int ndx)
{
	// Throw an error if the index is out of range
	if (ndx < 0 || ndx >= capacity) {
		throw std::out_of_range("Index out of range");
	}

	// Throw an error if the bucket at the index is empty
	if (hashtable[ndx]->empty()) {
		throw std::runtime_error("Bucket is empty");
	}

	// Return the value at the index
	return hashtable[ndx]->getValue();
}

// Recursively searches for the index of an item in the hashtable
template<typename T>
int LinearHashtable<T>::linearProbeFind(T item, int ind, int startInd) const
{
	// If the bucket has already been checked, return -1 to avoid infinite loops
	if(ind == startInd) {
		return -1;
	}

	// Return the index of the item if found, or -1 if not found
	if (item == hashtable[ind]->getValue()) {
		return ind;
	}
	if (hashtable[ind]->emptySinceStart()) {
		return -1;  // Item not found, and we can stop searching since the bucket has been empty since the start of the program
	}

	// Set the starting index for the search if it hasn't been set yet
	if (startInd == -1) {
		startInd = ind;  // Set the starting index for the search
	}

	// Increment index and wrap around if necessary
	if (ind == capacity - 1) {
		ind = 0;  // Wrap around to the beginning of the hashtable
	} else {
		++ind;  // Move to the next bucket
	}

	return linearProbeFind(item, ind, startInd);  // Recursively search the next bucket
}

template<typename T>
int LinearHashtable<T>::linearProbeEmpty(int ind, int startInd)
{
	// If the bucket has already been checked, return -1 to avoid infinite loops
	if (ind == startInd) {
		return -1;  
	}

	// Return the index of the first empty bucket found
	if (hashtable[ind]->empty()) {
		return ind;
	}

	// Set the starting index for the search if it hasn't been set yet
	if (startInd == -1) {
		startInd = ind;  // Set the starting index for the search
	}

	// Increment index and wrap around if necessary
	if (ind == capacity - 1) {
		ind = 0;  // Wrap around to the beginning of the hashtable
	}
	else {
		++ind;  // Move to the next bucket
	}

	return linearProbeEmpty(ind, startInd);  // Recursively search the next bucket
}

/* GETTERS */

template <typename T>
bool LinearHashtable<T>::contains(T item) {
	// Return true if the item is found in the hashtable, and false otherwise
	return linearProbeFind(item, getKey(item)) != -1;
}

template <typename T>
bool LinearHashtable<T>::empty() {
	return sz == 0;
}

template <typename T>
int LinearHashtable<T>::size() {
	return sz;
}

/* FRIEND FUNCTIONS */

template <typename T>
std::ostream& operator<<(std::ostream& out, LinearHashtable<T>& t) {

	// For each bucket, print the index and the value if the bucket is not empty
	for (int i = 0; i < t.capacity; ++i) {

		out << std::setw(8) << i << ": ";

		// If the bucket is not empty, print the value in the bucket
		if (!t.hashtable[i]->empty()) {
			out << t.hashtable[i]->getValue();
		}
		out << std::endl;
	}

	return out;
}

