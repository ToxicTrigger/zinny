#ifndef ZINNY_H
#define ZINNY_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>

#define ARCHIVE_NAME ".pot"

/*
readable + version + sha256_key + file_list + file_size_list + files
*/

class zinny
{
  public:
	int version;

  private:
	uint size;
	std::string name;
	bool readable;
	std::vector<std::string> command;

	uint skip(std::ifstream *in)
	{
		bool readable;
		in->read(reinterpret_cast<char *>(&readable), sizeof(bool));
		int version;
		in->read(reinterpret_cast<char *>(&version), sizeof(int));
		uint size;
		in->read(reinterpret_cast<char *>(&size), sizeof(uint));
		return size;
	}

  public:
	zinny(std::vector<std::string> command)
	{
		this->version = 1;
		this->readable = true;
		this->name = command[1] + ARCHIVE_NAME;
		this->command = command;
	}

	bool packing()
	{
		std::ofstream out;
		out.open(name, std::ios::binary);

		out.write(reinterpret_cast<const char *>(&this->readable), sizeof(bool));
		out.write(reinterpret_cast<const char *>(&this->version), sizeof(int));
		uint tmp = this->command.size() - 2;
		out.write(reinterpret_cast<const char *>(&tmp), sizeof(uint));

		for (int i = 2; i < this->command.size(); ++i)
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

		for (int i = 2; i < this->command.size(); ++i)
		{
			std::ifstream in(this->command[i], std::ios::binary);
			//file_size
			in.seekg(0, std::ios::end);
			uint size = in.tellg();
			out.write(reinterpret_cast<const char *>(&size), sizeof(uint));
			in.close();
		}

		for (int i = 2; i < this->command.size(); ++i)
		{
			std::ifstream in(this->command[i], std::ios::binary);
			out << in.rdbuf();
			std::cout << "\t- " << this->command[i] << " Packed" << std::endl;
			in.close();
		}

		out.close();
		std::cout << this->name << " Packaged." << std::endl;
		return true;
	}

	void view()
	{
		std::ifstream in(this->command[1], std::ios::binary);

		bool readable;
		in.read(reinterpret_cast<char *>(&readable), sizeof(bool));
		std::cout << "Readable \t: " << readable << std::endl;

		int version;
		in.read(reinterpret_cast<char *>(&version), sizeof(int));
		std::cout << "Version \t: " << version << std::endl;

		uint size;
		in.read(reinterpret_cast<char *>(&size), sizeof(uint));
		std::cout << "Files \t\t: " << size << std::endl;

		std::vector<std::string> filenames = std::vector<std::string>();
		for (int i = 0; i < size; ++i)
		{
			std::string name;
			std::getline(in, name, '\0');
			filenames.push_back(name);
		}
		for (int i = 0; i < size; ++i)
		{
			uint ssize;
			in.read(reinterpret_cast<char *>(&ssize), sizeof(uint));
			std::cout << "\tname : " << filenames[i] << std::endl;
			std::cout << "\t- " << ssize << " bytes" << std::endl;
			std::cout << std::endl;
		}
	}

	void unpack()
	{
		std::ifstream in(this->command[1], std::ios::binary);
		if(!in.is_open())
		{
			std::cout << "Faile" << std::endl;
		}
		std::string out_path;
		char dir[1000];
		getcwd(dir, 1000);
		out_path = dir;
		out_path += "/";

		uint size = this->skip(&in);

		//1. load file size
		std::vector<std::string> filenames = std::vector<std::string>();
		for (int i = 0; i < size; ++i)
		{
			std::string name;
			std::getline(in, name, '\0');
			filenames.push_back(name);
		}
		std::vector<uint> file_size = std::vector<uint>();
		for (int i = 0; i < size; ++i)
		{
			uint ssize;
			in.read(reinterpret_cast<char *>(&ssize), sizeof(uint));
			std::cout << "\tname : " << filenames[i] << std::endl;
			std::cout << "\t- " << ssize << " bytes" << std::endl;
			std::cout << std::endl;
			file_size.push_back(ssize);
		}

		std::cout << out_path << std::endl;

		for (int i = 0; i < size; ++i)
		{
			uint ssize = file_size[i];
			std::ofstream out(filenames[i].substr(filenames[i].rfind('/')+1), std::ios::binary);
			std::cout << "start: " << in.tellg()  << " size : " << ssize << std::endl;
			uint start = in.tellg();

			char *buf = new char[ssize];
			in.read(reinterpret_cast<char*>(buf), ssize);
			out.write(reinterpret_cast<const char*>(buf), ssize);
			
			out.close();
		}
	}

	int open(std::string path)
	{
		if (path.empty())
			return 0;

		std::ifstream fin;
		fin.open(path, std::ios::binary);
		fin.seekg(0, std::ios::end);

		int size = fin.tellg();

		fin.seekg(0, std::ios::beg);
		this->size = size;
		std::cout << "sucsess : " << this->size << " byte are readed!" << std::endl;
		return size;
	}
};

#endif