//============================================================================
// Name        : cbcmacattack.cpp
// Author      : Klemens Gro?mann
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "oracle.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#define MAX_LENGTH 512
#define BLOCK_LENGTH 16
static string attack_msg = "I, the server, hereby agree that I will pay $100 to this student";
//
void PrintOutTag(unsigned char* tag) {
	cout << "Forged tag for messsage '" << attack_msg.c_str() << "': " <<  endl;
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) {
		printf("%2X",tag[i]);
	}
	printf("\n");
}
//
void LetOracleTagMessage(unsigned char* message, unsigned int msglen, unsigned char* tag) {
	Oracle_Connect();
	Mac(message,(int)msglen,tag);
	Oracle_Disconnect();
}
//
void LetOracleVerifyTagAndMessage(unsigned char* message, unsigned int msglen, unsigned char* tag) {
	Oracle_Connect();
	if (Vrfy(message,(int)msglen, tag)) {
		printf("Message verified successfully!\n");
	} else {
		printf("Message verficiation failed.\n");
	}
	Oracle_Disconnect();
}
//
int main(int argc, char *argv[]) {
	cout << "MOOC Cryptography - PA4" << endl;
	//
	unsigned char message[MAX_LENGTH];
	unsigned char taggedmessage[MAX_LENGTH];
	unsigned char tag[BLOCK_LENGTH];
	unsigned int  msglen = attack_msg.size();
	memset(message,0,MAX_LENGTH);
	memset(taggedmessage,0,MAX_LENGTH);
	for(unsigned int i=0;i<msglen;i++) {
		message[i] = attack_msg[i];
	}
	//1. send the first two blocks to the MAC oracle a get the intermediate tag
	for(unsigned int i=0;i<2*BLOCK_LENGTH;i++) {
		taggedmessage[i] = message[i];
	}
	//
	LetOracleTagMessage(taggedmessage,2*BLOCK_LENGTH,tag);
	//the xor of the thirst message block with the intermediate tag gives the first block
	//of the next message to send to the MAC oracle:
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) {
		taggedmessage[i] = tag[i]^message[2*BLOCK_LENGTH+i];
	}
	//the second block of the next message to send is the 4th block of the text
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) {
		taggedmessage[BLOCK_LENGTH+i] = message[3*BLOCK_LENGTH+i];
	}
	LetOracleTagMessage(taggedmessage,2*BLOCK_LENGTH,tag);
	LetOracleVerifyTagAndMessage(message,msglen,tag);
	PrintOutTag(tag);
	return 0;
}
