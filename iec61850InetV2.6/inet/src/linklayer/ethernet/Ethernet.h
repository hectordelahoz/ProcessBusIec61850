/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __INET_ETHERNET_H
#define __INET_ETHERNET_H

#include "INETDefs.h"


// Constants from the 802.3 spec
#define MAX_PACKETBURST              13

#define GIGABIT_MAX_BURST_BYTES            8192  /* don't start new frame after 8192 or more bytes already transmitted */
#define MAX_ETHERNET_DATA_BYTES            1500  /* including LLC, SNAP etc headers */
#define MAX_ETHERNET_FRAME_BYTES           1518  /* excludes preamble and SFD */
#define MIN_ETHERNET_FRAME_BYTES             64  /* excludes preamble and SFD */
#define GIGABIT_MIN_FRAME_BYTES_WITH_EXT    512  /* excludes preamble and SFD, but includes 448 byte extension */
#define INTERFRAME_GAP_BITS                  96

#define ETHERNET_TXRATE                      10000000.0   /* 10 Mbit/sec (in bit/s) */
#define FAST_ETHERNET_TXRATE                100000000.0   /* 100 Mbit/sec (in bit/s) */
#define GIGABIT_ETHERNET_TXRATE            1000000000.0   /* 1 Gbit/sec (in bit/s) */
#define FAST_GIGABIT_ETHERNET_TXRATE      10000000000.0   /* 10 Gbit/sec (in bit/s) */
#define FOURTY_GIGABIT_ETHERNET_TXRATE    40000000000.0   /* 40 Gbit/sec (in bit/s) */
#define HUNDRED_GIGABIT_ETHERNET_TXRATE  100000000000.0   /* 100 Gbit/sec (in bit/s) */

#define MAX_ATTEMPTS                 16
#define BACKOFF_RANGE_LIMIT          10
#define JAM_SIGNAL_BYTES             4
#define PREAMBLE_BYTES               7
#define SFD_BYTES                    1
#define PAUSE_UNIT_BITS              512 /* one pause unit is 512 bit times */

#define ETHER_MAC_FRAME_BYTES        (6+6+2+4) /* src(6)+dest(6)+length/type(2)+FCS(4) */
#define ETHER_LLC_HEADER_LENGTH      (3) /* ssap(1)+dsap(1)+control(1) */
#define ETHER_SNAP_HEADER_LENGTH     (5) /* org(3)+local(2) */
#define ETHER_802_1_Q_HEADER_LENGTH  (4) /* TCI(2)+APPTYPE(2)*/
#define ETHER_PAUSE_COMMAND_BYTES    (2+2) /* opcode(2)+parameters(2) */

#define ETHER_PAUSE_COMMAND_PADDED_BYTES std::max(MIN_ETHERNET_FRAME_BYTES, ETHER_MAC_FRAME_BYTES+ETHER_PAUSE_COMMAND_BYTES)

struct stPriTag
{   uint8_t ppc:3;      // Priority Code Point (PCP): a 3-bit field which refers to the IEEE 802.1p priority. It indicates the frame priority level.
                        // Values are from 0 (best effort) to 7 (highest); 1 represents the lowest priority.

    uint8_t de:1;       // Drop Eligible (DE): a 1-bit field. May be used separately or in conjunction with PCP to indicate frames eligible to be dropped in the presence of congestion.

    uint16_t vid:12;    // VLAN Identifier (VID): a 12-bit field specifying the VLAN to which the frame belongs.
                        // The hexadecimal values of 0x000 and 0xFFF are reserved. All other values may be used as VLAN identifiers, allowing up to 4,094 VLANs.
                        // The reserved value 0x000 indicates that the frame does not belong to any VLAN; in this case,
                        // the 802.1Q tag specifies only a priority and is referred to as a priority tag. On bridges,
                        // VLAN 1 (the default VLAN ID) is often reserved for a management VLAN; this is vendor-specific.
};

#endif

