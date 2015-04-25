//-----------------------------------------------------------------------------------------
//Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
//-----------------------------------------------------------------------------------------

// CalcHash.cpp : additional code, that allows us to calculate function hash by its name.

#define _CRT_SECURE_NO_WARNINGS //Some stuff to make Visual Studio not to go crazy about "scanf" function.

#include "stdafx.h" //Includes StdAfx.h header file.

int main(int argc, char* argv[]) {
	char szFunctionName[70]; //String that contains function name.
	unsigned long dwNameHash = 0; //Calculated hash of function name.

	printf("Enter an API Function Name: ");
	scanf("%s", &szFunctionName); //Reads input string.
	
	//Beginning of assembler code, that detects function hash.
	__asm {
		lea esi, szFunctionName
			xor eax, eax

		_CalcHash :
		ror eax, 7
			xor dwNameHash, eax
			lodsb
			test al, al
			jnz _CalcHash
	}

	printf("Function Hash: %X\n", dwNameHash); //Printing function hash.
	system("pause");
	return 0;
}