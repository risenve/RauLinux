#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "invalid arguments" << std::endl;
    return 1;
  }

  int source_file = open(argv[1], O_RDONLY);
  if (source_file == -1) {
    std::cout << "error opening file" << std::endl;
    return errno;
  }

  int new_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (new_file == -1) {
    std::cout << "error opening new file" << std::endl;
    close(source_file);
    return errno;
  }

  const int BUF_SIZE = 256;
  char buffer[BUF_SIZE];
  ssize_t bytes_read;

   bytes_read = read(source_file, buffer, sizeof(buffer) - 1);
  while (bytes_read > 0) {
    buffer[bytes_read] = '\0'; 
    ssize_t bytes_written = write(new_file, buffer, bytes_read);
    if (bytes_written != bytes_read) {
      std::cerr << "error writing to new file" << std::endl;
      close(source_file);
      close(new_file);
      return errno;
    }
    bytes_read = read(source_file, buffer, sizeof(buffer) - 1);
  }

  if (bytes_read == -1) {
    std::cerr << "error reading from source file" << std::endl;
    close(source_file);
    close(new_file);
    return errno;
  }

  close(source_file);
  close(new_file);

  std::cout << "the new file has been copied" << std::endl;
  return 0;
}
