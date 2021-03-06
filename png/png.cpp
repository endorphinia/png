// png.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <conio.h>

using namespace std;

bool equals(char* first, char* second, int size) 
{
	for (int i = 0; i < size; ++i)
	{
		if (first[i] != second[i])
			return false;
	}
	return true;
}

const char* get_name(char* name) 
{
	char ihdr[4] = { 0x49, 0x48, 0x44, 0x52 };
	char idat[4] = { 0x49, 0x44, 0x41, 0x54 };
	char iend[4] = { 0x49, 0x45, 0x4E, 0x44 };
	char plte[4] = { 0x50, 0x4C, 0x54, 0x45 };

	if (equals(name, ihdr, 4))
		return "IHDR";

	if (equals(name, idat, 4))
		return "IDAT";

	if (equals(name, iend, 4))
		return "IEND";

	if (equals(name, plte, 4))
		return "PLTE";

	return "NULL";
}

bool check_signature(char* signature)
{
	char png_signature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	return equals(signature, png_signature, 8);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "Error!";
		_getch();
		return 0;
	}

	ifstream file(argv[1], ios::binary);
	if (!file.is_open())
	{
		cout << "File isn't found!";
		_getch();
		return 0;
	}

	char signature[8];
	file.read(signature, 8);
	if (!check_signature(signature))
	{
		cout << "It isn't a png-file!";
		_getch();
		return 0;
	}

	char header_data_length_raw[4];
	file.read(header_data_length_raw, 4);
	int header_data_length = _byteswap_ulong(*((unsigned int*)header_data_length_raw));
	if (header_data_length != 13)
	{
		cout << "Header data lenght != 13";
		_getch();
		return 0;
	}

	char header_name[4];
	file.read(header_name, 4);
	if (strcmp(get_name(header_name), "IHDR") != 0)
	{
		cout << "Header name != IHDR";
		_getch();
		return 0;
	}

	//пропустили data и crc-блоки в header

	file.seekg(header_data_length + 4, ios_base::cur);

	while(true)
	{
		long chunk_position = file.tellg();

		char chunk_data_length_raw[4];
		file.read(chunk_data_length_raw, 4);
		int chunk_data_length = _byteswap_ulong(*((unsigned int*)chunk_data_length_raw));
		char chunk_name[4];
		file.read(chunk_name, 4);

		file.seekg(chunk_data_length, ios_base::cur);

		char chunk_crc_raw[4];
		file.read(chunk_crc_raw, 4);
		int chunk_crc = _byteswap_ulong(*((unsigned int*)chunk_crc_raw));

		cout << "Chunk type: " << get_name(chunk_name)
			 << " Chunk position: " << chunk_position
			 << " Chunk data lenght: " << chunk_data_length
			 << " Chunk CRC: " << chunk_crc << endl;

		if (strcmp(get_name(chunk_name), "IEND") == 0)
			break;
	}

	_getch();
    return 0;
}

