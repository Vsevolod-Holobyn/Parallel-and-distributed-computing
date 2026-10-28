//1.2.1
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <cmath>
#include <Windows.h>

std::queue<int> data_queue;
bool flag = false;
std::mutex m;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

void DataPreparation() {
    std::cout << "Введіть цілі числа (введіть будь-яку літеру або 0 для завершення):\n";
    int num;
    while (std::cin >> num && num != 0) {
        std::lock_guard<std::mutex> lk(m);
        data_queue.push(num);
    }

    // Встановлення прапорця після завершення введення
    {
        std::lock_guard<std::mutex> lk(m);
        flag = true;
    }
}

void DataProcessing() {
    std::unique_lock<std::mutex> lk(m);
    while (!flag) {
        lk.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lk.lock();
    }

    std::cout << "\nПрості числа у черзі: ";
    bool found = false;
    while (!data_queue.empty()) {
        int val = data_queue.front();
        data_queue.pop();
        if (isPrime(val)) {
            std::cout << val << " ";
            found = true;
        }
    }
    if (!found) {
        std::cout << "відсутні";
    }
    std::cout << std::endl;
}

int main() {
    SetConsoleCP(1251);        
    SetConsoleOutputCP(1251);
    std::thread prepThread(DataPreparation);
    prepThread.detach(); // Від'єднання потоку підготовки даних

    std::thread procThread(DataProcessing);
    procThread.join();   // Головний потік очікує на завершення обробки

    return 0;
}



//1.2.2
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <Windows.h>


std::mutex cv_m;
std::condition_variable cv;
int i = 0;

void Waits(int id) {
    std::unique_lock<std::mutex> lk(cv_m);
    std::cout << "Потік " << id << ": входить у стан очікування...\n";

    cv.wait(lk, [] { return i == 1; });

    std::cout << "Потік " << id << ": очікування завершене (i == 1).\n";
}

void Awake() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "\n[Awake]: Починаю повідомляти решту потоків (перша спроба, i = 0)...\n";
    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1;
        std::cout << "[Awake]: Змінній i присвоєно 1. Повторне повідомлення потоків...\n";
    }
    cv.notify_all();
}

int main() {
    SetConsoleCP(1251);        
    SetConsoleOutputCP(1251);
    std::thread t1(Waits, 1);
    std::thread t2(Waits, 2);
    std::thread t3(Waits, 3);
    std::thread awakeThread(Awake);

    awakeThread.join();
    t1.join();
    t2.join();
    t3.join();

    return 0;
}



//1.2.3
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex cv_m;
std::condition_variable cv;
int i = 0;

void ThreadFunc(int id) {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, [] { return i == 1; });
    std::cout << "Повідомлення з потоку " << id << std::endl;
}

void Notify() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1; 
    }
    cv.notify_one();
}

int main() {
    std::thread Thread1(ThreadFunc, 1);
    std::thread Thread2(ThreadFunc, 2);
    std::thread Thread3(ThreadFunc, 3);
    std::thread notifyThread(Notify);

    Thread1.detach();
    Thread2.detach();
    Thread3.detach();

    notifyThread.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return 0;
}



//1.2.4
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <Windows.h>

std::queue<int> data_queue;
std::mutex m;
std::condition_variable cv;

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

void DataPreparation() {
    std::cout << "Введіть цілі числа через пробіл (введіть 0 для завершення вводу):\n";
    std::queue<int> local_inputs;
    int num;
    while (std::cin >> num && num != 0) {
        local_inputs.push(num);
    }

    // Додавання маркером завершення 0 на випадок, якщо користувач нічого не ввів
    local_inputs.push(0);

    {
        std::lock_guard<std::mutex> lk(m);
        data_queue = std::move(local_inputs);
    }
    cv.notify_one(); // Сповіщаємо потік обробки, що черга не порожня
}

void DataProcessing() {
    std::unique_lock<std::mutex> lk(m);

    // Очікуємо, доки черга не перестане бути порожньою
    cv.wait(lk, [] { return !data_queue.empty(); });

    std::cout << "\nПрості числа у черзі: ";
    bool found = false;
    while (!data_queue.empty()) {
        int val = data_queue.front();
        data_queue.pop();
        if (val != 0 && isPrime(val)) {
            std::cout << val << " ";
            found = true;
        }
    }
    if (!found) {
        std::cout << "відсутні";
    }
    std::cout << std::endl;
}

int main() {
    SetConsoleCP(1251);        
    SetConsoleOutputCP(1251);
    std::thread prepThread(DataPreparation);
    prepThread.detach();

    std::thread procThread(DataProcessing);
    procThread.join();

    return 0;
}



//1.2.5
#include <iostream>
#include <future>
#include <chrono>
#include <cmath>
#include <Windows.h>

bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

long long findNthPrime(int n) {
    int count = 0;
    long long candidate = 1;
    while (count < n) {
        candidate++;
        if (isPrime(candidate)) {
            count++;
        }
    }
    return candidate;
}

