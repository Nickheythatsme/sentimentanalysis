/*
6/28/18 Nicholas Grout
This class manages a templated btree, where items are stored based on their
key. Each key must implement the relational operators in order to work with 
this class.
*/
#define DEBUG
#ifndef SENTIMENT_ANALYSIS_BTREE
#define SENTIMENT_ANALYSIS_BTREE

// Used to define the number of data points in a node. Children are always data points + 1
#define BSIZE 5

#include <utility>
#include <memory>
#include <algorithm>
#include <iterator>

#ifdef DEBUG
#include <iostream>
#endif

// Used to store data, with a key and a data value
template<class K, class D>
using key_data = std::pair<K, D>;

// Abbreviation of compare function. This is used to sort data in the array
template<class K>
using Compare = bool(*)(const K &lhs, const K &rhs);

// Default compare function
template<class K>
bool default_compare(const K &lhs, const K &rhs)
{
	return lhs < rhs;
}

// Forward declaration
template<class K, class D>
class holder;

// split_holder is used to split the holder. We need to create a new node, as well as
// push up data to our parent node/the tree class. This struct is returned by the 
// holder's split function.
template<class K, class D>
struct split_holder 
{
	std::unique_ptr<key_data<K,D>> push_up;
	std::unique_ptr<holder<K,D>> new_rhs;
};

template<class K, class D>
class holder
{
public:
	holder();
	explicit holder(key_data<K,D> rhs);
	holder(holder &&rhs);
	holder(const holder &rhs);
	virtual ~holder() = default;

	// Compare to_compare to they keys in our holder.
	// Returns the index of the first data which is greater than to_compare
	size_t first_greater(const K &to_compare);

	// Push data into the holder. 
	// Returns TRUE if success, FALSE if we're full and it's not added.
	bool push(key_data<K,D> rhs);

	// Split the node 
    // Returns a split_holder with a 
	split_holder<K,D> split(key_data<K,D> new_data);

	// Return TRUE if we're full. FALSE if we're not.
	bool full() const;

#ifndef DEBUG
protected:
#endif
    size_t exists(const K &to_find) const;
	static Compare<K> compare;

    static void sort(key_data<K,D>* to_sort, size_t len);
	static bool is_sorted(const key_data<K,D>* to_check, size_t len);
#ifndef DEBUG
private:
#endif
	std::unique_ptr<key_data<K,D>[]> data;
	size_t data_count;
};

template<class K, class D>
Compare<K> holder<K,D>::compare = default_compare<K>;

// CONSTRUCTOR
template<class K, class D>
inline holder<K, D>::holder() : 
	data_count(0),
	data(new key_data<K,D>[BSIZE])
{
}

// CONSTRUCTOR with arguments
template<class K, class D>
inline holder<K, D>::holder(key_data<K,D> rhs) :
	data_count(1),
	data(new key_data<K,D>[BSIZE])
{
	data[0] = std::move(rhs);
}

template<class K, class D>
inline holder<K, D>::holder(holder && rhs) :
	data(std::move(rhs.data)),
	data_count(rhs.data_count)
{
	rhs.data = nullptr;
	rhs.data_count = 0;
}

template<class K, class D>
inline holder<K, D>::holder(const holder &rhs) :
	data(),
	data_count(new key_data<K,D>[BSIZE])
{
	data = new key_data<K,D>[BSIZE];
	for (size_t i = 0; i < data_count; ++i)
	{
		data[i] = rhs.data[i];
	}
}

// Compare to_compare to they keys in our holder.
// Returns the index of the first data which is greater than to_compare
template<class K, class D>
inline size_t holder<K, D>::first_greater(const K & to_compare)
{
	size_t i;
	for (i = 0; i < data_count; ++i)
	{
		if (!compare(data[i].first, to_compare)) // same as: to_compare <= data[i].first
			return i;
	}

	return i;
}

