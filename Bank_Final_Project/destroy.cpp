#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "bank_structures.h"

int main() {
    shm_unlink("/bank");
    std::cout << "Bank destroyed successfully" << std::endl;
    return 0;
}
