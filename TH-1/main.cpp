#include <iostream>

#include <chrono>
#include <vector>
#include <cstdlib>
#include <numeric>

#include <pthread.h>

struct  my_threads{
    const std::vector<int>* arr;
    long long* sum;
    std::size_t start;
    std::size_t end;
};

template <typename Iterator>
long long MySum(Iterator start, Iterator end) {
    long long sum = 0;
    for (auto it = start; it != end; ++it) {
        sum += *it;
    }
    return sum;
}

// void SectionSum(const std:: vector<int>& arr, long long& sum, std::size_t start, std::size_t end){
//   sum = MySum (arr.begin()+start, arr.begin()+end);
// }

void* SectionSum(void* arg) {
  my_threads* data = static_cast<my_threads*>(arg);
    *(data->sum) = MySum(data->arr->begin() + data->start, data->arr->begin() + data->end);
    return nullptr;
}

int main(int argc, char* argv[]) { 
   
  if (argc != 3)
    {
        std::cout << "Invalid arguments: " << std::endl;
        return errno;
    }
  

  std::size_t n = std::stoull(argv[1]); //long long
  int m = std::stoi(argv[2]);//int
  if (n <= 1000000 || m < 1) {
      std::cerr << "provide N > 1,000,000 and M >= 1." << std::endl;
      return errno;
  }
  
  std::vector <int> arr(n);
  for (size_t i = 0; i < n; ++i) {
      arr[i] = rand() % 100;
  }
  // std::size_t start;
  // std::size_t end;
  long long single_sum = 0;
  long long multi_sum = 0;
  
  //single thread
  auto start_time = std::chrono::high_resolution_clock::now();
  single_sum = MySum(arr.begin(), arr.end());
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> single_duration = end_time - start_time;
  std::cout << "single thread sum: " << single_sum << std::endl;
  std::cout << "time taken (single threaded): " << single_duration.count() << " seconds\n";
  
  //multi thread

  start_time = std::chrono::high_resolution_clock::now();
  std::vector<pthread_t> threads_list(m);
  std::vector<my_threads> thread_data(m);
  std::vector<long long> partial_sums(m, 0);
  size_t block = n / m;

  for (int i = 0; i < m; ++i) {
      size_t start_ind = i * block;
      size_t end_ind = 0;
      if(i == (m - 1)) {end_ind = n;}
      else {end_ind = start_ind + block;}  
      thread_data[i] = {&arr, &partial_sums[i], start_ind, end_ind};
    pthread_create(&threads_list[i], nullptr, SectionSum, &thread_data[i]);
  }
  
  for (int i = 0; i < m; ++i) {
      pthread_join(threads_list[i], nullptr);
  }

  multi_sum = MySum(partial_sums.begin(), partial_sums.end());
  end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> multi_duration = end_time - start_time;

  std::cout << "multi threaded sum: " << multi_sum << std::endl;
  std::cout << "iime taken (multi threaded): " << multi_duration.count() << " seconds\n";
  
  
  
}
