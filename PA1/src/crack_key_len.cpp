//============================================================================
// Name        : crack_key_len.cpp
// Author      : Klemens Grossmann
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
#define CHAR_RANGE_LOWER_BOUND 0
#define CHAR_RANGE_UPPER_BOUND 255
#define ENGLISH_LETTER_FREQUENCY 0.065
#define CFILE "ctext.txt"
//
float CalcDistributinScore(std::map<unsigned int,unsigned int>* histogram, unsigned int letterstotal) {
   float fdistributinScore = 0;
   if(histogram->size()) {
	   for (std::map<unsigned int,unsigned int>::iterator it=histogram->begin(); it!=histogram->end();it++) {
		   float real_probabilty = it->second/(float)letterstotal;
		   fdistributinScore += real_probabilty*real_probabilty;
	   }
   }
   return (fdistributinScore);
}
//
unsigned int DetermineTextLegnth() {
	FILE* fcIn = fopen(CFILE,"r");
	unsigned int textlen = 0;
	unsigned char ch;
	while (fscanf(fcIn,"%2hhX",&ch)!=EOF) {
		textlen++;
	}
	fclose(fcIn);
	return textlen;
}
//
std::map<unsigned int,unsigned int>* CreateInitializeHistogram() {
	std::map<unsigned int,unsigned int>* histogram = new std::map<unsigned int,unsigned int>;
	for(unsigned int i=CHAR_RANGE_LOWER_BOUND;i<=CHAR_RANGE_UPPER_BOUND;i++) {
		histogram->insert(pair<unsigned int,unsigned int>(i,0));
	}
	return (histogram);
}
//
int main() {

	int lextlen = DetermineTextLegnth();
	std::cout << "text length : " << lextlen << "\n";
	//
	float fbestScore = 0.0;
	float fbestDiff = 1.0;
	unsigned int bestkeylen = 1;
	unsigned int keylen = 1;
	while(keylen<=lextlen)
	{
		FILE* fcIn = fopen("ctext.txt","r");
		unsigned int letterstotal = 0;
		std::map<unsigned int,unsigned int>* histogram = CreateInitializeHistogram();
		unsigned int i = 0;
		unsigned char ch;
		while (fscanf(fcIn,"%2hhX",&ch)!=EOF) {
			i++;
			if(((i-1)%keylen)==0) {
				(*histogram)[ch]++;
				letterstotal++;
			}
		}
		float fScore = CalcDistributinScore(histogram,letterstotal);
		delete  histogram;
		float fdiff = ENGLISH_LETTER_FREQUENCY-fScore>= 0.0 ? ENGLISH_LETTER_FREQUENCY-fScore : fScore-ENGLISH_LETTER_FREQUENCY;
		if(fdiff<=fbestDiff && fScore>fbestScore) {
			fbestDiff = fdiff;
		    fbestScore = fScore;
		    bestkeylen = keylen;
		    std::cout << " better score " << fbestScore << " for keylen = " << bestkeylen << "\n";
		}
		keylen++;
		fclose(fcIn);
	}
	//
	std::cout << " best score " << fbestScore << " at keylen = " << bestkeylen << "\n";
	return 0;
}
