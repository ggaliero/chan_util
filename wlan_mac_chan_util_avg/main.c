/*
 * main.c
 *
 *  Created on: Feb 12, 2016
 *      Author: gabri
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
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

/****************************************************************************************/

void test1() {

	// Intializes random number generator
	time_t t;
	srand((unsigned) time(&t));

	// Initizalizes the entry structures
	wlan_mac_high_init_mpdu_info(&mpdu_data);
	wlan_mac_high_init_chan_info(&chan_info);

	// Add a user which only transmits 5 mpdus
	u8 mult_addr [6] = {255, 255, 255, 255, 255, 255};
	user_entry user;
	memcpy(&((user.user_mac).addr[0]), &(mult_addr[0]), 6 * sizeof(u8));
	user.num_mpdu[0] = 5;
	user.num_mpdu[1] = 0;
	user.num_mpdu[2] = 0;
	copy_user_entry(user, &(chan_info.user_entries[0]));
	chan_info.num_users++;

	u32 max_users = 5;
	//u8 num_users = (u8) random_at_most(max_users);
	u8 num_users = 2;
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

	test1();

	return 0;
}
