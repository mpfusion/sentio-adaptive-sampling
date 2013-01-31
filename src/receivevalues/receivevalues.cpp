#include <cstdlib>
#include <ctime>
#include "XBEE_Radio.h"

static uint8_t length[48];
static uint8_t source[8];

static union {
	struct {
		uint8_t temperature[7];
		uint8_t delimiter0;
		uint8_t slice[3];
		uint8_t delimiter1;
		uint8_t battery[3];
		uint8_t delimiter2;
		uint8_t nodeID;
		uint8_t delimiter3;
	} payload;
	uint8_t PAYLOAD[18];
};

int main( int argc, char const *argv[] )
{
	XBEE_Radio myradio;

	//.device is first argument or default value
	std::string device = argc == 2 ? argv[1] : "/dev/ttyUSB1";

	myradio.initializeInterface( device, 38400 );
	myradio.initializeSystemBuffer( PAYLOAD, source, length );

	time_t rawtime;
	struct tm * timeinfo;
	char timebuf [32];

	while ( true )
		if ( myradio.getPacketReceived() )
		{
			time( &rawtime );
			timeinfo = localtime( &rawtime );
			strftime( timebuf, 32, "%Y-%m-%d %H:%M:%S", timeinfo );

			std::cout
				<< payload.nodeID      << ','
				<< timebuf             << ','
				<< payload.temperature << ','
				<< payload.slice       << ','
				<< payload.battery     << std::endl;
		}

	return EXIT_SUCCESS;
}