void runExperiment(int n, std::launch policy, const std::string& policyName) {
    std::cout << "\n=== Режим: " << policyName << " ===\n";

    // Запуск std::async з обраною політикою
    std::future<long long> primeFuture = std::async(policy, findNthPrime, n);

    int choice;
    std::cout << "Поки йде обчислення, оберіть функцію від числа " << n << ":\n"
        << "1 - Корінь квадратний (sqrt)\n"
        << "2 - Синус (sin)\n"
        << "3 - Натуральний логарифм (ln)\n"
        << "Ваш вибір: ";
    std::cin >> choice;

    switch (choice) {
    case 1: std::cout << "sqrt(" << n << ") = " << std::sqrt(n) << "\n"; break;
    case 2: std::cout << "sin(" << n << ") = " << std::sin(n) << "\n"; break;
    case 3: std::cout << "ln(" << n << ") = " << std::log(n) << "\n"; break;
    default: std::cout << "Невірний вибір.\n"; break;
    }

    std::cout << "Очікування результату обчислення простого числа...\n";
    auto startWait = std::chrono::high_resolution_clock::now();

    long long result = primeFuture.get();

    auto endWait = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> waitTime = endWait - startWait;

    std::cout << n << "-те просте число: " << result << "\n";
    std::cout << "Час очікування на get(): " << waitTime.count() << " сек.\n";
}

int main() {
    SetConsoleCP(1251);        
    SetConsoleOutputCP(1251);
    int n;
    std::cout << "Введіть номер простого числа n: ";
    std::cin >> n;  

    runExperiment(n, std::launch::deferred, "std::launch::deferred");
    runExperiment(n, std::launch::async, "std::launch::async");

    return 0;
}



//1.2.6
#include <iostream>
#include <thread>
#include <future>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <string>
#include <Windows.h>


std::deque<std::packaged_task<long long(int)>> tasks_deque;
std::deque<int> n_deque;
std::mutex deque_mutex;
std::condition_variable cv;
bool stop_worker = false;

bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

long long findNthPrime(int n) {
    int count = 0;
    long long candidate = 1;
    while (count < n) {
        candidate++;
        if (isPrime(candidate)) {
            count++;
        }
    }
    return candidate;
}

void WorkerThread() {
    while (true) {
        std::packaged_task<long long(int)> task;
        int n = 0;

        {
            std::unique_lock<std::mutex> lk(deque_mutex);
            cv.wait(lk, [] { return !tasks_deque.empty() || stop_worker; });

            if (stop_worker && tasks_deque.empty()) {
                break; // Усі завдання виконано і отримано сигнал завершення
            }

            task = std::move(tasks_deque.front());
            tasks_deque.pop_front();

            n = n_deque.front();
            n_deque.pop_front();
        }

        // Виконання завдання поза блокуванням м'ютекса
        task(n);
    }
}

int main() {
    SetConsoleCP(1251);       
    SetConsoleOutputCP(1251);
    std::thread worker(WorkerThread);
    std::vector<std::pair<int, std::future<long long>>> results;

    std::cout << "Вводьте номери простих чисел n (або 'q' для завершення):\n";
    std::string input;
    while (std::cout << "n = " && std::cin >> input && input != "q") {
        int n = std::stoi(input);
        if (n <= 0) continue;

        std::packaged_task<long long(int)> task(findNthPrime);
        std::future<long long> fut = task.get_future();
        results.push_back({ n, std::move(fut) });

        {
            std::lock_guard<std::mutex> lk(deque_mutex);
            tasks_deque.push_back(std::move(task));
            n_deque.push_back(n);
        }
        cv.notify_one();
    }

    {
        std::lock_guard<std::mutex> lk(deque_mutex);
        stop_worker = true;
    }
    cv.notify_one();

    // Очікування завершення робочого потоку
    worker.join();

    std::cout << "\nРезультати обчислень:\n";
    for (auto& pair : results) {
        std::cout << pair.first << "-те просте число = " << pair.second.get() << "\n";
    }

    return 0;
}



//1.2.7
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>
#include <Windows.h>


bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

long long findNthPrime(int n) {
    int count = 0;
    long long candidate = 1;
    while (count < n) {
        candidate++;
        if (isPrime(candidate)) {
            count++;
        }
    }
    return candidate;
}

void FirstThread(int n,
    std::promise<long long> promNth,
    std::promise<bool> promSignal,
    std::promise<long long> promTenNth) {

    long long nthPrime = findNthPrime(n);
    promNth.set_value(nthPrime);

    promSignal.set_value(true);

    long long tenNthPrime = findNthPrime(n * 10);
    promTenNth.set_value(tenNthPrime);
}

void SecondThread(int n, std::future<bool> futSignal) {
    if (futSignal.get()) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "[Другий потік]: Квадратний корінь з " << n
            << " = " << std::sqrt(n) << std::endl;
    }
}

int main() {
    SetConsoleCP(1251);        
    SetConsoleOutputCP(1251);

    int n;
    std::cout << "Введіть номер простого числа n: ";
    std::cin >> n;

    std::promise<long long> promNth;
    std::future<long long> futNth = promNth.get_future();

    std::promise<bool> promSignal;
    std::future<bool> futSignal = promSignal.get_future();

    std::promise<long long> promTenNth;
    std::future<long long> futTenNth = promTenNth.get_future();

    std::thread t1(FirstThread, n, std::move(promNth), std::move(promSignal), std::move(promTenNth));
    std::thread t2(SecondThread, n, std::move(futSignal));

    std::cout << "[Головний потік]: " << n << "-те просте число = " << futNth.get() << std::endl;

    std::cout << "[Головний потік]: " << (n * 10) << "-те просте число = " << futTenNth.get() << std::endl;

    t1.join();
    t2.join();

    return 0;
}
