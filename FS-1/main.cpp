#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

int main(int argc, char** argv)
{
	if (argc==2)
	{
		int file_desc = open(argv[1], O_RDONLY);
		if(file_desc == -1)
		{
			std::cout<<"error opening file"<<std::endl;
			return errno;
		}
		const int size = 256;
		char BUF[size];
		int count_bytes;
		while((count_bytes = read(file_desc, BUF, size)) > 0)
		{
			std::cout.write(BUF, count_bytes);
		}
		if(count_bytes == -1)
		{
			std::cout<<"error reading file"<<std::endl;
			close(file_desc);
			return errno;
		}
		close(file_desc);
		return 0;
	}
	else
	{
		std::cout<<"invalidddd argument"<< std::endl;
		return 1;
	}
}
