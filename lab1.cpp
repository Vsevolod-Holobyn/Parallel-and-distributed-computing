//1.2.1
#include <iostream>
#include <thread>

using namespace std;

void Thread1() {
    cout << 1 << endl;
}

void Thread2() {
    cout << 2 << endl;
}

int main() {
    thread t1(Thread1);
    thread t2(Thread2);

    return 0;
}



//1.2.2
#include <iostream>
#include <thread>

using namespace std;

void Thread1() {
    cout << 1 << endl;
}

void Thread2() {
    cout << 2 << endl;
}

int main() {
    thread t1(Thread1);
    thread t2(Thread2);

    t1.detach();
    t2.detach();

    Sleep(1000);
    return 0;
}



//1.2.3
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>

using namespace std;

list<int> l;

void AddToList(int val) {
    for (int i = 0; i < 10; ++i) {
        l.push_back(val + i);
        cout << "Added: " << val + i << "\n";
    }
}

void ListContains(int val) {
    for (int i = 0; i < 10; ++i) {
        auto it = find(l.begin(), l.end(), val);
        if (it != l.end()) {
            cout << "Contains " << val << ": Yes\n";
        }
        else {
            cout << "Contains " << val << ": No\n";
        }
    }
}

int main() {
    int start_val = 5;
    thread t1(AddToList, start_val);
    thread t2(ListContains, start_val);

    t1.join();
    t2.join();

    return 0;
}


//1.2.4
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>

using namespace std;

list<int> l;
mutex m;

void AddToList(int val) {
    for (int i = 0; i < 10; ++i) {
        m.lock();
        l.push_back(val + i);
        cout << "Added: " << val + i << "\n";
        m.unlock();
    }
}

void ListContains(int val) {
    for (int i = 0; i < 10; ++i) {
        m.lock();
        auto it = find(l.begin(), l.end(), val);
        if (it != l.end()) {
            cout << "Contains " << val << ": Yes\n";
        }
        else {
            cout << "Contains " << val << ": No\n";
        }
        m.unlock();
    }
}

int main() {
    int start_val = 5;
    thread t1(AddToList, start_val);
    thread t2(ListContains, start_val);

    t1.join();
    t2.join();

    return 0;
}

//1.2.5
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <Windows.h>

using namespace std;

list<int> l;
mutex m;

void AddToList(int val) {
    lock_guard<mutex> guard(m);
    l.push_back(val);
    cout << "Added: " << val << "\n";
}

void ListContains(int val) {
    lock_guard<mutex> guard(m);
    auto it = find(l.begin(), l.end(), val);
    if (it != l.end()) {
        cout << "Contains " << val << ": Yes\n";
    }
    else {
        cout << "Contains " << val << ": No\n";
    }
}

int main() {
    int start_val = 5;

    for (int i = 0; i < 10; ++i) {
        thread t1(AddToList, start_val + i);
        t1.detach();

        thread t2(ListContains, start_val);
        t2.detach();
    }

    Sleep(1000); 
    return 0;
}

//1.2.6
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <Windows.h>

using namespace std;

class someData {
public:
    string name = "Default";
    string surname = "Default";
    string address = "Default";
    int age = 0;
};

class exchangePerson {
public:
    someData data;
    mutex m;

    static void JohnDoe(exchangePerson& p) {
        lock_guard<mutex> guard(p.m);
        p.data.name = "John";
        p.data.surname = "Doe";
        p.data.address = "Unknown";
        p.data.age = 120;
    }

    static void JacobSmith(exchangePerson& p) {
        lock_guard<mutex> guard(p.m);
        p.data.name = "Jacob";
        p.data.surname = "Smith";
        p.data.address = "Known";
        p.data.age = 1;
    }

    static void Swap(exchangePerson& p1, exchangePerson& p2) {
        if (&p1 == &p2) return; 

        lock(p1.m, p2.m);

        lock_guard<mutex> guard1(p1.m, adopt_lock);
        lock_guard<mutex> guard2(p2.m, adopt_lock);

        cout << "\n--- Before Swap ---\n";
        cout << "Person 1: " << p1.data.name << " " << p1.data.surname << ", " << p1.data.age << "\n";
        cout << "Person 2: " << p2.data.name << " " << p2.data.surname << ", " << p2.data.age << "\n";

        swap(p1.data.name, p2.data.name);
        swap(p1.data.surname, p2.data.surname);
        swap(p1.data.address, p2.data.address);
        swap(p1.data.age, p2.data.age);

        cout << "\n--- After Swap ---\n";
        cout << "Person 1: " << p1.data.name << " " << p1.data.surname << ", " << p1.data.age << "\n";
        cout << "Person 2: " << p2.data.name << " " << p2.data.surname << ", " << p2.data.age << "\n";
    }
};

int main() {
    exchangePerson p1;
    exchangePerson p2;

    thread t1(exchangePerson::JohnDoe, ref(p1));
    t1.detach();

    thread t2(exchangePerson::JacobSmith, ref(p2));
    t2.detach();

    Sleep(100); 

    thread t3(exchangePerson::Swap, ref(p1), ref(p2));
    t3.join();

    return 0;
}



//1.2.7
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <Windows.h>

using namespace std;

class someData {
public:
    string name = "Default";
    string surname = "Default";
    string address = "Default";
    int age = 0;
};

class exchangePerson {
public:
    someData data;
    mutex m;

    static void JohnDoe(exchangePerson& p) {
        lock_guard<mutex> guard(p.m);
        p.data.name = "John";
        p.data.surname = "Doe";
        p.data.address = "Unknown";
        p.data.age = 120;
    }

    static void JacobSmith(exchangePerson& p) {
        lock_guard<mutex> guard(p.m);
        p.data.name = "Jacob";
        p.data.surname = "Smith";
        p.data.address = "Known";
        p.data.age = 1;
    }

    static void Swap(exchangePerson& p1, exchangePerson& p2) {
        if (&p1 == &p2) return;

        unique_lock<mutex> lock1(p1.m, defer_lock);
        unique_lock<mutex> lock2(p2.m, defer_lock);

        lock(lock1, lock2);

        cout << "\n--- Before Swap ---\n";
        cout << "Person 1: " << p1.data.name << " " << p1.data.surname << ", " << p1.data.age << "\n";
        cout << "Person 2: " << p2.data.name << " " << p2.data.surname << ", " << p2.data.age << "\n";

        swap(p1.data.name, p2.data.name);
        swap(p1.data.surname, p2.data.surname);
        swap(p1.data.address, p2.data.address);
        swap(p1.data.age, p2.data.age);

        cout << "\n--- After Swap ---\n";
        cout << "Person 1: " << p1.data.name << " " << p1.data.surname << ", " << p1.data.age << "\n";
        cout << "Person 2: " << p2.data.name << " " << p2.data.surname << ", " << p2.data.age << "\n";
    }
};

int main() {
    exchangePerson p1;
    exchangePerson p2;

    thread t1(exchangePerson::JohnDoe, ref(p1));
    t1.detach();

    thread t2(exchangePerson::JacobSmith, ref(p2));
    t2.detach();

    Sleep(100);

    thread t3(exchangePerson::Swap, ref(p1), ref(p2));
    t3.join();

    return 0;
}