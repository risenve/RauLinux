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

  int fd1 = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC , 0644);
  if(fd1 == -1){
  std::cerr<<"error, cant open the file"<<std::endl;
  exit(2);
  }

  write(fd1, "first line\n", 11);
  int fd2 = dup(fd1);
  
  write(fd2, "second line\n", 12);
  close(fd1);
  close(fd2);
  
  return 0;
}
