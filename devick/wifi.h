#ifndef _WIFI_H
#define _WIFI_H

#pragma once

#include "../src/protocol.h"
#include <linux/wireless.h>

#define ESSID_LEN 32

#define FRAME_CONTROL_TYPE_MGMT_FRAME 0
#define FRAME_CONTROL_SUBTYPE_DEAUTH 12
#define FRAME_CONTROL_SUBTYPE_BEACON 8

#define INFO_ELEMENT_ID_SSID 0
#define INFO_ELEMENT_ID_DS 3

#define CHANNEL_MAX 255

struct ap_info {
	int chan;
	uint8_t bssid[IFHWADDRLEN];
	uint8_t essid[ESSID_LEN+1];
};

struct radiotap_hdr {
	uint8_t  version;
	uint8_t  pad;
	uint16_t len;
	uint32_t present;
} __attribute__((__packed__));

struct frame_control {
	uint8_t protocol_version:2;
	uint8_t type:2;
	uint8_t subtype:4;
	uint8_t to_ds:1;
	uint8_t from_ds:1;
	uint8_t more_frag:1;
	uint8_t retry:1;
	uint8_t pwr_mgt:1;
	uint8_t more_data:1;
	uint8_t protected_frame:1;
	uint8_t order:1;
} __attribute__((__packed__));

struct sequence_control {
	uint16_t fragment:4;
	uint16_t sequence:12;
} __attribute__((__packed__));


struct mgmt_frame {
	struct frame_control fc;
	uint16_t duration;
	uint8_t  dest_mac[IFHWADDRLEN];
	uint8_t  src_mac[IFHWADDRLEN];
	uint8_t  bssid[IFHWADDRLEN];
	struct sequence_control sc;
	uint8_t  frame_body[];
} __attribute__((__packed__));

struct beacon_frame_body
{
	uint64_t timestamp;
	uint16_t interval;
	uint16_t capabilities;
	uint8_t  infos[];
} __attribute__((__packed__));

struct info_element
{
	uint8_t id;
	uint8_t len;
	uint8_t info[];
} __attribute__((__packed__));

struct iw_dev {
	char ifname[IFNAMSIZ+1];
	int ifindex;
	int fd_in;
	int fd_out;
	volatile int chan;
	struct ifreq old_flags;
	struct iwreq old_mode;
};

struct access_point {
	volatile unsigned int num_of_deauths;
	time_t last_beacon_tm;
	uint16_t sequence:12;
	struct ap_info info;
	struct access_point *next;
	struct access_point *prev;
};

struct ap_list {
	struct access_point *head;
	struct access_point *tail;
};



#endif