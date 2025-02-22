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

class Customer
{
private:
    sem_t *sem;
    int fd;
    int *add;
    const char *our_shop = "/our_shop";
    const char *sem_name = "/my_first_sem";
    size_t file_size;
    static std::map<int, std::string> numbers_to_items;

    MessageQueue queue;
    const char *queue_name = "/customer_queue";
    bool updates;
    std::string name;

public:
    // 1. init our numbers_to_items
    // 2. open fd.
    // 3. get add.
    // 4. open our semphore.
    // 5. get file size.
    // 6. make thread about updates in price.
    Customer();

    // Handle user order.
    void order();
};

std::map<int, std::string> Customer::numbers_to_items;

Customer::Customer()
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

    // increment our sem to be able our manger work syncronounsly with manager.
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
    updates = false;
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

        if (name < "1" || name > "6")
        {
            std::string num = queue.receiveMessage();
            numbers_to_items.insert({std::stoi(num), name}); 
        } 
        else 
        {
            updates = true;
        }
    } });

    // prevent zombie state.
    t.detach();
}

void Customer::order()
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

        while (true)
        {
            // guiding input..
            std::cout << "==============================================\n";
            std::cout << "Enter num. of our needed item: ";
            int needed_item;
            std::cin >> needed_item;
            std::cout << "How many items that you need: ";
            int num_items;
            std::cin >> num_items;

            // confirm order.
            if (needed_item >= 1 && needed_item <= numbers_to_items.size())
            {
                int price = add[(--needed_item) * 2 + 1];
                std::cout << "Then the total price will be: " << price * num_items << "\n";
                std::cout << "Confirm your order by (Y/N): ";

                std::string option;
                std::cin >> option;
                if (option == "Y")
                {
                    // check if there's change in price of needed item.
                    if (updates == true)
                    {
                        int updated_item = std::stoi(name) - 1;
                        if (needed_item == updated_item)
                        {
                            std::cout << "There's some change in price of needed items ...\n";
                            updates = false;
                            break;
                        }
                    }

                    if (add[needed_item * 2] >= num_items && num_items > 0)
                    {
                        // wait till our init process begin.
                        if (sem_wait(sem) == -1)
                        {
                            perror("Error: sem_wait");
                        }

                        // edit in your file
                        add[needed_item * 2] -= num_items;

                        // increment our sem to be able our manger work syncronounsly with manager.
                        if (sem_post(sem) == -1)
                        {
                            perror("Error: sem_post");
                        }

                        std::cout << "Order placed successfully!\n\n";
                    }
                    else if (num_items <= 0)
                    {
                        std::cout << "Invalid quantity! Please enter a positive number.\n\n";
                    }
                    else
                    {
                        std::cout << "Not enough stock available. Order again...\n\n";
                    }

                    // break to ask again in there's new order.
                    break;
                }
                else
                {
                    std::cout << "Your Order is discarded....\n\n";
                }
            }
            else
            {
                std::cout << "Wroong order!! Resume again... \n\n";
            }
        }
    }
}

int main()
{
    Customer customer;
    customer.order();
    return 0;
}