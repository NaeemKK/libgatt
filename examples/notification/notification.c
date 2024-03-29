/*
 *
 *  GattLib - GATT Library
 *
 *  Copyright (C) 2016-2017  Olivier Martin <olivier@labapart.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "gattlib.h"

// Battery Level UUID
const uuid_t g_battery_level_uuid = CREATE_UUID16(0x2A19);
static uuid_t g_uuid;
gatt_connection_t* connection;
GMainLoop *loop;

void notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
	int i;

	printf("Notification Handler: ");
	g_main_loop_quit(loop);
	//gattlib_rssi(connection);

	/*for (i = 0; i < data_length; i++) {
		printf("%02x ", data[i]);
	}
	printf("\n");*/
}

static void usage(char *argv[]) {
	printf("%s <device_address>\n", argv[0]);
}

int main(int argc, char *argv[]) {
	int ret;


	if (argc != 2) {
		usage(argv);
		return 1;
	}

	if (gattlib_string_to_uuid("c0687591-18c2-4fe6-9a45-fc709de0ee4f", 36 + 1, &g_uuid) < 0) {
			usage(argv);
			return 1;
		}
	if (g_uuid.type == 0x1C)
	{
		uint8_t copy[16];
		memcpy(copy, &g_uuid.value, 16);
		printf("UUID: ");
		for(int i=0; i<16; i++)
			printf("%02X", copy[i]);
		printf("\n");
	}
	connection = gattlib_connect(NULL, argv[1], BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);
	if (connection == NULL) {
		fprintf(stderr, "Fail to connect to the bluetooth device.\n");
		return 1;
	}

	gattlib_register_notification(connection, notification_handler, NULL);

	ret = gattlib_notification_start(connection, &g_uuid);
	if (ret) {
		fprintf(stderr, "Fail to start notification\n.");
		return 1;
	}


	loop = g_main_loop_new(NULL, 0);
	g_main_loop_run(loop);
	printf("loop ended\n");
	g_main_loop_unref(loop);
	gattlib_disconnect(connection,true);
	puts("Done");
	return 0;
}
