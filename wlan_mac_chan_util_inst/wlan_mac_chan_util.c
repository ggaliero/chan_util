/*
 * wlan_mac_chan_util.c
 *
 *  Created on: Feb 12, 2016
 *      Author: gabri
 */

#include "wlan_mac_chan_util.h"
#include "xil_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////// Global Variables ///////////////
extern volatile u8	sniff_mode;

//////////// Initialization Functions ////////////

/**
 * @brief Initializes a rx_mpdu_info structure to zero elements
 *
 * @param  rx_mpdu_info* mpdu_info
 * 		- Pointer to a rx_mpdu_info structure
 * @return None
 */
void wlan_mac_high_init_mpdu_info(rx_mpdu_info* mpdu_info){
	mpdu_info->num_elem = 0;
}

/**
 * @brief Initializes a mac_chan_info structure to zero elements and  zero users
 *        It also initializes the number of MPDUs to zero, for each user
 * @param  mac_chan_info* chan_info
 * 		- Pointer to a mac_chan_info structure
 * @return None
 */
void wlan_mac_high_init_chan_info(mac_chan_info* chan_info){
	chan_info->num_users  = 0;
	u8 counter;
	for(counter = 0; counter < MAX_NUM_USER; counter++) {
		chan_info->user_entries[counter].num_mpdu = 0;
	}
}


/////////// Processing Functions ///////////////

void set_mac_chan_info(rx_mpdu_info* mpdu_info, mac_chan_info* chan_info){
	int idx;
	u16 counter;
	u16 num_users = chan_info->num_users;
	mac_addr src_mac_mpdu;
	for(counter = 0; counter < mpdu_info->num_elem; counter++){
		memcpy(&(src_mac_mpdu.addr[0]), &((mpdu_info->mpdu_entries[counter].src_addr).addr[0]), 6 * sizeof(u8));
		idx = is_known_mac(&(src_mac_mpdu.addr[0]), &(chan_info->user_entries[0]), num_users);

		if(idx != -1) {
			chan_info->user_entries[idx].num_mpdu++;
		} else {
			memcpy(&((chan_info->user_entries[num_users].user_mac).addr[0]), &(src_mac_mpdu.addr[0]), 6 * sizeof(u8));
			chan_info->user_entries[num_users].num_mpdu++;
			num_users++;
		}
	}
	chan_info->num_users = num_users;
	wlan_mac_high_init_mpdu_info(mpdu_info);
	//Debug
	//chan_info->num_elem++;
}



int is_known_mac(u8* addr, user_entry* users, u16 num_users){
	u16 idx;
	for(idx = 0; idx < num_users; idx++){
		if(wlan_eq_mac_addr(addr, &((users[idx].user_mac).addr[0]))){
			return idx;
		}
	}
	return -1;
}

/**
 * @brief Compares two MAC addresses and indicates if they are the same
 *
 * @param  u8* addr1
 *     - Pointer to a MAC address
 * @param  u8* addr2
 *     - Pointer to a MAC address
 * @return
 *     - 0 if MAC addresses are not equal
 *     - 1 if MAC addresses are equal
 */
