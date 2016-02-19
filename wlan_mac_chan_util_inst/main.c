/*
 * main.c
 *
 *  Created on: Feb 12, 2016
 *      Author: gabri
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "wlan_mac_chan_util.h"
#include "xil_types.h"

/*************************** Global Variables *******************************************/
volatile u8 		sniff_mode;
rx_mpdu_info		mpdu_data;
mac_chan_info		chan_info;
/****************************************************************************************/

/*************************** Function Prototypes *******************************************/
void test1();
void test2();
/****************************************************************************************/

void test1() {
	wlan_mac_high_init_mpdu_info(&mpdu_data);
	wlan_mac_high_init_chan_info(&chan_info);

	fprintf(stdout,"Number of MPDU entries: %d\n", mpdu_data.num_elem);
	fprintf(stdout,"Number of user entries: %d\n", chan_info.num_users);

	u8 user1 [6] = {128, 56, 230, 15, 178, 97};
	u8 user2 [6] = {250, 26, 87, 187, 110, 98};

	// First MPDU entry
	mpdu_data.mpdu_entries[0].mcs = 3;
	mpdu_data.mpdu_entries[0].power = -35;
	mpdu_data.mpdu_entries[0].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[0].src_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[0].dst_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Second MPDU entry
	mpdu_data.mpdu_entries[1].mcs = 3;
	mpdu_data.mpdu_entries[1].power = -35;
	mpdu_data.mpdu_entries[1].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[1].src_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[1].dst_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Third MPDU entry
	mpdu_data.mpdu_entries[2].mcs = 1;
	mpdu_data.mpdu_entries[2].power = -20;
	mpdu_data.mpdu_entries[2].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[2].src_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[2].dst_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Forth MPDU entry
	mpdu_data.mpdu_entries[3].mcs = 1;
	mpdu_data.mpdu_entries[3].power = -20;
	mpdu_data.mpdu_entries[3].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[3].src_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[3].dst_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Fifth MPDU entry
	mpdu_data.mpdu_entries[4].mcs = 1;
	mpdu_data.mpdu_entries[4].power = -20;
	mpdu_data.mpdu_entries[4].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[4].src_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[4].dst_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	print_mpdu_info(&mpdu_data);
	set_mac_chan_info(&mpdu_data, &chan_info);
	fprintf(stdout,"Number of MPDU entries: %d\n", mpdu_data.num_elem);
	fprintf(stdout,"Number of users: %d\n", chan_info.num_users);
	//sort_users(&chan_info);
	print_chan_info(&chan_info);

	u8 user3 [6] = {180, 88, 21, 200, 110, 52};
	u8 user4 [6] = {99, 121, 59, 25, 201, 156};

	// First MPDU entry
	mpdu_data.mpdu_entries[0].mcs = 3;
	mpdu_data.mpdu_entries[0].power = -35;
	mpdu_data.mpdu_entries[0].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[0].src_addr).addr[0]), &(user4[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[0].dst_addr).addr[0]), &(user3[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Second MPDU entry
	mpdu_data.mpdu_entries[1].mcs = 3;
	mpdu_data.mpdu_entries[1].power = -35;
	mpdu_data.mpdu_entries[1].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[1].src_addr).addr[0]), &(user3[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[1].dst_addr).addr[0]), &(user4[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Third MPDU entry
	mpdu_data.mpdu_entries[2].mcs = 1;
	mpdu_data.mpdu_entries[2].power = -20;
	mpdu_data.mpdu_entries[2].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[2].src_addr).addr[0]), &(user3[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[2].dst_addr).addr[0]), &(user4[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Forth MPDU entry
	mpdu_data.mpdu_entries[3].mcs = 1;
	mpdu_data.mpdu_entries[3].power = -20;
	mpdu_data.mpdu_entries[3].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[3].src_addr).addr[0]), &(user4[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[3].dst_addr).addr[0]), &(user3[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	// Fifth MPDU entry
	mpdu_data.mpdu_entries[4].mcs = 1;
	mpdu_data.mpdu_entries[4].power = -20;
	mpdu_data.mpdu_entries[4].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[4].src_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[4].dst_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	print_mpdu_info(&mpdu_data);
	set_mac_chan_info(&mpdu_data, &chan_info);
	fprintf(stdout,"Number of MPDU entries: %d\n", mpdu_data.num_elem);
	fprintf(stdout,"Number of users: %d\n", chan_info.num_users);
	print_chan_info(&chan_info);

	//Now we just fill three more MPDU entries
	mpdu_data.mpdu_entries[0].mcs = 3;
	mpdu_data.mpdu_entries[0].power = -35;
	mpdu_data.mpdu_entries[0].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[0].src_addr).addr[0]), &(user1[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[0].dst_addr).addr[0]), &(user2[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	mpdu_data.mpdu_entries[1].mcs = 1;
	mpdu_data.mpdu_entries[1].power = -35;
	mpdu_data.mpdu_entries[1].time_stamp = 10000;
	memcpy(&((mpdu_data.mpdu_entries[1].src_addr).addr[0]), &(user4[0]), 6 * sizeof(u8));
	memcpy(&((mpdu_data.mpdu_entries[1].dst_addr).addr[0]), &(user3[0]), 6 * sizeof(u8));
	mpdu_data.num_elem++;

	print_mpdu_info(&mpdu_data);
	get_mac_chan_util(&mpdu_data,&chan_info,1);
	print_chan_info(&chan_info);
}

void test2() {

	// Intializes random number generator
	time_t t;
	srand((unsigned) time(&t));

	// Initizalizes the entry structures
	wlan_mac_high_init_mpdu_info(&mpdu_data);
	wlan_mac_high_init_chan_info(&chan_info);

	u32 max_users = 5;
	u8 num_users = (u8) random_at_most(max_users);
	fprintf(stdout,"Number of random generated users: %d\n\n", num_users);

	if(num_users > 0) {
		mac_addr users [num_users];
		generate_users_mac(&(users[0]), num_users);
		print_users(&(users[0]), num_users);
		u8 counter = 0;
		while(counter < 10) {
			u8 times;
			for(times = 0; times < 3; times++) {
				generate_mpdu_entries(&mpdu_data, &users[0], num_users);
				print_mpdu_info(&mpdu_data);
				set_mac_chan_info(&mpdu_data, &chan_info);
				fprintf(stdout,"Number of detected users: %d\n", chan_info.num_users);
				print_chan_info(&chan_info);
			}

			get_mac_chan_util(&mpdu_data, &chan_info, 1);
			counter++;

			time_t start, end;
			double elapsed;

			time(&start);  /* start the timer */

			do {
				time(&end);

				elapsed = difftime(end, start);
				/* For most data types one could simply use
				   elapsed = end - start;
				   but for time_t one needs to use difftime(). */
			} while(elapsed < 1);  /* run for 1 seconds */

		}
	}
}

int main() {

	test2();


	return 0;
}
