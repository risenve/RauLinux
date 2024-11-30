#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class parallel_scheduler {
public:
  // конструктор, чтобы задать размер
  explicit parallel_scheduler(size_t capacity)
      : threads_size(capacity), stop(false) {
    pthread_mutex_init(&mutex, nullptr); //иниц мютекса
    for (size_t i = 0; i < threads_size; ++i) {
      threads.emplace_back(&parallel_scheduler::main_thread, this);
    }
  }
  //добавляю функциб в очередь
  void run(void (*function)(int), int arg) {
    pthread_mutex_lock(&mutex);
    thread_stack.push([function, arg]() {
      function(arg); // тут функция работает
    });
    pthread_mutex_unlock(&mutex);
  }

  ~parallel_scheduler() {
    pthread_mutex_lock(&mutex);
    stop = true;
    pthread_mutex_unlock(&mutex);

    for (std::thread &worker : threads) {
      if (worker.joinable()) {
        worker.join();
      }
    }

    pthread_mutex_destroy(&mutex); //пока мютекс
  }

private:
  size_t threads_size;
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> thread_stack;
  pthread_mutex_t mutex;
  bool stop;

  void main_thread() {
    while (true) {
      std::function<void()> task = nullptr;

      pthread_mutex_lock(&mutex);
      if (stop && thread_stack.empty()) {
        pthread_mutex_unlock(&mutex);
        break; //если задач нет выходит из цикла
      }

      if (!thread_stack.empty()) {
        task = std::move(thread_stack.front());
        thread_stack.pop();
      }
      pthread_mutex_unlock(&mutex);

      if (task) {
        task();
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }
};


void print_task(int value) {
    std::cout << "task return value: " << value << std::endl;
}

int main() {
    parallel_scheduler scheduler(4);

    // adding   tasks to stack
    for (int i = 0; i < 10; ++i) {
        scheduler.run(print_task, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