int  wlan_eq_mac_addr(u8* addr1, u8* addr2) {
	u8 idx;
	for(idx = 0; idx < 6; idx++) {
		if(addr1[idx] != addr2[idx]){
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Sorts the array of users within mac_chan_info structure
 *
 * This function sorts the array of users which access the channel, in such a way
 * that the users generating more traffic are in the first position in the array
 *
 * @param  mac_chan_info* chan_info
 *     - Pointer to the structure which contains the statistics for the channel
 * @return None
 */
void sort_users(mac_chan_info* chan_info) {
	u8 counter1;
	u8 counter2;
	u32 aux;
	mac_addr aux_mac;
	u16 num_user = chan_info->num_users;
	for(counter1 = 0; counter1 < num_user; counter1++) {
		for(counter2 = 0; counter2 < num_user - counter1; counter2++) {
			if(chan_info->user_entries[counter2].num_mpdu < chan_info->user_entries[counter2 + 1].num_mpdu) {
				memcpy(&(aux_mac.addr[0]), &((chan_info->user_entries[counter2].user_mac).addr[0]), 6 * sizeof(u8));
				aux = chan_info->user_entries[counter2].num_mpdu;
				memcpy(&((chan_info->user_entries[counter2].user_mac).addr[0]), &((chan_info->user_entries[counter2 + 1].user_mac).addr[0]), 6 * sizeof(u8));
				chan_info->user_entries[counter2].num_mpdu = chan_info->user_entries[counter2 + 1].num_mpdu;
				memcpy(&((chan_info->user_entries[counter2 + 1].user_mac).addr[0]), &(aux_mac.addr[0]), 6 * sizeof(u8));
				chan_info->user_entries[counter2 + 1].num_mpdu = aux;
			}
		}
	}
}

/**
 * @brief Calculate the number of users which are transmitting over the channel
 *        and number of MPDUs per second for each independent user
 *
 * Calculate how many users are accessing/transmitting information over the channel
 * and accounts for the average number of captured MPDUs for each user. The AP will pause
 * the sniff mode to calculate the statistics.
 *
 * @param  None
 * @return None
 */
void get_mac_chan_util(rx_mpdu_info* mpdu_info, mac_chan_info* chan_info, u32 mac_chan) {
	// Sniff mode is paused until the channel utilization is processed
	sniff_mode = SNIFF_DISABLED;
	// If the structure rx_mpdu_info has elements on it, process them and store
	// in chan_info
	if(mpdu_info->num_elem > 0) {
		set_mac_chan_info(mpdu_info,chan_info);		// Update statistics to chan_info
	}

	//Debug
	//print_chan_info(chan_info);

	sort_users(chan_info);
	// Prints the channel utilization menu with the list of users and the MPDUs per second
	print_chan_util(mac_chan, chan_info);
	// Reset chan_info to zero users and MPDUs per second
	// Debug, uncomment for normal operation
	wlan_mac_high_init_chan_info(chan_info);

	// Sniff mode is again enabled
	sniff_mode = SNIFF_ENABLED;
}

/**
 * @brief Print information of the captured mpdus
 *
 * Print information values of the sniffed packets:
 * 		- Time difference with respect the previous sniffed packet
 * 		- Sequence number of the RTP packet
 * 		- Received power (dBm)
 * 		- MAC source and destination address
 * 		- MCS
 * 		- FCS
 *
 * @param  rx_mpdu_info* mpdu_info
 * 		- Pointer to rx_mpdu_info structure comprising the information of all captured mpdus
 * @return None
 *
 */
void print_mpdu_info(rx_mpdu_info* mpdu_info){
	int counter;		// Counter to go through all elements of rx_pkt_val array
	int n_elem;			// Indicates number of elements that rx_mpdu_info* comprises
	mac_addr src_mac;	// MAC source address of the captured mpdus
	mac_addr dst_mac;	// MAC destination address of the captured mpdus

	if(mpdu_info != NULL){
		n_elem = mpdu_info->num_elem;
		for(counter = 0; counter < n_elem; counter++){
			memcpy(&src_mac.addr[0], &((mpdu_info->mpdu_entries[counter].src_addr).addr[0]), 6 * sizeof(u8));
			memcpy(&dst_mac.addr[0], &((mpdu_info->mpdu_entries[counter].dst_addr).addr[0]), 6 * sizeof(u8));
			fprintf(stdout,"%05d - ", mpdu_info->mpdu_entries[counter].time_stamp);
			fprintf(stdout,"Prx=%d dBm -> ", mpdu_info->mpdu_entries[counter].power);
			fprintf(stdout,"Src: %02x:%02x:%02x:%02x:%02x:%02x -> ",
					    src_mac.addr[0], src_mac.addr[1], src_mac.addr[2],
					    src_mac.addr[3], src_mac.addr[4], src_mac.addr[5]);
			fprintf(stdout,"Dst: %02x:%02x:%02x:%02x:%02x:%02x - ",
					    dst_mac.addr[0], dst_mac.addr[1], dst_mac.addr[2],
					    dst_mac.addr[3], dst_mac.addr[4], dst_mac.addr[5]);
			fprintf(stdout,"FCS= %d - ", mpdu_info->mpdu_entries[counter].fcs);
			fprintf(stdout,"mcs= %d", mpdu_info->mpdu_entries[counter].mcs);
			fprintf(stdout,"\n");
		}
		fprintf(stdout,"Number of captured MPDUs: %d\n", mpdu_info->num_elem);
	}
}

void print_chan_info(mac_chan_info* chan_info){
	u16 counter;
	u16 n_user;
	mac_addr user_mac;

	if(chan_info != NULL){
		n_user = chan_info->num_users;
		for(counter = 0; counter < n_user; counter++){
			memcpy(&user_mac.addr[0], &((chan_info->user_entries[counter].user_mac).addr[0]), 6 * sizeof(u8));
			fprintf(stdout, "user %02d: %02x:%02x:%02x:%02x:%02x:%02x\t mpdus: %d\n",
					counter,
					user_mac.addr[0], user_mac.addr[1], user_mac.addr[2],
					user_mac.addr[3], user_mac.addr[4], user_mac.addr[5],
					chan_info->user_entries[counter].num_mpdu);
		}
	}
}


/**
 * @brief Prints the channel utilization giving specific MPDUs per second for each
 *        of the users accessing the channel
 *
 * @param u32 mac_channel
 * 		- MAC channel
 * @param mac_chan_info* chan_info
 * 		- Structure comprising the channel information
 * @return None
 */
void print_chan_util(u32 mac_channel, mac_chan_info* chan_info){
	u16 counter;								// Counter to loop over the users
	mac_addr user_mac;							// User MAC address
	u16 user_per_sec = chan_info->num_users;	// Number of users per second
	fprintf(stdout,"-------------Channel Utilization-------------\n");
	fprintf(stdout,"Channel:            %d\n", mac_channel);
	fprintf(stdout,"Users/sec:          %d\n", user_per_sec);
	// Prints the list of users and the MPDUs per second
	for(counter = 0; counter < user_per_sec; counter++) {
		memcpy(&(user_mac.addr[0]), &((chan_info->user_entries[counter].user_mac).addr[0]), 6 * sizeof(u8));
		fprintf(stdout,"user %02d: %02x:%02x:%02x:%02x:%02x:%02x\t mpdu/s: %d\n", counter+1,
					user_mac.addr[0], user_mac.addr[1], user_mac.addr[2],
					user_mac.addr[3], user_mac.addr[4], user_mac.addr[5],
					chan_info->user_entries[counter].num_mpdu);
	}
	fprintf(stdout,"---------------------------------------------\n");
}

/**
 * @brief Prints a list of the user MAC addresses which are currently accessing
 *        the channel
 *
 * @param mac_chan_info* chan_info
 * 		- Information about channel usage
 * @return None
 */
void print_user_mac_addr(mac_chan_info* chan_info){
	u16 counter;
	mac_addr user_mac;
	for(counter = 0; counter < chan_info->num_users; counter++){
		memcpy(&user_mac.addr[0], &((chan_info->user_entries[counter].user_mac).addr[0]), 6 * sizeof(u8));
		fprintf(stdout,"user %02d: %02x:%02x:%02x:%02x:%02x:%02x\n", counter,
			        user_mac.addr[0], user_mac.addr[1], user_mac.addr[2],
					user_mac.addr[3], user_mac.addr[4], user_mac.addr[5]);
	}
}

/**
 * @brief Maps the MAC channel with the index of the channel within channels
 * array variable
 *
 * @param  u32 mac_channel
 * 		- Value of the MAC channel, 1-11, 36,40,44,48
 * @return int returned_value
 * 		- Position of the channel within the external variable channels
 * @author Gabriele Galiero Casay
 */
int get_mac_channel_index(u32 mac_channel){
	int returned_value;
	if(mac_channel > 0 && mac_channel < 12){	//2.4 GHz
		returned_value = mac_channel - 1;
	}else{										//5 GHz
		switch(mac_channel){
			case 36:
				returned_value = 11;
			break;
			case 40:
				returned_value = 12;
			break;
			case 44:
				returned_value = 13;
			break;
			case 48:
				returned_value = 14;
			break;
			default:
				returned_value = -1;
			break;
		}
	}
	return returned_value;
}

void generate_mpdu_info(rx_mpdu_info* mpdu_info) {

	// Multicast mac addr
	u8 mult_addr [6] = {255, 255, 255, 255, 255, 255};

	// Generates a random number of users
	u32 num_users = (u32) random_at_most(3);		//10

	if(num_users > 0) {		// Only if there are users, we fill the mpdu structure

		mac_addr users_mac_addr [num_users];	// Array with user mac addresses

		// Generates user mac addresses
		u32 counter_users;
		for(counter_users = 0; counter_users < num_users; counter_users++) {
			generate_mac_addr(&(users_mac_addr[counter_users]));
		}

		u16 counter_mpdus;
		u16 idx;
		for(counter_mpdus = 0; counter_mpdus < MAX_NUM_MPDU; counter_mpdus++) {
			idx = random_at_most(num_users);
			memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].src_addr).addr[0]), &((users_mac_addr[idx]).addr[0]), 6 * sizeof(u8));
			memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].dst_addr).addr[0]), &(mult_addr[0]), 6 * sizeof(u8));
			mpdu_info->mpdu_entries[counter_mpdus].fcs = 1;
			mpdu_info->mpdu_entries[counter_mpdus].mcs = 3;
			mpdu_info->mpdu_entries[counter_mpdus].power = -35;
			mpdu_info->mpdu_entries[counter_mpdus].time_stamp = 100000;
			mpdu_info->num_elem++;
		}
	}
}

