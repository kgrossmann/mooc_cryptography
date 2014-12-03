#include "oracle.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#define MAX_LENGTH 		512
#define BLOCK_LENGTH 	16
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
	Mac(message,(int)msglen,tag);
}
//
void LetOracleVerifyTagAndMessage(unsigned char* message, unsigned int msglen, unsigned char* tag) {
	if (Vrfy(message,(int)msglen, tag)) {
		printf("Message verified successfully!\n");
	} else {
		printf("Message verficiation failed.\n");
	}
}
//
int main(int argc, char *argv[]) {
	cout << "MOOC Cryptography - PA4" << endl;
	unsigned char message[MAX_LENGTH];
	unsigned char taggedmessage[MAX_LENGTH];
	unsigned char tag[BLOCK_LENGTH];
	unsigned int  msglen = attack_msg.size();

	Oracle_Connect();

	memset(message,0,MAX_LENGTH);
	memset(taggedmessage,0,MAX_LENGTH);
	for(unsigned int i=0;i<msglen;i++) 	message[i] = attack_msg[i];
	//send the first two blocks to the MAC oracle and retrieve the intermediate tag in the
	//CBC-MAC chain
	for(unsigned int i=0;i<2*BLOCK_LENGTH;i++) taggedmessage[i] = message[i];
	LetOracleTagMessage(taggedmessage,2*BLOCK_LENGTH,tag);
	//the XOR of the thirst message block with this intermediate tag gives the first block
	//of the next 2-block message to send to the MAC oracle for retrieving the final forged tag
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) taggedmessage[i] = tag[i]^message[2*BLOCK_LENGTH+i];
	//the second block of the next 2-block message to send is the 4th block of the text
	for(unsigned int i=0;i<BLOCK_LENGTH;i++) taggedmessage[BLOCK_LENGTH+i] = message[3*BLOCK_LENGTH+i];
	//sending this 2-block message, with the fist input message being
	//the calculated input to the third block cipher in the chains, gives the
	//tag for attack message
	LetOracleTagMessage(taggedmessage,2*BLOCK_LENGTH,tag);
	//sending that tag along with the attack message should verify correctly
	LetOracleVerifyTagAndMessage(message,msglen,tag);
	//
	Oracle_Disconnect();
	//
	PrintOutTag(tag);
	return 0;
}
