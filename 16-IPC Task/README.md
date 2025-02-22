# Table of Contents

---

# **Clothing Shop Simulation - Design Steps**

The application consists of **three main processes** that must communicate effectively to simulate a clothing shop. We need to establish **Inter-Process Communication (IPC)** mechanisms and ensure synchronization between them.

---

# **📌 Requirements Analysis**

## **1) The Shop (Main Process)**
- Manages the shop's inventory.
- Prints the count and price of each item periodically.
- Initial values are stored in a file, used only for initialization.

### **Implementation Details**
1. **[Req.1]** Use `shm_open()`, `ftruncate()`, and `mmap()` in our constructor.
2. **[Req.2]** Implement a `mutex` to be released after the initialization thread completes.
3. **[Req.3]** Run an information thread periodically using thread sleep to maintain CPU efficiency.
4. **[Req.4]** init your file.
5. **[Req.5]** The information thread should update and read changes every **4 seconds**.
6. **[Req.6]** Use a semaphore (`sem_open()`, `sem_wait()`, and `sem_post()`) to control access to the file during initialization.
7. **[Req.7]** Use `mq_open()` to establish a message queue for updates on item availability and price changes.
8. **[Req.8]** Maintain a background thread to receive messages from the queue and handle updates dynamically.

## **2) Customer Process**
- Simulates customers placing orders.
- Interactively takes orders from the user.

### **Implementation Details**
1. **[Req.1]** Provide guidance on available items and required user input.
2. **[Req.2]** Display the selected item's price and confirm the order with the user.
3. **[Req.3]** Use `shm_open()` and `mmap()` for shared file access in read-write mode.
4. **[Req.4]** Ensure the shop process has started using `sem_open()`, `sem_wait()`, and `sem_post()`.
5. **[Req.5]** Process customer orders using `sem_wait()`, update the file, then release with `sem_post()`.
6. **[Req.6]** Use `mq_open()` to establish a message queue for updates on item availability and price changes.
7. **[Req.7]** Maintain a background thread to receive messages from the queue and handle updates dynamically.

## **3) Inventory Manager Process**
- Interacts with the user to accept inventory and price updates.
- Restocks items and updates pricing when needed.

### **Implementation Details**
1. **[Req.1]** Provide guidance to the manager on available items, stock count, and pricing.
2. **[Req.2]** Use `shm_open()` and `mmap()` for shared file access in read-write mode.
3. **[Req.3]** Ensure synchronization using `sem_open()`, `sem_wait()`, and `sem_post()`.
4. **[Req.4]** Process updates from the manager using `sem_wait()`, modify the file, then release with `sem_post()`.
5. **[Req.5]** If an item is restocked or its price changes, send notifications via the message queue (`mq_send()`) to inform customers.
---

## **🛠️ IPC Mechanisms Needed:**
- **Message Queues (`mq_open()`)**
- **Shared Memory (`shm_open()`, `mmap()`)**
- **Semaphores (`sem_open()`, `sem_wait()`, `sem_post()`)**
- **Mutex for synchronization**
- **Multi-threading for background tasks**

## How to build 
```bash
g++ Shop.cpp MessageQueue.cpp -o shop
g++ Customer.cpp MessageQueue.cpp -o customer
g++ Manager.cpp MessageQueue.cpp -o manager
```