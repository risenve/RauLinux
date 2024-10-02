#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv){
  if(argc!= 2){
    std::cerr <<"invalid arguments" << std::endl;
    return 1;
  }
  int file_desc = open(argv[1],O_RDWR);
  if(file_desc == -1){
    std::cerr<<"error opening file" << std::endl;
    return errno;
  }
  
  const int buffer_size = 1024;
  char buffer[buffer_size];
  ssize_t bytes_read = read(file_desc, buffer, buffer_size);

  if(bytes_read == -1)
    {
      std::cerr<<"error reading file"<<std::endl;
      return errno;
    }

  while((bytes_read = read(file_desc, buffer, buffer_size)) > 0)
    {
      buffer[bytes_read] = '\0';
      lseek(file_desc, -bytes_read, SEEK_CUR);
      
      ssize_t bytes_written = write(file_desc, "\0", bytes_read);
      if (bytes_written == -1) {
        std::cerr << "Error writing to file" << std::endl;
        close(file_desc);
        return errno;
      }
    }

  close(file_desc);

  if(unlink(argv[1]) != 0)
    {
      std::cerr<<"error - cant delete file"<<std::endl;
      return errno;
    }
  
  std::cout<<"file is deleted"<<std::endl;
  return 0;
}