// Push data into the holder. Data_count should also increase.
template<class K, class D>
inline bool holder<K, D>::push(key_data<K,D> rhs)
{
	if (data_count == BSIZE) return false;
	data[data_count++] = std::move(rhs);
    if (!is_sorted(data.get(), data_count))
        sort(data.get(), data_count);
	return true;
}

// Split the holder. 
template<class K, class D>
split_holder<K,D> holder<K, D>::split(key_data<K,D> new_data)
{
	// Array of size BSIZE+1 to hold our data + 1 more data
	std::unique_ptr<key_data<K, D>[]> to_sort{ new key_data<K,D>[BSIZE + 1] }; 

	// Struct to hold new split data (the new rhs holder and push up data)
	split_holder<K,D> split_dest; 
	size_t i;

	for (i = 0; i < BSIZE; ++i)
	{
		to_sort[i] = std::move(data[i]);
	}
	to_sort[i] = std::move(new_data);

	// TODO implement std::sort from <algorithm>
    sort(to_sort.get(), BSIZE+1);

	// TODO test logic here...
	split_dest.push_up.reset(new key_data<K, D>(std::move(to_sort[BSIZE / 2])));
	split_dest.new_rhs.reset(new holder<K, D>());
	for (i = BSIZE / 2 + 1; i < BSIZE + 1; ++i)
	{
		split_dest.new_rhs->push(std::move(to_sort[i]));
	}
	this->data.reset(new key_data<K, D>[BSIZE]);
    this->data_count = 0;
	for (i = 0; i < BSIZE / 2; ++i)
	{
		this->push(std::move(to_sort[i]));
	}
#ifdef DEBUG
    std::cout << std::endl << "self: ";
	for (i=0; i < this->data_count; ++i)
    {
        std::cout << this->data[i].first << ", ";
    }

    std::cout << std::endl << 
        "Push up data: " << split_dest.push_up->first << std::endl;

    std::cout << "rhs: ";
	for (i=0; i < split_dest.new_rhs->data_count; ++i)
	{
		std::cout << split_dest.new_rhs->data[i].first << ", ";
	}
    std::cout << std::endl;
    
#endif
	return split_dest;
}

template<class K, class D>
inline bool holder<K, D>::full() const
{
	return data_count != BSIZE;
}

template<class K, class D>
inline bool holder<K, D>::is_sorted(const key_data<K,D>* data, size_t len)
{
	for (size_t i = 0; i < len-1; ++i)
	{
        if (!compare(data[i].first, data[i + 1].first))
            return false;
	}
	return true;
}

// TODO implement quicksort? This bubble sort is lame.
template<class K, class D>
void holder<K,D>::sort(key_data<K,D>* data, size_t len)
{
	for (size_t i = 0; i < len - 1; ++i)
    {
        if (!compare(data[i].first, data[i + 1].first))
        {
            std::swap(data[i], data[i+1]);
        }
    }
    if (!is_sorted(data, len))
    {
        sort(data, len);
    }
}

/* NODE Implementation */
template<class K, class D>
class node : public holder<K,D>
{
    public:
        node();
        node(const node<K,D> &rhs);
        node(node<K,D> &&rhs);
        ~node();
        // Returns pointer to the new root
        node<K,D>* insert(key_data<K,D> new_data);
        key_data<K,D>& find(const K &to_find);
    protected:
    private:
};

template<class K, class D>
node<K,D>::node() :
    holder<K,D>()
{
}

template<class K, class D>
node<K,D>::node(const node<K,D> &rhs)
{
}

template<class K, class D>
node<K,D>::node(node<K,D> &&rhs)
{
}

template<class K, class D>
node<K,D>::~node()
{
}

// Insert, returns pointer to the new root
template<class K, class D>
node<K,D>* node<K,D>::insert(key_data<K,D> new_data)
{
}

template<class K, class D>
key_data<K,D>& node<K,D>::find(const K &to_find)
{
}


#endif // SENTIMENT_ANALYSIS_BTREE
