#include <ctime>
#include "XBEE_Radio.h"

int main()
{
	uint8_t length[48];
	uint8_t source[8];

	union {
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

	XBEE_Radio myradio;

	myradio.initializeInterface( "/dev/ttyUSB1", 38400 );
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

	return 0;
}
