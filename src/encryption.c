/*
 * encryption.c
 *
 *  Created on: Aug 16, 2016
 *      Author: ChauNM
 */

#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

char* GetCpuid()
{
	FILE *f = fopen("/proc/cpuinfo", "r");
	if (!f) {
		return NULL;
	}
	char line[256];
	//int serial;
	char* serial_string = NULL;
	while (fgets(line, 256, f)) {
		if (strncmp(line, "Serial", 6) == 0) {
			serial_string = calloc(17, sizeof(char));
			strcpy(serial_string, strchr(line, ':') + 2);
		}
	}
	fclose(f);
	return serial_string;
}

int main (void)
{
	char* cpuid = GetCpuid();
	// add key to session keyring
#ifndef RELEASE
	printf("add key\n");
#endif
	system("sudo printf \"%s\" \"raspberry\" | ecryptfs-insert-wrapped-passphrase-into-keyring /home/pi/.ecryptfs/wrapped-passphrase -");
	// mount encrypt folder
#ifndef RELEASE
	printf("mount encypted folder\n");
#endif
	system("sudo mount -i -t ecryptfs /home/pi/.encrypt /home/pi/encrypt/ -o ecryptfs_sig=85386d4c52ae4323,"
		"ecryptfs_fnek_sig=85386d4c52ae4323,ecryptfs_cipher=aes,ecryptfs_key_bytes=32,ecryptfs_unlink_sigs");
	sleep(2);
	// try to open encrypted file that contain cpuid
	FILE* fCpuId = fopen("/home/pi/encrypt/.cpuid", "r");
	if (fCpuId != NULL) // file already exists
	{
		char line[256];
		if(strcmp(fgets(line, 256, fCpuId), cpuid) == 0)
		{
			fclose(fCpuId);
			return EXIT_SUCCESS;
		}
		fclose(fCpuId);
#ifndef RELEASE
		printf("wrong cpuid, unmount \n");
#endif
		system("sudo umount /home/pi/encrypt");
	}
	else // no cpuid file
	{
#ifndef RELEASE
		printf("no cpuid file, creat one\n");
#endif
		fCpuId = fopen("/home/pi/encrypt/.cpuid", "w");
		fprintf(fCpuId, "%s", cpuid);
		fclose(fCpuId);
	}
#ifndef RELEASE
	printf("cpu_id: %s\n", cpuid);
#endif
	return EXIT_SUCCESS;
}
