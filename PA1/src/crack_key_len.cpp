//============================================================================
// Name        : crack_key_len.cpp
// Author      : Klemens Gro?mann
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//
using namespace std;
//
#define ENGLISH_LETTER_FREQUENCY 0.065
//
static unsigned int letters_total = 0;
static std::map<unsigned char,int>* histogram = new std::map<unsigned char,int>;  //node ID and index in current cycle
//
float CalcDistributinScore() {
   float fdistributinScore = 0;
   if(histogram->size()) {
	   for (std::map<unsigned char,int>::iterator it=histogram->begin(); it!=histogram->end();it++) {
		   float real_probabilty = it->second/(float)letters_total;
		   fdistributinScore += real_probabilty*real_probabilty;

	   }
   }
   return (fdistributinScore);
}

int main() {
	unsigned char ch;
	float fbestScore = 0.0;
	float fbestDiff = 1.0;
    FILE *fcIn, *fpOut;
	int i;
	int bestkeylen = 1;
	int lextlen = 0;
	fcIn = fopen("ctext.txt","r");
	while (fscanf(fcIn,"%2hhX",&ch)!=EOF) {
		lextlen++;
	}
	fclose(fcIn);
	std::cout << "text len " << lextlen << "\n";
	//
	int keylen = 1;
	while(keylen<=lextlen)
	{
		fcIn = fopen("ctext.txt","r");
		i=0;
		histogram->clear();
		for(unsigned int b=0;b<=255;b++) {
			histogram->insert(pair<unsigned char,int>((char)b,0));
		}
		letters_total = 0;
		while (fscanf(fcIn,"%2hhX",&ch)!=EOF) {
			i++;
			if(((i-1)%keylen)==0) {
				(*histogram)[ch]++;
				letters_total++;
			}
		}
		float fScore = CalcDistributinScore();
		float fdiff = ENGLISH_LETTER_FREQUENCY-fScore>= 0.0 ? ENGLISH_LETTER_FREQUENCY-fScore : fScore-ENGLISH_LETTER_FREQUENCY;
		std::cout << "score " << fScore << " diff " << fdiff << " keylen = " << keylen << "\n";
		if(fdiff<=fbestDiff) {
			if (fScore>fbestScore) {
		    	fbestDiff = fdiff;
		    	fbestScore = fScore;
		    	bestkeylen = keylen;
		    	std::cout << " best score " << fbestScore << " keylen = " << bestkeylen << "\n";
		    }
		}
		keylen++;
		fclose(fcIn);
	}
	//
	std::cout << " best score " << fbestScore << " keylen = " << bestkeylen << "\n";
	return 0;
}
