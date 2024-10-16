#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

//FS-11: Напишите программу для копирования определенного количества 
// байтов из одного файла в другой, при этом разрешите пользователю указать
// начальное смещение в исходном файле и начальное смещение в файле  
// назначения. Используйте системные вызовы open, read, write и lseek.


int main(int argc, char** argv){

  if(argc!=6){
    std::cerr<<"invalid arguments "<<std::endl;
    return errno;
  }

  int fd_source = open(argv[1], O_RDONLY);
  if(fd_source==-1){
    std::cerr<<"cant open source file "<<std::endl;
    return errno;
  }

  int fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd_dest==-1){  
    std::cerr<<"cant open destination file "<<std::endl;
    return errno;
  }

  const size_t bytes_count = 1024;
  char buffer[bytes_count];

  int bytes_read, bytes_write;
  int bytes_copy = 0, holes_count = 0;

  int input_offset_source = atoi(argv[3]);
  int input_offset_dest = atoi(argv[4]);

  if(lseek(fd_source, input_offset_source, SEEK_SET)==-1){
    std::cerr << "error seeking source file" << std::endl;
    close(fd_source);
    close(fd_dest);
    return errno;
  }
  if(lseek(fd_dest, input_offset_dest, SEEK_SET)==-1){
    std::cerr << "error seeking destination file" << std::endl;
    close(fd_source);
    close(fd_dest);
    return errno;
  }
  
  //int sorce_offset = lseek(fd_source, input_offset_source, SEEK_SET);
  //int destination_offset = lseek(fd_dest, input_offset_dest, SEEK_SET);

  
  
  //copy algorithm
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
