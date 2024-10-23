#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>

// Напишите программу на языке C++ под названием «do-command», которая принимает N аргументов в качестве входных данных.
// Предположим, что первый аргумент — это имя команды, а остальные — аргументы для данной команды.
// Программа должна содержать специальную функцию do_command(char** argv), где argv — вектор аргументов, завершающийся нулем.
// Функция «do_command» должна создать новый процесс, выполнить указанную команду с указанными аргументами и дождаться ее завершения.
// После завершения выполнения программы функция do_function должна вывести статус завершения команды и продолжительность выполнения команды.
// «Основная» функция программы отвечает за построение правильного вектора аргументов и использование функции do_command для выполнения команды.

void do_command(char** argv){

  int status = -1;
  auto start_time = std::chrono::high_resolution_clock::now();
  int pid_child = fork();

  if(pid_child == -1){
      std::cerr << "Error something went wrog " << std::endl;
      return ;
  }
  else if(pid_child == 0){
    execvp(argv[0],argv);
    std::cout << "Error when executing the command: " << getpid() << std::endl;
    exit(errno);
  }
  else{
    
    if(waitpid(pid_child, &status, 0)==-1){
      std::cout << "Child waiting process error: " << getpid() << std::endl;
      exit(errno);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;  
    std::cout << "Command completed with: " << status << "code and took " << duration.count() << " seconds" << std::endl;
  }
  
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Something went wrong, provide arguments: " << std::endl;
        return 1;
    }

  char** arguments = new char*[argc];
  for (int i = 0; i < argc-1; i++) {
     arguments[i+1] = argv[i];
  }
  
  arguments[argc-1] = nullptr;

  do_command(arguments);
  delete[] arguments;
  
  return 0;
}
