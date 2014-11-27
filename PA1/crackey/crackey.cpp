//============================================================================
// Name        : crack_key_len.cpp
// Author      : Klemens Gross	mann
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
//
using namespace std;

#define MAX_KEY_LENGTH 512
#define ENGLISH_LETTER_FREQUENCY 0.065
#define STREAM_TOLERANCE_AGAINST_ENGLISH_LETTER_FREQUENCY 0.10
#define CFILE "ctext.txt"
#define PFILE "ptext.txt"

typedef std::map<int,int> CHARLIST;
static char msgbuf[1024];
//
std::map<unsigned int,int>* CreateInitializeHistogram() {
	std::map<unsigned int,int>* histogram = new std::map<unsigned int,int>;
	for(unsigned int i=0;i<=255;i++) {
	   histogram->insert(pair<unsigned int,int>(i,0));
	}
	return histogram;
}
//
std::map<int,CHARLIST>* CreateInitializeKeyCharacters(unsigned int keylen) {
	std::map<int,CHARLIST>* keyscharacters = new std::map<int,CHARLIST>;
	for(int i=1;i<=keylen;i++) {
		CHARLIST charlist;
		keyscharacters->insert(pair<int,CHARLIST>(i,charlist));
	}
	return keyscharacters;
}
//
int SumUpPossibleKeys(std::map<int,CHARLIST>* keyscharacters)  {
	int iPossibleKeys = 0;
	if(keyscharacters->size()>0) {
		iPossibleKeys = 1;
		for (std::map<int,CHARLIST>::iterator it=keyscharacters->begin();it!=keyscharacters->end();it++) {
			CHARLIST keylist = it->second;
			iPossibleKeys *= keylist.size();
		}
	}
	return iPossibleKeys;
}
//
std::map<int,int>* CreateInitializePromisingKeyLength(unsigned int maxNumberOfKeys) {
	std::map<int,int>* promisingkeys = new std::map<int,int>;
	for(unsigned int k=1;k<=maxNumberOfKeys;k++) {
		promisingkeys->insert(pair<int,int>(k,0));
	}
	return promisingkeys;
}
//
float CalculateScoreFromHistogram(std::map<unsigned int,int>* histogram, unsigned int letters_total) {
   float fdistributinScore = 0.0;
   for (std::map<unsigned int,int>::iterator it=histogram->begin();it!=histogram->end();it++) {
	   if (it->second>0) {
		   if (it->first>=32 && it->first<=127
			  && (it->first != '~' && /*it->first != '{' && it->first != '}' &&*/ it->first != '#' && it->first != '`') //excluse some punctuations
			  && (isupper(it->first) || islower(it->first) || ispunct(it->first) || isspace(it->first))) {
			   float real_probabilty = it->second/(float)letters_total;
			   fdistributinScore += real_probabilty*real_probabilty;
		   }  else {
			   return 0.0;
		   }
	   }
   }
   return (fdistributinScore);
}
//
void DecryptByKeyAndPrint(std::map<int,int>* key) {
    FILE* cfile  = fopen(CFILE,"r");
    FILE* pfile  = fopen(PFILE,"w");
    if((pfile==NULL) || (cfile==NULL)) {
    	printf("File error!\n");
    	return;
    }
    unsigned int letters_total = 0;
    std::map<unsigned int,int>* histogram = CreateInitializeHistogram();
    std::cout << "-----------------------------------------------------------------\n";
   	sprintf(msgbuf,"%s","Ciphertext decrypted by key  ");
   	fprintf(pfile,"%s",msgbuf);
   	std::cout << msgbuf;
    for(int i=1;i<=key->size();i++)	{
		sprintf(msgbuf,"%02X",(*key)[i]);
		std::cout << msgbuf;
		fprintf(pfile,"%s",msgbuf);
    }
    sprintf(msgbuf,"%s","\n");
    std::cout << msgbuf;
    fprintf(pfile,"%s",msgbuf);
    //
    int i=0;
    unsigned char ch;
    unsigned int plainchar;
    unsigned int keylen = key->size();
    while (fscanf(cfile,"%2hhX",&ch)!=EOF) {
    	i++;
    	int keychar = (*key)[(i-1)%keylen+1];
    	plainchar = (int)ch^keychar;
    	sprintf(msgbuf,"%c",plainchar);
    	std::cout << msgbuf;
    	fprintf(pfile,"%s",msgbuf);
    	(*histogram)[plainchar]++;
        letters_total++;
    }
    sprintf(msgbuf,"%s","\n");
    std::cout << msgbuf;
    fprintf(pfile,"%s",msgbuf);

    float fdistributinScore = CalculateScoreFromHistogram(histogram,letters_total);
    delete histogram;

    sprintf(msgbuf,"Score : %f",fdistributinScore);
    std::cout << msgbuf;
    fprintf(pfile,"%s",msgbuf);
    sprintf(msgbuf,"%s","\n");
    std::cout << msgbuf;
    fprintf(pfile,"%s",msgbuf);
    fclose(pfile);
    fclose(cfile);
}
//
unsigned int IterateThroughPromisingKeysAndShowStats(std::map<int,int>* promisingkeys) {
	unsigned int iBestKeyIndex=0;
	float fbestScore = 0.0;
	std::cout << "-----------------------------------------------------------------\n";
	for (std::map<int,int>::iterator it=promisingkeys->begin();it!=promisingkeys->end();it++) {
		if(it->second>0) {
			iBestKeyIndex = it->first;
			std::cout << "promising keylen: " << it->first << ", number of possible keys: " << it->second << "\n";
		}
	}
	return iBestKeyIndex;
}
//
void IterateThroughKeysAndDecrypt(std::map<int,CHARLIST>* keyscharacters, unsigned int keylen,std::map<int,int>* characters_so_far) {
	unsigned int number_of_characters_so_far = characters_so_far->size();
	if(characters_so_far->size()==keylen) {
		DecryptByKeyAndPrint(characters_so_far);
		return;
	} else {
		CHARLIST charlist = (*keyscharacters)[number_of_characters_so_far+1];
		for (std::map<int,int>::iterator it=charlist.begin();it!=charlist.end();it++) {
			characters_so_far->insert(pair<int,int>(characters_so_far->size()+1,it->first));
			IterateThroughKeysAndDecrypt(keyscharacters,keylen,characters_so_far);
			characters_so_far->erase(characters_so_far->size());
		}
	}
}
//
void IterateThroughPromisingKeysAndDecrypt(std::map<int,int>* promisingkeys,std::map<int,CHARLIST>** keyscharacters) {
	for (std::map<int,int>::iterator it=promisingkeys->begin();it!=promisingkeys->end();it++) {
		if(it->second>0) {
			int keylen = it->first;
			std::map<int,CHARLIST>* charactersforkey = keyscharacters[keylen];
			std::map<int,int>* characters_so_far = new std::map<int,int>;
			IterateThroughKeysAndDecrypt(charactersforkey,keylen,characters_so_far);
			delete characters_so_far;
		}
	}
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
int main() {
	unsigned int textlen = DetermineTextLegnth();
	std::cout << "text length : " << textlen << "\n";
	//
	std::map<int,int>* promisingkeys = CreateInitializePromisingKeyLength(textlen);
	std::map<int,CHARLIST>* keyscharacters[MAX_KEY_LENGTH+1];
	//
	for(int keylength=1;keylength<=textlen;keylength++)
	{
		keyscharacters[keylength] = CreateInitializeKeyCharacters(keylength);
		int startIndex = 1;
		while(startIndex<=keylength)
		{
			//each iteration here goes through a i-stream of the key
			//characters in this stream are all decrypted by the same character in the key
			bool bAtLeastOneViableKeyFound = false;
			for(unsigned int keychar=0;keychar<=255;keychar++) {
				FILE* fcIn = fopen(CFILE,"r");
				unsigned int letters_total = 0;
				std::map<unsigned int,int>* histogram = CreateInitializeHistogram();
				int i=0;
				unsigned char ch;
				while (fscanf(fcIn,"%2hhX",&ch)!=EOF) {
					i++;
					if((i-startIndex)%keylength==0) {
						unsigned int plainchar = (int)ch^keychar;
						(*histogram)[plainchar]++;
						letters_total++;
					}
				}
				//CalculateScore() also weans out (return is 0.0 in that case)
				//when a chosen key character yields a character not in the plaintext
				float fScore = CalculateScoreFromHistogram(histogram,letters_total);
				delete histogram;
				if(fScore>0.0) {
					float fdiff = ENGLISH_LETTER_FREQUENCY-fScore>= 0.0 ? ENGLISH_LETTER_FREQUENCY-fScore : fScore-ENGLISH_LETTER_FREQUENCY;
					//we only take those into account which a certain tolerance against the English letter frequency
					if (fdiff<=STREAM_TOLERANCE_AGAINST_ENGLISH_LETTER_FREQUENCY) {
						bAtLeastOneViableKeyFound = true;
						(*keyscharacters[keylength])[startIndex].insert(pair<int,int>(keychar,1));
					}
				}
				fclose(fcIn);
			}
			if (!bAtLeastOneViableKeyFound) {
				std::cout << "keylen " << keylength << " is hopeless!" << "\n";
				keyscharacters[keylength]->clear();
				break;
			}
			else {
				startIndex++;
			}
		}
		(*promisingkeys)[keylength] = SumUpPossibleKeys(keyscharacters[keylength]);
	}
	//
	IterateThroughPromisingKeysAndShowStats(promisingkeys);
	IterateThroughPromisingKeysAndDecrypt(promisingkeys,keyscharacters);
	//
	std::cout << "-----------------------------------------------------------------\n";
	//tidy up the room before you leave:
	for(int keylength=1;keylength<=textlen;keylength++) {
		delete keyscharacters[keylength];
	}
	delete promisingkeys;
	return 0;
}
