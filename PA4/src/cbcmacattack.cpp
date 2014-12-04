#include "oracle.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
using namespace std;

#define MAX_LENGTH 512
#define BLOCK_LENGTH 16

void PrintOutTag(unsigned char* tag) {
	cout << "Forged tag: " <<  endl;
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) {
		printf("%2X",tag[i]);
	}
	cout << endl;
}

void LetOracleTagMessage(unsigned char* message, unsigned int msglen, unsigned char* tag) {
	Mac(message,(int)msglen,tag);
}

void LetOracleVerifyTagAndMessage(unsigned char* message, unsigned int msglen, unsigned char* tag) {
	if (Vrfy(message,(int)msglen, tag)) {
		printf("Message verified successfully!\n");
	} else {
		printf("Message verficiation failed.\n");
	}
}

int main(int argc, char *argv[]) {
	cout << "MOOC Cryptography - PA4" << endl;
	string attack_msg = "I, the server, hereby agree that I will pay $100 to this student";
	unsigned char message[MAX_LENGTH];
	unsigned char tag[BLOCK_LENGTH];
	unsigned int  msglen = attack_msg.size();
	memset(message,0,MAX_LENGTH);

	Oracle_Connect();

	//send the first two blocks to the MAC oracle and retrieve the intermediate tag (for 2 blocks)
	//in the 4-blocks CBC-MAC chain
	for(unsigned int i=0;i<2*BLOCK_LENGTH;i++) message[i] = attack_msg[i];
	LetOracleTagMessage(message,2*BLOCK_LENGTH,tag);
	//the XOR of the third message block with the intermediate tag gives the first block
	//of the next 2-block message. Sending tht next 2-block messaged to the MAC oracle yields
	//the final forged tag
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) message[i] = tag[i]^attack_msg[2*BLOCK_LENGTH+i];
	//the second block of the next 2-block message is the 4th block of the text:
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) message[BLOCK_LENGTH+i] = attack_msg[3*BLOCK_LENGTH+i];
	//sending this 2-block message to the oracle, we obtain the forged tag
	LetOracleTagMessage(message,2*BLOCK_LENGTH,tag);
	//sending that tag along with the attack message should becverified with success
	LetOracleVerifyTagAndMessage((unsigned char*)attack_msg.c_str(),msglen,tag);

	Oracle_Disconnect();

	PrintOutTag(tag);
	return 0;
}
