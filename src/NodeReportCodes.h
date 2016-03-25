#ifndef NODEREPORTCODES_H
#define NODEREPORTCODES_H
// List of hex values and discriptions of Node Report Codes

#define RcDebug        0x0000  // Booted in debug mode (Only used in development)
#define RcPowerOk      0x0001  // Power On Tests successful
#define RcPowerFail    0x0002  // Hardware tests failed at Power On
#define RcSocketWr1    0x0003  // Last UDP from Node failed due to truncated length, Most likely caused by a collision.
#define RcParseFail    0x0004  // Unable to identify last UDP transmission. Check OpCode and \packet length.
#define RcUdpFail      0x0005  // Unable to open Udp Socket in last transmission attempt
#define RcShNameOk     0x0006  // Confirms that Short Name programming via ArtAddress, was successful.
#define RcLoNameOk     0x0007  // Confirms that Long Name programming via ArtAddress, was successful.
#define RcDmxError     0x0008  // DMX512 receive errors detected.
#define RcDmxUdpFull   0x0009  // Ran out of internal DMX transmit buffers.
#define RcDmxRxFull    0x000a  // Ran out of internal DMX Rx buffers.
#define RcSwitchErr    0x000b  // Rx Universe switches conflict.
#define RcConfigErr    0x000c  // Product configuration does not match firmware.
#define RcDmxShort     0x000d  // DMX output short detected. See GoodOutput field.
#define RcFirmwareFail 0x000e  // Last attempt to upload new firmware failed.
#define RcUserFail     0x000f  // User changed switch settings when address locked by remote programming. User changes ignored.

#endif