void generate_users_mac(mac_addr* users, u32 num_users) {
	u32 num;
	if(num_users > 0 && users != NULL) {
		for(num = 0; num < num_users; num++) {
			generate_mac_addr(&(users[num]));
		}
	}
}

void print_users(mac_addr* users, u32 num_users) {
	u32 counter;
	fprintf(stdout, "List of random generated users:\n");
	for(counter = 0; counter < num_users; counter++) {
		fprintf(stdout,"user %02d: %02x:%02x:%02x:%02x:%02x:%02x\n", counter + 1,
				users[counter].addr[0], users[counter].addr[1],users[counter].addr[2],
				users[counter].addr[3], users[counter].addr[4],users[counter].addr[5]);
	}
}

void generate_mpdu_entries(rx_mpdu_info* mpdu_info, mac_addr* users, u32 num_users){
	// Multicast mac addr
	u8 mult_addr [6] = {255, 255, 255, 255, 255, 255};
	// Initialize the array of MPDU entries to zero elements
	wlan_mac_high_init_mpdu_info(mpdu_info);

	u32 counter_mpdus;
	u32 idx;
	for(counter_mpdus = 0; counter_mpdus < MAX_NUM_MPDU; counter_mpdus++){
		idx = random_at_most(num_users-1);
		fprintf(stdout,"User index: %d\n", idx+1);
		memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].src_addr).addr[0]), &((users[idx])), 6 * sizeof(u8));
		memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].dst_addr).addr[0]), &(mult_addr[0]), 6 * sizeof(u8));
		mpdu_info->mpdu_entries[counter_mpdus].fcs = 1;
		mpdu_info->mpdu_entries[counter_mpdus].mcs = 3;
		mpdu_info->mpdu_entries[counter_mpdus].power = -35;
		mpdu_info->mpdu_entries[counter_mpdus].time_stamp = 100000;
		mpdu_info->num_elem++;
	}
}

