#ifndef ZINNY_H
#define ZINNY_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#define ARCHIVE_NAME ".pot"

/*
readable + version + sha256_key + file_list + file_size_list + files
*/

class zinny 
{
private:
	uint size;
	std::string name;
	bool readable = true;
	std::vector<std::string> command;
	const int version = 1;

public:
	zinny(std::vector<std::string> command)
	{
		this->name = command[1] + ARCHIVE_NAME;	
		this->command = command;
	}

	bool packing()
	{
		std::ofstream out;
		out.open(name , std::ios::binary);

		out.write(reinterpret_cast<const char*>(&this->readable), sizeof(bool));
		out.write(reinterpret_cast<const char*>(&this->version), sizeof(int));
		uint tmp = this->command.size() - 2;
		out.write(reinterpret_cast<const char*>(&tmp), sizeof(uint));

		for(int i = 2; i < this->command.size() ; ++i)
		{
			std::ifstream in(this->command[i], std::ios::binary);
			//file_name
			out.write(this->command[i].c_str(), this->command[i].size());
			out.write("\0", sizeof(char));
			in.seekg(0, std::ios::end);
			this->size += in.tellg();
			std::cout << this->command[i] << "\t\t\tReaded : " << in.tellg() << std::endl;
			in.close();
		}

		for(int i = 2; i < this->command.size() ; ++i)
		{
			std::ifstream in(this->command[i], std::ios::binary);
			//file_size
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			out.write(reinterpret_cast<const char*>(&size), sizeof(std::size_t));
			in.close();
		}

		int buf_size = sizeof(this->readable) + size;
		char buf[buf_size];
		out.write(buf, buf_size);
		std::cout << this->name << " Packaged." << std::endl;
		return true;
	}

	void view()
	{
		auto file_list = std::vector<std::string>();
		std::ifstream in(this->command[1], std::ios::binary);

		bool readable;
		in.read(reinterpret_cast<char*>(&readable), sizeof(bool));
		std::cout << "Readable \t: " << readable << std::endl;

		int version;
		in.read(reinterpret_cast<char*>(&version), sizeof(int));
		std::cout << "Version \t: " << version << std::endl;

		uint size;
		in.read(reinterpret_cast<char*>(&size), sizeof(uint));
		std::cout << "Files \t\t: " << size << std::endl;

		auto filenames = std::vector<std::string>();
		for(int i = 0; i < size ; ++i)
		{
			std::string name;
			std::getline(in, name, '\0');
			filenames.push_back(name);
		}
		for(int i = 0; i < size ; ++i)
		{
			std::size_t ssize;
			in.read(reinterpret_cast<char*>(&ssize), sizeof(std::size_t));
			std::cout << "\tname : " << filenames[i] << std::endl;
			std::cout << "\t- " << ssize << " bytes" << std::endl;
			std::cout << std::endl;
		}
	}

	int open(std::string path)
	{
		if(path.empty()) return 0;

		std::ifstream fin;
		fin.open(path, std::ios::binary);
		fin.seekg(0, std::ios::end);

		int size = fin.tellg();

		fin.seekg(0, std::ios::beg);
		this->size = size;
		std::cout << "sucsess : " << this->size << " byte are readed!"<< std::endl;
		return size;
	}
};


#endif