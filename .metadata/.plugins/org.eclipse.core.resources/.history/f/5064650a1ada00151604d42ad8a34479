/*
 * wlan_mac_chan_util.h
 *
 *  Created on: Feb 12, 2016
 *      Author: gabri
 */

#ifndef WLAN_MAC_CHAN_UTIL_H_
#define WLAN_MAC_CHAN_UTIL_H_

#define WLAN_CHANNELS					    15  	/// Number of WLAN channels

#define MAX_NUM_MPDU						5		/// Maximum number of MPDUs to store
#define MAX_NUM_STAT						5
#define MAX_NUM_USER						50		/// Maximum number of users

// AP Sniff Modes
#define SNIFF_DISABLED			       		0		/// Sniff mode disabled, does not capture MPDUs
#define SNIFF_ENABLED				   		1		/// Sniff mode enabled, captures MPDUs

#include "xil_types.h"

typedef struct{
	u8		addr[6];								/// MAC address
}mac_addr;

typedef struct{
	mac_addr	src_addr;							/// Source MAC addresses
	mac_addr	dst_addr;							/// Destination MAC addresses
	u64			time_stamp;							/// Time stamp of the reception
	char		power;								/// Received power values
	u16			length;								/// Length of the MPDU in bytes
	u8			mcs;								/// MCS
	u8			fcs;								/// FCS
}mpdu_entry;

typedef struct{
	u16			num_elem;							/// Number of MPDU entries
	mpdu_entry	mpdu_entries[MAX_NUM_MPDU];			/// MPDU entries
}rx_mpdu_info;

typedef struct{
	mac_addr	user_mac;							/// User MAC address
	u32			num_mpdu[3];						/// Number of MPDUs transmitted
	u32			avg_mpdus;
}user_entry;

typedef struct{
	u8			num_elem;
	u16			num_users;							/// Number of user entries
	user_entry	user_entries[MAX_NUM_USER];			/// User entries
}mac_chan_info;

//////////// Initialization Functions ////////////
void	wlan_mac_high_init_mpdu_info(rx_mpdu_info* mpdu_info);
void	wlan_mac_high_init_chan_info(mac_chan_info* chan_info);
void	wlan_mac_high_reset_chan_info(mac_chan_info* chan_info);
void	wlan_mac_high_refresh_chan_info(mac_chan_info* chan_info);

//////////// Processing Functions ////////////////
void set_mac_chan_info(rx_mpdu_info* mpdu_info, mac_chan_info* chan_info);
void sort_users(mac_chan_info* chan_info);
int  is_known_mac(u8* addr, user_entry* users, u16 num_users);
int  wlan_eq_mac_addr(u8* addr1, u8* addr2);
int  get_mac_channel_index(u32 mac_channel);
void get_mac_chan_util(rx_mpdu_info* mpdu_info, mac_chan_info* chan_info, u32 mac_chan);
void copy_user_entry(const user_entry in, user_entry* out);
void get_avg_mpdu(user_entry* user);
void remove_inactive_users(mac_chan_info* chan_info);
void remove_user_at_index(mac_chan_info* chan_info, u16 index);

void print_mpdu_info(rx_mpdu_info* rx_info);
void print_chan_info(mac_chan_info* chan_info);
void print_chan_util(u32 mac_channel, mac_chan_info* chan_info);
void print_user_mac_addr(mac_chan_info* chan_info);
void print_users(mac_addr* users, u32 num_users);

void generate_mpdu_info(rx_mpdu_info* mpdu_info);
void generate_mpdu_entries(rx_mpdu_info* mpdu_info, mac_addr* users, u32 num_users);
void generate_users_mac(mac_addr* users, u32 max_users);
void generate_mac_addr(mac_addr* mac);
s32  random_at_most(s32 max);

#endif /* WLAN_MAC_CHAN_UTIL_H_ */
