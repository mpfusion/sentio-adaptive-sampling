#define DEBUG

#include <iostream>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read.hpp>

namespace ba {
	using namespace boost::asio;
}

const unsigned int max_datagram_size = 65536;

// packet structure of the datagram received on the UDP socket
static union {
	struct {
		uint8_t address;
		uint8_t type;
		uint8_t size;
		uint8_t payload[max_datagram_size-3];
	} data_frame;
	uint8_t DATA_FRAME[max_datagram_size];
};

template <typename Socket>
unsigned int udp_receive( Socket& socket )
{
	unsigned int received_size;
	unsigned int payload_size;

	received_size = socket.receive( boost::asio::buffer( DATA_FRAME, max_datagram_size ));
	payload_size  = received_size - sizeof( data_frame ) + sizeof( data_frame.payload );

#ifdef DEBUG
	std::cerr << "address:\t0x"    << int(data_frame.address) << std::endl;
	std::cerr << "type:\t\t0x"     << int(data_frame.type)    << std::endl;
	std::cerr << "payload_size:\t" << payload_size            << std::endl;
	std::cerr << "payload:\t"      << data_frame.payload      << std::endl;
#endif

	return received_size;
}

template <typename Port>
void serial_send( Port& port, uint8_t *frame, unsigned int size )
{
#ifdef DEBUG
	std::cerr << "Sending data " << size << " octets to Sentio…"
		<< std::endl << std::endl;
#endif

	port.write_some( ba::buffer( frame, size ) );
}

template <typename Socket, typename Port>
void udp_to_serial( Socket &socket, Port &port)
{
	unsigned int received_size;

	while ( true )
	{
		received_size = udp_receive( socket );
		serial_send( port, DATA_FRAME, received_size );
	}
}

template <typename Port>
void serial_read( Port &port)
{
	const unsigned int READ_BUFFER_SIZE = 1;
	std::vector<char> buffer( READ_BUFFER_SIZE );

	while ( true )
	{
		boost::asio::read( port, boost::asio::buffer( buffer ));
		std::cout << buffer[0];
	}
}

int main( int argc, char const *argv[] )
{
	const std::string  device   = argc >= 2 ? argv[1] : "/dev/ttyUSB0";
	const unsigned int udp_port = argc >= 3 ? std::atoi( argv[2] ) : 1234;

#ifdef DEBUG
	std::cerr << "Opening UDP port:\t"      << udp_port << std::endl;
	std::cerr << "Forwarding packets to:\t" << device   << std::endl << std::endl;
#endif

	try
	{
		// for socket IO
		ba::io_service net_io;
		ba::ip::udp::socket net_sock( net_io, ba::ip::udp::endpoint( ba::ip::udp::v4(), udp_port));

		// for serial IO
		ba::io_service serial_io;
		ba::serial_port port( serial_io );
		boost::system::error_code ec;

		if ( port.open( device, ec ) )
		{
			std::cout << "cannot open " << device << std::endl;
			return EXIT_FAILURE;
		}

		// serial port setup
		port.set_option( ba::serial_port::baud_rate( 2000000 ) );
		port.set_option( ba::serial_port::parity   ( ba::serial_port::parity::odd ) );

		std::thread t_udp_read_and_serial_send(
			&udp_to_serial<ba::ip::udp::socket, ba::serial_port>, std::ref( net_sock ), std::ref( port ) );

		std::thread t_serial_read(
			&serial_read<ba::serial_port>, std::ref( port ) );

		t_udp_read_and_serial_send.join();

		net_sock.close();
		port.close();

		return EXIT_SUCCESS;
	}
	catch ( const std::exception& e )
	{
		std::cout << e.what() << std::endl;
	}
}
