#ifndef PRIORITYCODES_H
#define PRIORITYCODES_H
// List of hex values and discriptions of Priority Codes

#define DpLow      0x10  // Low priority message.
#define DpMed      0x40  // Medium priority message.
#define DpHigh     0x80  // High priority message.
#define DpCritical 0xe0  // Critical priority message.
#define DpVolatile 0xf0  // Volatile message. Messages of this type are displayed on a single line in the DMX-Workshop diagnostics display. All other types are displayed in a list box

#endif