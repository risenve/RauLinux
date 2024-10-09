#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>


void initialize(int argc, char** argv)
{
    if (argc != 2){
      std::cerr<<"invalid arguments "<<std::endl;
      exit(1);
    }

  int fd = open(argv[1], O_RDONLY);
  if(fd == -1){
    std::cerr<<"error, cant open the file"<<std::endl;
    exit(2);
  }

  dup2(fd, 0);
  close(fd);
}

int main(int argc, char** argv)
{
 
  initialize(argc, argv);

  std::string input;
  std::cin >> input;
  
  std::string reversed(input.rbegin(), input.rend());
  
  std::cout << reversed <<std::endl;

  return 0;
}
