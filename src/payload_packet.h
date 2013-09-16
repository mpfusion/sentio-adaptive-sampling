/*
 * CC1101 - Transmitter and serial receiver - payload_packet.h
 *
 *  Created on: 2013-09-11
 *      Author: Marco Patzer
 */

#ifndef PAYLOAD_PACKET_H_MB9NXB2W
#define PAYLOAD_PACKET_H_MB9NXB2W

namespace Packet
{
	static union {
		struct {
			uint8_t  node_id;
			float    temperature;
			float    humidity;
			uint16_t adaptive_slices;
			uint16_t sleep_time;
			float    battery_level;
		} payload_packet;
		uint8_t payload[60];
	};
}

#endif /* end of include guard: PAYLOAD_PACKET_H_MB9NXB2W */