//void generate_mpdu_entries(rx_mpdu_info* mpdu_info, u32 max_users){
//	// Multicast mac addr
//	u8 mult_addr [6] = {255, 255, 255, 255, 255, 255};
//	// Initialize the array of MPDU entries to zero elements
//	wlan_mac_high_init_mpdu_info(mpdu_info);
//	// Generates random number of users
//	u32 num_users = random_at_most(max_users);
//	fprintf(stdout,"Number random generated users: %d\n", num_users);
//	if(num_users > 0) {
//		mac_addr users_mac_addr [num_users];	// Array with user mac addresses
//		// Generates user mac addresses
//		u32 counter_users;
//		for(counter_users = 0; counter_users < num_users; counter_users++) {
//			generate_mac_addr(&(users_mac_addr[counter_users]));
//			fprintf(stdout,"user %02d: %02x:%02x:%02x:%02x:%02x:%02x\n", counter_users+1,
//					users_mac_addr[counter_users].addr[0], users_mac_addr[counter_users].addr[1],users_mac_addr[counter_users].addr[2],
//					users_mac_addr[counter_users].addr[3], users_mac_addr[counter_users].addr[4],users_mac_addr[counter_users].addr[5]);
//		}
//
//		fprintf(stdout,"List of user index used for each MPDU entry:\n");
//
//		u32 counter_mpdus;
//		u32 idx;
//		for(counter_mpdus = 0; counter_mpdus < MAX_NUM_MPDU; counter_mpdus++){
//			idx = random_at_most(num_users-1);
//			fprintf(stdout,"User: %d\n", idx+1);
//			memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].src_addr).addr[0]), &((users_mac_addr[idx]).addr[0]), 6 * sizeof(u8));
//			memcpy(&((mpdu_info->mpdu_entries[counter_mpdus].dst_addr).addr[0]), &(mult_addr[0]), 6 * sizeof(u8));
//			mpdu_info->mpdu_entries[counter_mpdus].fcs = 1;
//			mpdu_info->mpdu_entries[counter_mpdus].mcs = 3;
//			mpdu_info->mpdu_entries[counter_mpdus].power = -35;
//			mpdu_info->mpdu_entries[counter_mpdus].time_stamp = 100000;
//			mpdu_info->num_elem++;
//		}
//	}
//}

void generate_mac_addr(mac_addr* mac) {
	u8 counter;
	for(counter = 0; counter < 6; counter++) {
		mac->addr[counter] = random_at_most(255);
	}
}

s32 random_at_most(s32 max) {
  u32
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (u32) max + 1,
    num_rand = (u32) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  s32 x;
  do {
   x = rand();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (u32)x);

  // Truncated division is intentional
  return x/bin_size;
}
