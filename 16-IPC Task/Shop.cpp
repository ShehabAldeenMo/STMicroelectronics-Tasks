#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>
#include <memory>
#include <functional>
#include <map>
#include <sys/mman.h>
#include <sys/types.h>
#include <iomanip>
#include "MessageQueue.h"

class Shop
{
private:
    std::mutex m;
    sem_t *sem;
    int fd;
    int *add;
    const char *our_shop = "/our_shop";
    const char *sem_name = "/my_first_sem";
    size_t file_size;

    static Shop *instance;
    static std::map<int, std::string> numbers_to_items;

    Shop();

    MessageQueue queue;
    const char *queue_name = "/shop_queue";
    std::string name;

public:
    // singleton design pattern.
    static Shop *get_instance();
    ~Shop();
    void open_shop();
};

Shop *Shop::instance = nullptr;
std::map<int, std::string> Shop::numbers_to_items;

Shop::Shop()
{
    // [Req.2]
    // 1. require our mutex. 2. make semphore and acquired to prevent access on other processes.
    std::lock_guard<std::mutex> lock(m); // Locks the mutex

    // [Req.6]
    // to ensure that there's no other semphore created before.
    sem = sem_open(sem_name, 0); // signaling other processes that are ready to work when finishs our initialization.
    if (sem != SEM_FAILED)
    {
        // Unlink the named semaphore (removes it from the system)
        if (sem_unlink(sem_name) == -1)
        {
            perror("Error: sem_unlink");
        }

        // Close the named semaphore
        if (sem_close(sem) == -1)
        {
            perror("Error: sem_close");
        }
        sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1); // signaling other processes that are ready to work when finishs our initialization.

        if (sem == SEM_FAILED)
        {
            perror("Error: sem_open");
            exit(1);
        }
    }

    if (sem_wait(sem) == -1)
    {
        perror("Error: sem_wait");
    }

    // [Req.1]
    // 1. create our shop file  2. map our file.
    fd = shm_open(our_shop, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, 1024);
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1)
    {
        std::cerr << "Error: fstat failed";
    }
    file_size = statbuf.st_size;
    add = (int *)mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FILE, fd, 0);

    // [Req.4]
    // 1.make init operation for items.
    numbers_to_items.insert({0, "dress"});
    numbers_to_items.insert({1, "T-Shirt"});
    numbers_to_items.insert({2, "Jeans"});
    numbers_to_items.insert({3, "Jacket"});
    numbers_to_items.insert({4, "Sneakers"});
    numbers_to_items.insert({5, "Hat"});

    // insert # of available items. and insert prices.
    add[0] = 10;
    add[1] = 150;
    add[2] = 10;
    add[3] = 200;
    add[4] = 10;
    add[5] = 80;
    add[6] = 10;
    add[7] = 90;
    add[8] = 10;
    add[9] = 100;
    add[10] = 10;
    add[11] = 110;
    add[12] = '\0';

    if (sem_post(sem) == -1)
    {
        perror("Error: sem_post");
    }

    // change the protection of our file from this process to read only
    if (mprotect(add, file_size, PROT_READ) == -1)
    {
        std::cerr << "Error: changing memory protection to read permission";
        exit(1);
    }

    if (!queue.openQueue(queue_name, O_CREAT | O_RDONLY)) // Open queue with read permissions
        exit(EXIT_FAILURE);

    // create thread about updates.
    std::thread t([this]()
                  {
        while (true)
        {
                // get its name.
                name = queue.receiveMessage();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                std::string num = queue.receiveMessage();
                numbers_to_items.insert({std::stoi(num), name});
        
        } });

    // prevent zombie state.
    t.detach();

    // release our mutex will be automacally when finish our function.
}

Shop *Shop::get_instance()
{
    if (instance == nullptr)
    {
        instance = new Shop;
        return instance;
    }
    return instance;
}

Shop::~Shop()
{
    if (instance != nullptr)
        delete this;

    // Unlink the named semaphore (removes it from the system)
    if (sem_unlink(sem_name) == -1)
    {
        perror("Error: sem_unlink");
    }

    // Close the named semaphore
    if (sem_close(sem) == -1)
    {
        perror("Error: sem_close");
    }
}

void Shop::open_shop()
{
    // require our mutex.
    {
        std::lock_guard<std::mutex> lock(m);
    }

    // [Req.3] [Req.5]
    std::thread t([this]()
                  {
    while (true)
    {
        std::cout << std::left << std::setw(12) << "Item"
                  << std::setw(25) << "Available Quantity"
                  << std::setw(10) << "Price $" << std::endl;
        std::cout << "-----------------------------------------------------\n";


        // show up our items details.
        int i = 0;
        while (i < numbers_to_items.size())
        {
            if (numbers_to_items.find(i) != numbers_to_items.end())
            {
                std::cout << std::left << std::setw(12) << numbers_to_items.at(i)
                          << std::setw(25) << add[i * 2]
                          << std::setw(10) << add[i * 2 + 1] << "\n";
            }
            i++;
        }
        std::cout << "==============================================\n\n";

        // make some thread on this handler with lamda function make periodically sleep time.
        std::this_thread::sleep_for(std::chrono::seconds(4));
    } });

    // detecting to prevent blocking behaviour and enable kernal to flush it from zombie state if terminated.
    t.detach();
}

int main()
{
    Shop *instance = Shop::get_instance();
    instance->open_shop();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}