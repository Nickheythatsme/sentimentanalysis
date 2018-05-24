#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "job.h"
using namespace std;

#define ARRAY_SIZE 5

template <typename T>
using varray = vector<vector<T>>;

// Long running function to test the queue
template<class T>
varray<T> matrix_mult(varray<T> src_array)
{
    auto dest_array = src_array;
    for (auto &col : dest_array)
    {
        for (auto &item : col)
            item *= item;
    }
    return dest_array;
}

// Display the 2D array as a square
template<class T>
void display_array(const varray<T> &array)
{
    for (auto &col : array)
    {
        for (auto &item : col)
        {
            cout << setw(3);
            cout << item << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Make the array
template<typename T>
decltype(auto) make_2d_array(size_t size)
{
    varray<T> array;
    for (size_t i=0; i < ARRAY_SIZE; ++i)
    {
        vector<T> col;
        for (size_t j=0; j < ARRAY_SIZE; ++j)
            col.emplace_back(T(i+j));
        array.emplace_back(col);
    }
    return array;
}

template<typename T>
T simple_func(T s)
{
    cout << s << endl;
    return s;
}


int main()
{
    /*
    auto array = make_2d_array<double>(ARRAY_SIZE);
    job<varray<double>, varray<double>> j1(matrix_mult, array));
    display_array(array);
    j1.start();
    j1.set_args(array);
    display_array(j1.get_return_val());
    varray<double> c { std::move(j1.get_return_val()) };
    */

    int x = 4;
    job<int, int> j1(simple_func, int(4));
    j1.set_args(std::move(x));
    j1.set_args(x);

    return 0;
}
