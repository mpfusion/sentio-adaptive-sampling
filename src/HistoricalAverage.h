/*
 * HistoricalAverage.h
 *
 *  Created on: 2013-01-29
 *      Author: Marco Patzer
 */

#ifndef HISTORICALAVERAGE_H_NDF27VXU
#define HISTORICALAVERAGE_H_NDF27VXU

#include <cstddef>
#include <algorithm>

/**
 * Class to store the historical average.
 *
 * Templated storage class to hold the historical average.
 *
 * @param The number of elements in the list. Needs to be provided when an
 * object of this class is instantiated.
 *
 * @param The type of storage for the elements, e.g. `int` or `float`.
 */
template <const std::size_t N, typename T> class HistoricalAverage
{
	T  buffer[N]; ///< main storage array
	T* ptr;       ///< pointer used in the `push()` and `pop()` methods

public:

	/**
	 * Constructor
	 *
	 * Initialises all elements with zero.
	 */
	HistoricalAverage() : ptr( buffer )
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


#endif /* end of include guard: HISTORICALAVERAGE_H_NDF27VXU */
