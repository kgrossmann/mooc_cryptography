//============================================================================
// Name        : onetimecrack.cpp
// Author      : Klemens Grossmann
// Version     : V 2.0
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
#include <cmath>
//
using namespace std;
//
#define KEY_CHAR_RANGE_LOWER_BOUND 0
#define KEY_CHAR_RANGE_UPPER_BOUND 255
#define KEY_LENGTH 31
#define NO_OF_MESSAGES 7
#define PFILE "ptext.txt"
#define SECTION_SEPARATOR "--------------------------------------------------------------------------------------\n"
#define SPACE_AND_LETTER_PREFIX "01"
#define LETTER_AND_LETTER_PREFIX "00"
#define SPACE_IN_DECIMAL 32
#define SECTION_SEPARATOR "--------------------------------------------------------------------------------------\n"
//
enum PAIRING_TYPE {SPACE_LETTER, LETTER_LETTER, IDENTICAL};
static string cipher_messages[NO_OF_MESSAGES] = {"BB3A65F6F0034FA957F6A767699CE7FABA855AFB4F2B520AEAD612944A801E",
						"BA7F24F2A35357A05CB8A16762C5A6AAAC924AE6447F0608A3D11388569A1E",
						"A67261BBB30651BA5CF6BA297ED0E7B4E9894AA95E300247F0C0028F409A1E",
						"A57261F5F0004BA74CF4AA2979D9A6B7AC854DA95E305203EC8515954C9D0F",
						"BB3A70F3B91D48E84DF0AB702ECFEEB5BC8C5DA94C301E0BECD241954C831E",
						"A6726DE8F01A50E849EDBC6C7C9CF2B2A88E19FD423E0647ECCB04DD4C9D1E",
						"BC7570BBBF1D46E85AF9AA6C7A9CEFA9E9825CFD5E3A0047F7CD009305A71E"};

static unsigned char crackedkey[KEY_LENGTH];
//
unsigned char ExtractCharFromCipherMessage(unsigned int messsageIdx, unsigned int letterIdx) {
	unsigned char character;
	unsigned int position = (letterIdx+1)*2-2; //zero-based index and position
	string hexcode =  cipher_messages[messsageIdx].substr(position,2);
	sscanf(hexcode.c_str(),"%2hhX",&character);
	return character;
}
//
void FormatToBinary(unsigned int number,unsigned int digits,char* buf ) {
	unsigned int n = number & ((unsigned int)pow(2,digits)-1);
	for(int i=0;i<digits;i++) {
		int shift =	digits-i-1;
		if(n>>shift==1) {
			buf[i] = '1';
		} else {
			buf[i] = '0';
		}
		n = n<<(shift+1);
		n = n>>(shift+1);
	}
	buf[digits] = '\0';
}
//
PAIRING_TYPE GetPairingType(unsigned char char1,unsigned char char2) {
	PAIRING_TYPE pairingtype = LETTER_LETTER;
	unsigned char xorresult = char1^char2;
	if(xorresult==0) {
		pairingtype = IDENTICAL;
	} else {
		char binarystring[10];
		sprintf(binarystring,"%b",xorresult);
		FormatToBinary(xorresult,8,binarystring);
		//itoa(xorresult,binarystring,2);
		string prefix = binarystring;
		prefix = prefix.substr(0,2);
		if(prefix==SPACE_AND_LETTER_PREFIX) {
			pairingtype = SPACE_LETTER;
		} else if (prefix==LETTER_AND_LETTER_PREFIX) {
			pairingtype = LETTER_LETTER;
		}
	}
	return pairingtype;
}
//
unsigned int WhichOneIsLetter(unsigned char char1,unsigned int message1Idx,unsigned char char2,unsigned int message2Idx,unsigned int letterIdx) {
	//the one which is letter and is paired at leat once which another message
	//will get at least once LETTER_AND_LETTER_PREFIX
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		if(c!=message1Idx) {
			unsigned char other_char = ExtractCharFromCipherMessage(c,letterIdx);
			if(GetPairingType(char1,other_char)==LETTER_LETTER) {
				return 1;
			}
		}
	}
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		if(c!=message2Idx) {
			unsigned char other_char = ExtractCharFromCipherMessage(c,letterIdx);
			if(GetPairingType(char2,other_char)==LETTER_LETTER) {
				return 2;
			}
		}
	}
	return 0;
}
//
void DecryptAndPrint() {
	char msgbuf[10];
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		std::cout << SECTION_SEPARATOR;
		for(unsigned int i=0;i<KEY_LENGTH;i++) {
			unsigned char cipher_char = ExtractCharFromCipherMessage(c,i);
			unsigned char key_char = crackedkey[i];
			unsigned char plain_char = cipher_char^key_char;
			sprintf(msgbuf,"%c",plain_char);
			std::cout << msgbuf;
		}
		std::cout << "\n";
	}
}
//
int main() {

	//
	memset(crackedkey,0,KEY_LENGTH);
	for(unsigned int i=0;i<KEY_LENGTH;i++) {
		for(unsigned int c1=0;c1<NO_OF_MESSAGES;c1++) {
			for(unsigned int c2=0;c2<NO_OF_MESSAGES;c2++) {
				if(c1!=c2) {
					unsigned char char1 = ExtractCharFromCipherMessage(c1,i);
					unsigned char char2 = ExtractCharFromCipherMessage(c2,i);
					PAIRING_TYPE pairingtype = GetPairingType(char1,char2);
					if(pairingtype==SPACE_LETTER) {
						unsigned int iLetter = WhichOneIsLetter(char1,c1,char2,c2,i);
						if(iLetter==1)  {
							unsigned char key = char2^SPACE_IN_DECIMAL;
							crackedkey[i] = key;
						} else if(iLetter==2)  {
							unsigned char key = char1^SPACE_IN_DECIMAL;
							crackedkey[i] = key;
						}
					}
				}
			}
		}
	}
	DecryptAndPrint();
	//

	return 0;
}
