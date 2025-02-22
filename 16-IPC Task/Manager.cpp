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
#include <cstring>
#include "MessageQueue.h"

class Manager
{
private:
    sem_t *sem;
    int fd;
    int *add;
    const char *our_shop = "/our_shop";
    const char *sem_name = "/my_first_sem";
    size_t file_size;
    static std::map<int, std::string> numbers_to_items;

    MessageQueue shop_queue;
    MessageQueue customer_queue;
    const char *shop_queue_name = "/shop_queue";
    const char *customer_queue_name = "/customer_queue";

    void guiding_lines();
    void add_new_item();

public:
    // 1. init our numbers_to_items
    // 2. open fd.
    // 3. get add.
    // 4. open our semphore.
    // 5. get file size.
    // 6. make thread about updates in price.
    Manager();

    // Handle user order.
    void restocking();
};

std::map<int, std::string> Manager::numbers_to_items;

Manager::Manager()
{
    // 1. init our numbers_to_items mapper.
    numbers_to_items.insert({0, "dress"});
    numbers_to_items.insert({1, "T-Shirt"});
    numbers_to_items.insert({2, "Jeans"});
    numbers_to_items.insert({3, "Jacket"});
    numbers_to_items.insert({4, "Sneakers"});
    numbers_to_items.insert({5, "Hat"});

    // 4. open our semphore.
    sem = sem_open(sem_name, 0);
    if (sem == SEM_FAILED)
    {
        perror("Error: sem_open");
        exit(1);
    }

    // wait till our init process begin.
    if (sem_wait(sem) == -1)
    {
        perror("Error: sem_wait");
    }

    // increment our sem to be able our manger work syncronounsly with customer.
    if (sem_post(sem) == -1)
    {
        perror("Error: sem_post");
    }

    // 2. open fd.
    fd = shm_open(our_shop, O_CREAT | O_RDWR, 0666);
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1)
    {
        std::cerr << "Error: fstat failed";
    }

    // 5. get file size.
    file_size = statbuf.st_size;

    // 3. get add.
    add = (int *)mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FILE, fd, 0);

    // 6. connect on opur queue for changes in price.
    if (!shop_queue.openQueue(shop_queue_name, O_CREAT | O_RDWR)) // Open queue with read permissions
        exit(EXIT_FAILURE);

    if (!customer_queue.openQueue(customer_queue_name, O_CREAT | O_RDWR)) // Open queue with read permissions
        exit(EXIT_FAILURE);
}

void Manager::guiding_lines()
{
    // guiding input..
    std::cout << "==============================================\n";
    std::cout << "Enter num. of updated item: ";
    int needed_item;
    std::cin >> needed_item;
    needed_item--;

    // edit in number of items
    std::cout << "Edit in number of items (Y/N) ";
    std::string option;
    std::cin >> option;

    if (option == "Y")
    {
        std::cout << "Number of restocking items: ";
        int num_items;
        std::cin >> num_items;

        if (needed_item < 0 || needed_item >= numbers_to_items.size())
        {
            std::cout << "Invalid item number!\n";
            return;
        }

        // wait till our init process begin.
        if (sem_wait(sem) == -1)
        {
            perror("Error: sem_wait");
        }

        // edit in numer of available items
        add[needed_item * 2] += num_items;

        // increment our sem to be able our manger work syncronounsly with manager.
        if (sem_post(sem) == -1)
        {
            perror("Error: sem_post");
        }
    }

    // edit in price of items
    std::cout << "Edit in price of items (Y/N) ";
    std::cin >> option;

    if (option == "Y")
    {
        std::cout << "Price of restocking item: ";
        int price;
        std::cin >> price;

        // wait till our init process begin.
        if (sem_wait(sem) == -1)
        {
            perror("Error: sem_wait");
        }

        // edit price of item
        add[needed_item * 2 + 1] = price;

        // increment our sem to be able our manger work syncronounsly with manager.
        if (sem_post(sem) == -1)
        {
            perror("Error: sem_post");
        }

        // notify customer
        customer_queue.sendMessage(std::to_string(needed_item + 1), 1);
    }
}

void Manager::add_new_item()
{
    int needed_item = numbers_to_items.size();

    // edit in number of items
    std::cout << "Add new item (Y/N) ";
    std::string option;
    std::cin >> option;

    if (option == "Y")
    {
        std::cout << "Enter its name: ";
        std::string name;
        std::cin.ignore(); // Clear the newline character
        std::getline(std::cin, name);
        numbers_to_items.insert({needed_item, name});

        // edit in numer of available items
        std::cout << "Number of restocking items: ";
        int num_items;
        std::cin >> num_items;

        // wait till our init process begin.
        if (sem_wait(sem) == -1)
        {
            perror("Error: sem_wait");
        }

        add[needed_item * 2] = num_items;

        // edit in price of items
        std::cout << "Price of restocking item: ";
        int price;
        std::cin >> price;

        add[needed_item * 2 + 1] = price;

        // add nul terminator
        add[needed_item * 2 + 2] = '\0';

        if (sem_post(sem) == -1)
        {
            perror("Error: sem_post");
        }

        // notify customer and shop
        customer_queue.sendMessage(name, 1);
        customer_queue.sendMessage(std::to_string(needed_item), 1);

        shop_queue.sendMessage(name, 1);
        shop_queue.sendMessage(std::to_string(needed_item), 1);
    }
}

void Manager::restocking()
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
                std::cout << std::left << std::setw(5) << i + 1
                          << std::setw(15) << numbers_to_items.at(i)
                          << std::setw(15) << add[i * 2]
                          << std::setw(10) << add[i * 2 + 1] << "\n";
            }
            i++;
        }

        guiding_lines();
        add_new_item();
    }
}

int main()
{
    Manager manager;
    manager.restocking();
    return 0;
}