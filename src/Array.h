/*
 * Array.h
 *
 *  Created on: 2013-07-24
 *      Author: Marco Patzer
 */

#ifndef ARRAY_H_DAHB3GQW
#define ARRAY_H_DAHB3GQW

#include <cstddef>    ///< for `size_t`
#include <algorithm>  ///< for `fill`

/**
 * A container for storing a fixed size sequence of elements.
 *
 * A simplified version of the C++11 array container class.
 *
 * @param The type of storage for the elements, e.g. `int` or `float`.
 *
 * @param The number of elements in the list. Needs to be provided when an
 * object of this class is instantiated and is fixed during the life time of
 * the object.
 */
template <typename T, const std::size_t N> class Array
{
	T  buffer[N]; ///< main storage array
	T* ptr;       ///< pointer used in the `push()` and `pop()` methods

public:

	typedef       T* pointer;
	typedef const T* const_pointer;
	typedef       T& reference;
	typedef const T& const_reference;
	typedef       T* iterator;
	typedef const T* const_iterator;

	iterator begin()
	{
		return iterator( data() );
	}

	const_iterator begin() const
	{
		return const_iterator( data() );
	}

	iterator end()
	{
		return iterator( data() + N );
	}

	const_iterator end() const
	{
		return const_iterator( data() + N );
	}

	pointer data()
	{
		return buffer;
	}

	const_pointer data() const
	{
		return buffer;
	}

	bool empty() const
	{
		return size() == 0;
	}

	reference front()
	{
		return *begin();
	}

	const_reference front() const
	{
		return *begin();
	}

	reference back()
	{
		return N ? *( end() - 1 ) : *end();
	}

	const_reference back() const
	{
		return N ? *( end() - 1 ) : *end();
	}

	reference
	operator[]( const std::size_t _N )
	{
		return buffer[_N];
	}

	const_reference
	operator[]( const std::size_t _N ) const
	{
		return buffer[_N];
	}

	/**
	 * Constructor
	 *
	 * Initialises all elements with zero.
	 */
	Array() : ptr( buffer )
	{
		std::fill( buffer, buffer + N, T( 0 ) );
	}

	/**
	 * Initialises with a default value.
	 *
	 * All elements of the array are initialised with the value provided.
	 *
	 * @param The value used for initialisation.
	 */
	void fill( const T& t )
	{
		std::fill( buffer, buffer + N, T( t ) );
	}

	/**
	 * The size of the array.
	 *
	 * The returned value is not the number of elements which have already
	 * been stored, but the *maximum* value which was used when the class was
	 * instantiated.
	 *
	 * @return Maximum amount of elements that can be stored.
	 */
	std::size_t size() const
	{
		return N;
	}

	/**
	 * Stores a value in the array.
	 *
	 * If stored value overwrites the oldest value.
	 */
	void push( const T& t )
	{
		*ptr = t;
		if ( ++ptr >= ( buffer + N ) )
			ptr = buffer;
	}

	/**
	 * Retrieves the last value in the array.
	 *
	 * If this method is called consecutively, it always retrieves the last
	 * value, regardless how often it is called. This means if less values are
	 * stored than the maximum capacity, a zero is returned.
	 *
	 * @return The last value from the array.
	 */
	const T pop() const
	{
		if ( ptr - 1 < buffer )
			return *( ptr + N - 1 );
		else
			return *( ptr - 1 );
	}

	/**
	 * Computes the sum of all values in the array.
	 *
	 * If less values are stored than the capacity, the returned value will be
	 * wrong since the sum includes the zeros that are used to initialise the
	 * array.
	 *
	 * @return The sum of all values in the array.
	 */
	const T sum() const
	{
		T t = 0;
		for ( std::size_t i = 0; i < N; ++i )
			t += *( buffer + i );
		return t;
	}

	/**
	 * Computes the average over all values in the array.
	 *
	 * If less values are stored than the capacity, the returned value will be
	 * wrong since the size includes the zero padding that is used to
	 * initialise the array.
	 *
	 * @return The average over all values in the array.
	 */
	const T average() const
	{
		return sum() / size();
	}

};

#endif /* end of include guard: ARRAY_H_DAHB3GQW */
