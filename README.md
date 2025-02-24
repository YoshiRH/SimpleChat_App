# SimpleChat [in progress]

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.27-%23008FBA)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

**SimpleChat** is a lightweight, command-line-based chat application written in C++. It allows users to create accounts, log in, send real-time messages, and view chat history. The program uses a client-server architecture with support for multiple concurrent clients, built on Winsock and threads. The whole programs runs locally on 127.0.0.1 (55555 port).

---

## Features

- **Registration and Login**: Create an account and log in using a username and password (passwords are hashed with SHA-256).
- **Message Sending**: Send messages to all logged-in users in real time.
- **Chat History**: View the last messages after joining the chat (limited to 25 messages).
- **Multi-Client Support**: The server can handle up to 20 clients simultaneously.
- **Security**: Passwords are stored encrypted in a `users.txt` file.
- **Simple Interface**: Intuitive console-based interface with basic commands.

---

## Screenshots
![image](https://github.com/user-attachments/assets/d4f83f2b-8b47-47ed-b7b7-b0f8b2ba2800)
![image](https://github.com/user-attachments/assets/aec73555-068a-4a4a-af26-d12ec5fecc24)
![image](https://github.com/user-attachments/assets/8dc5761c-424d-4ed2-9590-8cf86e224295)

---

## How to Use

### Prerequisites
- **CMake 3.27** or newer ([Download CMake](https://cmake.org/download/))
- C++17-compatible compiler (e.g., MSVC in Visual Studio 2022)
- Windows (the project uses Winsock, currently limited to this platform)

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/YoshiRH/SimpleChat_App.git
   cd SimpleChat_App
2. **Build with CMake**
Ensure you have a C++ compiler installed (e.g., MSVC in Visual Studio).
Run the following commands:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
3. **Run program**  
Best way to run the program would be to go to program location in file explorer [project dir]/build/Server and [project dir]/build/Client
and from there run server app first and then how many clients you want from 'Client' directory to connect to server.

### Usage instructions     
**Start the server**
The server listens on 127.0.0.1:55555. You’ll see a startup message in the console.

**Connect a Client**     
After launching the client, choose:
(1) Register: Create a new account (e.g., user1, pass123).
(2) Login: Log in to an existing account.
Once logged in, you can start sending messages.

**Controls**   
Type a message and press Enter to send it to all logged-in users.
Type exit to disconnect and close the client.

## License
This project is licensed under the MIT License. See the LICENSE.txt file for details.
