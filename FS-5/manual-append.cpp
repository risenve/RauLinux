#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
 
  if (argc != 2){
    std::cerr<<"invalid arguments "<<std::endl;
    exit(1);
  }

  int fd1 = open(argv[1], O_RDONLY);
  if(fd1 == -1){
  std::cerr<<"error, cant open the file"<<std::endl;
  exit(2);
  }

  int fd2 = dup(fd1);
  std::cout<<"first line\n";
  std::cout<<"second line\n";
  close(fd1);
  close(fd2);
  
  return 0;
}
