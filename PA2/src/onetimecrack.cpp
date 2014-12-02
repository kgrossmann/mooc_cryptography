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
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
//
using namespace std;
//
#define KEY_LENGTH 31
#define NO_OF_MESSAGES 7
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

//character gueesed by human reader: message (1-7), index(1-31),character
/*
static unsigned int number_of_human_guessed_plain_characters = 8;
static string human_guessed_plain_characters[] = {"1", "1","I",
						  "1", "9","n",
						  "1","18","e",
						  "1","21","e",
						  "1","30","n",
						  "1", "31",".",
						  "2","11","o",
						  "5", "7","k"};
*/
//
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
void FormatIntegerToBinary(unsigned int number,unsigned int digits,char* buf) {
	unsigned int n = number & ((unsigned int)pow(2,digits)-1);
	for(unsigned int i=0;i<digits;i++) {
		unsigned int shift = digits-i-1;
		if(n>>shift==1) {
			buf[i] = '1';
		} else {
			buf[i] = '0';
		}
		//getting rid of the lefmost non-zero digit by shifting left and right again:
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
		FormatIntegerToBinary(xorresult,8,binarystring);
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
unsigned int WhichOneIsTheSpaceCharacter(unsigned char char1,unsigned int message1Idx,unsigned char char2,unsigned int message2Idx,unsigned int letterIdx) {
	//the one which is letter and is paired at least once which another message
	//will yield at least once a LETTER_AND_LETTER_PREFIX; if so, the the other
	//character is the space character
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		if(c!=message1Idx) {
			unsigned char other_char = ExtractCharFromCipherMessage(c,letterIdx);
			if(GetPairingType(char1,other_char)==LETTER_LETTER) {
				//the first char is the letter character, hence the second one is the space
				return 2;
			}
		}
	}
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		if(c!=message2Idx) {
			unsigned char other_char = ExtractCharFromCipherMessage(c,letterIdx);
			if(GetPairingType(char2,other_char)==LETTER_LETTER) {
				//the second char is the letter character, hence the first one is the space
				return 1;
			}
		}
	}
	return 0;
}
//
void FillInKeyGapsByHumanGuessing(unsigned int message1Idx,unsigned int letterIdx,unsigned char plain_char) {
	//for(unsigned int i=0;i<number_of_human_guessed_plain_characters*3;i+=3) {
		//unsigned int message1Idx = atoi (human_guessed_plain_characters[i].c_str())-1;
		//unsigned int letterIdx = atoi (human_guessed_plain_characters[i+1].c_str())-1;
		//unsigned char plain_char = human_guessed_plain_characters[i+2][0];
	    message1Idx--;
	    letterIdx--;
		unsigned char cipher_char = ExtractCharFromCipherMessage(message1Idx,letterIdx);
		unsigned char key_char = plain_char^cipher_char;
		//human-guessed characters overwrite the previously calcualted ones
		crackedkey[letterIdx] = key_char;
	//}
}
//
void DecryptAndPrint() {
	char msgbuf[10];
	std::cout << SECTION_SEPARATOR;
	for(unsigned int k=0;k<KEY_LENGTH;k++) {
		sprintf(msgbuf,"%X",crackedkey[k]);
		std::cout << msgbuf;
	}
	std::cout << "\n";
	for(unsigned int c=0;c<NO_OF_MESSAGES;c++) {
		std::cout << SECTION_SEPARATOR;
		std::cout << c+1 << " : ";
		for(unsigned int i=0;i<KEY_LENGTH;i++) {
			unsigned char cipher_char = ExtractCharFromCipherMessage(c,i);
			unsigned char key_char = crackedkey[i];
			unsigned char plain_char = cipher_char^key_char;
			sprintf(msgbuf,"%c",plain_char);
			std::cout << msgbuf;
		}
		std::cout << "\n";
		std::cout << "    "<< "1234567890123456789012345678901\n";
	}
	std::cout << SECTION_SEPARATOR;
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
						unsigned int iSpaceLetter = WhichOneIsTheSpaceCharacter(char1,c1,char2,c2,i);
						if(iSpaceLetter==1)  {
							unsigned char key = char1^SPACE_IN_DECIMAL;
							crackedkey[i] = key;
						} else if(iSpaceLetter==2)  {
							unsigned char key = char2^SPACE_IN_DECIMAL;
							crackedkey[i] = key;
						}
					}
				}
			}
		}
	}
	//
	unsigned int messageIdx;
	unsigned int letterIdx;
	unsigned char plain_char;
	while(1) {
		DecryptAndPrint();
		std::cout << "Enter your plain character guess as 'message,postion,character':\n";
		scanf("%d,%d,%c",&messageIdx,&letterIdx,&plain_char);
		if(messageIdx==0) break;
		if(messageIdx<=7 && letterIdx>=1 && letterIdx<=31) {
			FillInKeyGapsByHumanGuessing(messageIdx,letterIdx,plain_char);
		}
	}
	std::cout << "Congrats and Good Bye!\n";
	//
	return 0;
}

