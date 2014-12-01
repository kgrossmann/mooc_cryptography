//============================================================================
// Name        : paddingoracleattack.cpp

// Author      : Klemens Grossmann
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
//
#include "oracle.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
//
#define BLOCK_LEN 16
#define CIPHER_CHALLENGE_FILE "challenge-ciphertext.txt"
using namespace std;
static unsigned char iv_block[BLOCK_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned char c1_block[BLOCK_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned char c2_block[BLOCK_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned char plainttext[2*BLOCK_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Read a ciphertext from a file, send it to the server, and get back a result.
// If you run this using the challenge ciphertext (which was generated correctly),
// you should get back a 1. If you modify bytes of the challenge ciphertext, you
// may get a different result...
// Note that this code assumes a 3-block ciphertext.

int main(int argc, char *argv[]) {

	 std::cout << "MOOC Cryptography PA3" << endl; // prints MOOC Cryptography

	 unsigned char ctext[3*BLOCK_LEN]; // allocate space for 48 bytes, i.e., 3 blocks
	 int i, tmp, ret;
	 FILE *fpIn;

	 fpIn = fopen(CIPHER_CHALLENGE_FILE, "r");

	 //the IV blick (co)
	 for(i=0; i<16; i++) {
	   fscanf(fpIn, "%02x", &tmp);
	   ctext[i] = tmp;
	   iv_block[i] = tmp;
	 }
	 //first ciphermessage block (c1)
	 for(i=0; i<16; i++) {
	 	 fscanf(fpIn, "%02x", &tmp);
	 	 ctext[16+i] = tmp;
	 	 c1_block[i] = tmp;
	 }
	 //second ciphermessage block (c2)
	 for(i=0; i<16; i++) {
	 	fscanf(fpIn, "%02x", &tmp);
	 	ctext[32+i] = tmp;
	 	c2_block[i] = tmp;
	 }
	 fclose(fpIn);
	 //
	 Oracle_Connect();
	 //figure out the message length of the last message
	 unsigned int lastmsglen = 0;
	 for(unsigned char i=0;i<BLOCK_LEN;i++) {
		 ctext[16+i] = 0x66;
		 ret = Oracle_Send(ctext,3);
		 if(ret!=1) {
			 lastmsglen = i;
			 break;
		 }
	 }
	 printf("last message length: %d\n",lastmsglen);
	 //restore the c1 block
	 for(unsigned char i=0;i<BLOCK_LEN;i++) {
		 ctext[16+i] = c1_block[i];
	 }
	 //
	 for(unsigned char pad_len=BLOCK_LEN-lastmsglen+1;pad_len<=BLOCK_LEN;pad_len++) {
		 for(int i=BLOCK_LEN-1;i>=BLOCK_LEN-pad_len;i--) {
		     ctext[16+i] = ctext[16+i]^(pad_len)^(pad_len-1);
		 }
		 int index = BLOCK_LEN-pad_len;
		 for(unsigned int b=0;b<255;b++) {
			 ctext[16+index] = b;
			 //we have filled in all the field of the ciphemessage, if we send it and get not error
			 //we know we have chosen the correct c1 value
			 ret = Oracle_Send(ctext,3);
			 if(ret==1) {
				 //hooray, ctext[16+index] now has the correct value s.t.
				 //m_i XOR c_o_i XOR c_o_i' = padding_len; from that, we
				 //obtain the plaintext m_i
				 plainttext[index+16] = pad_len^ctext[16+index]^c1_block[index];
				 printf("%d : Oracle returned: %d : %c\n",pad_len,ret,plainttext[index+16]);
				 break;
			 }
		 }
	 }
	 //restore the cb1_block
	 for(unsigned char i=0;i<BLOCK_LEN;i++) {
	     ctext[16+i] = c1_block[i];
	 }
	 //
	 for(unsigned char pad_len=1;pad_len<=BLOCK_LEN;pad_len++) {
	 		 for(int i=BLOCK_LEN-1;i>=BLOCK_LEN-pad_len;i--) {
	 		     ctext[i] = ctext[i]^(pad_len)^(pad_len-1);
	 		 }
	 		 int index = BLOCK_LEN-pad_len;
	 		 for(unsigned int b=0;b<255;b++) {
	 			 ctext[index] = b;
	 			 ret = Oracle_Send(ctext,2);
	 			 if(ret==1) {
	 				//hooray, ctext[index] now has the correct value s.t.
	 				//m_i XOR c_o_i XOR c_o_i' = padding_len; from that, we
	 				//obtain the plaintext m_i
	 				plainttext[index] = pad_len^ctext[index]^iv_block[index];
	 				printf("%d : Oracle returned: %d : %c\n",pad_len,ret,plainttext[index]);
	 				break;
	 			 }
	 		 }
	 }
	 Oracle_Disconnect();
	 //print out the plaintext
	 for(unsigned int i=0;i<2*BLOCK_LEN;i++) {
	 	printf("%c",plainttext[i]);
	 }
	 printf("\n");
	 return 0;
}

