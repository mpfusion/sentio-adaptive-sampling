/*
 * CircularBuffer.h
 *
 *  Created on: 2012-10-30
 *      Author: Marco Patzer
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <iterator>

/* // Example
 *
 * #include <iostream>
 * #include "CircularBuffer.h"
 * 
 * int main( int, char** )
 * {
 * 	CircularBuffer<3, float> cb;
 * 
 * 	cb.push_back( 41 );
 * 	cb.push_back( 42 );
 * 	cb.push_back( 43 );
 * 
 * 	CircularBuffer<3, float>::const_iterator i = cb.begin();
 * 
 * 	for ( int j = 0; j < cb.size() ; ++j, ++i )
 * 	{
 * 		std::cout << *i << std::endl;
 * 	}
 * }
 *
 * Unset elements are intialized to zero.
 *
 */

// Implement a templated fixed-size circular buffer, similar to boost::circular_buffer.
template<const std::size_t N, typename T> class CircularBuffer
{
public:

	typedef T              value_type;
	typedef T*             pointer;
	typedef T&             reference;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;

	friend class iterator_type;

	class iterator_type : public std::iterator < std::random_access_iterator_tag,
		typename CircularBuffer<N, T>::value_type >
	{
	public:

		typedef          CircularBuffer<N, T>                  buffer_type;
		typedef typename CircularBuffer<N, T>::value_type      value_type;
		typedef typename CircularBuffer<N, T>::pointer         pointer;
		typedef typename CircularBuffer<N, T>::reference       reference;
		typedef typename CircularBuffer<N, T>::size_type       size_type;
		typedef typename CircularBuffer<N, T>::difference_type difference_type;

	private:

		CircularBuffer<N, T>* p_buff;
		mutable pointer       p_data;

	public:

		iterator_type( buffer_type* pb = 0u,
		               pointer      pd = 0u ) : p_buff( pb ),
		               p_data( pd ) { }

		value_type& operator*()
		{
			return *p_data;
		}
		const value_type& operator*() const
		{
			return *p_data;
		}

		iterator_type& operator++( void )
		{
			if ( ++p_data >= p_buff->buffer + N )
			{
				p_data = p_buff->buffer;
			}
			return *this;
		}

		const iterator_type& operator++( void ) const
		{
			if ( ++p_data >= p_buff->buffer + N )
			{
				p_data = p_buff->buffer;
			}
			return *this;
		}

		iterator_type& operator--( void )
		{
			return *this;
		}
	};

	typedef iterator_type iterator;
	typedef const iterator_type const_iterator;

private:

	value_type buffer[N];
	pointer    in_ptr;
	pointer    out_ptr;

public:

	CircularBuffer()  : in_ptr( buffer ),
		out_ptr( buffer )
	{
		std::fill( buffer, buffer + N, T( 0 ) );
	}

	CircularBuffer( const std::size_t n, const T& t ) : in_ptr( buffer ),
		out_ptr( buffer )
	{
		for ( size_type i = 0u; ( ( i < N ) && ( i < n ) ); i++ )
		{
			buffer[i] = t;
			if ( ++in_ptr >= ( buffer + N ) )
			{
				in_ptr = buffer;
			}
		}
	}

	size_type size( void ) const
	{
		return N;
	}
	bool      empty( void ) const
	{
		return false;
	}

	iterator begin( void )
	{
		return iterator( this, out_ptr );
	}
	iterator end( void )
	{
		return iterator( this, in_ptr );
	}

	void push_back( const T& t )
	{
		*in_ptr = t;
		if ( ++in_ptr >= ( buffer + N ) )
		{
			in_ptr = buffer;
		}
	}

	T pop_back( void )
	{
		const T t = *out_ptr;
		if ( ++out_ptr >= ( buffer + N ) )
		{
			out_ptr = buffer;
		}
		return t;
	}
};


#endif /* CIRCULARBUFFER_H_ */
