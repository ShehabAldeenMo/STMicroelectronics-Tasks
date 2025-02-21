# Table of Content


# **Clothing Shop Simulation - Design Steps**  

The application consists of **three main processes** that must communicate effectively to simulate a clothing shop. We need to establish **Inter-Process Communication (IPC)** mechanisms and ensure synchronization between them.  

---

# **📌 Requirements Analysis**  

## **1) The Shop (Main Process)**
- Reads initial inventory (count and price) from a file.  
- Maintains a record of available items.  
- Periodically displays the inventory status.  
- Updates stock levels and prices based on external interactions.  

## **2) Customer Process**  
- Interacts with the user to take orders (multiple items, different counts).  
- Checks if requested items are available.  
- Updates the shop’s inventory after a successful purchase.  
- Notifies the user if an item is out of stock.  

## **3) Inventory Manager Process**  
- Interacts with the user to take restocking or price update inputs.  
- Monitors inventory levels and restocks items when low.  
- Updates price changes in the shop’s records.  

## **🛠️ IPC Mechanisms Needed:**  
- **Shared Memory**: To store and update inventory details dynamically.  
- **Message Queues**: To allow customers and the inventory manager to send requests to the shop process.  
- **Semaphores**: To ensure synchronization between processes (prevent race conditions).  