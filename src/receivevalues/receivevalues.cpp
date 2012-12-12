#include <ctime>
#include "XBEE_Radio.h"

int main()
{
	uint8_t buffer[48];
	uint8_t length[48];
	uint8_t source[8];

	XBEE_Radio myradio;

	myradio.initializeInterface( "/dev/ttyUSB1", 38400 );
	myradio.initializeSystemBuffer( buffer, source, length );

	time_t rawtime;
	struct tm * timeinfo;
	char timebuf [32];

	while ( true )
		if ( myradio.getPacketReceived() )
		{
			time( &rawtime );
			timeinfo = localtime( &rawtime );
			strftime( timebuf, 32, "%Y-%m-%d %H:%M:%S", timeinfo );

			std::cout << timebuf << "," << buffer << std::endl;
		}

	return 0;
}
