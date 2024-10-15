#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv){

  if(argc!=3){
    std::cerr<<"invalid arguments "<<std::endl;
    return errno;
  }

  int fd_source = open(argv[1], O_RDONLY);
  if(fd_source==-1){
    std::cerr<<"cant open source file "<<std::endl;
    return errno;
  }
  
  int fd_dest = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd_dest==-1){  
    std::cerr<<"cant open destination file "<<std::endl;
    return errno;
  }

  const size_t bytes_count = 1024;
  char buffer[bytes_count];

  int bytes_read, bytes_write;
  int bytes_copy = 0, holes_count = 0;

  while((bytes_read = read(fd_source, buffer, bytes_count)) > 0){
    int holes = 0;
    for(int i = 0; i< bytes_read; i++){
      
      if(buffer[i] == 0){
        holes++;
      }

      else{
        bytes_write = write(fd_dest, &buffer[i],1);
        if(bytes_write<0){
          std::cerr<< "error, cant write to destination file"<<std::endl;
          close (fd_source);
          close (fd_dest);

          return errno;
        }//if
        bytes_copy ++;
      }//else
    }//for
    holes_count += holes;
  }//while


  //Successfully copied X bytes (data: M, hole: N).
  std::cout << "Successfully copied " << bytes_copy << " bytes (data: " << bytes_copy - holes_count << " hole: " << holes_count << ")" << std::endl;

  close (fd_source);
  close (fd_dest);
  return 0;
}
