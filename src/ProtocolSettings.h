#ifndef PROTOCOLSETTINGS_H
#define PROTOCOLSETTINGS_H

// UDP port
#define ARTNET_PORT 6454

// Buffers
#define ARTNET_MAX_BUFFER 530
#define DMX_MAX_BUFFER 512

// Packet constants
#define ARTNET_ID "Art-Net"
#define ARTNET_DMX_START_LOC 18

// Packet confines
#define ARTNET_SHORT_NAME_MAX_LENGTH 17
#define ARTNET_LONG_NAME_MAX_LENGTH 63

// DMX settings
#define DMX_MAX_OUTPUTS 4
#define DMX_MS_BETWEEN_TICKS 25

// RDM
#define DMX_RDM_STARTCODE 0xCC

#endif
