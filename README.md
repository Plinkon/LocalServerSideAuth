# Local ServerSide Auth:
A neat little project I made where you can host a server which holds a database, then a client can connect and retrieve data from that database.
***THIS ONLY WORKS WITH WINDOWS!!!***

---
## How does this work?
It uses the Winsock API to setup a TCP server on a specified IP and port, then that server holds a custom database class for users and accounts. Then, there is a client which connects to that IP and port through the Winsock API and that client can send requests and recieve a response. I tried my best to make this very easy to use and understand, all you need is some intermediate C++ knowledge and you'll be good to go!

## How can I use/set this up?
First, make sure you are on windows **AND** have **G++** installed before setting up or using this.
1.  Download the source code:
	```bash
	git clone https://github.com/Plinkon/LocalServerSideAuth.git
	```
2. Open the Client.cpp and Server.cpp files in the following directories:

   `Client.cpp`:
   ```bash
	cd src/client/
   ```
   `Server.cpp`:
   ```bash
	cd src/server/
   ```

3. Open the 2 files with any editor and configure the definitions at the top of each file:

   ```cpp
	#define  USE_PORT_FROM_FILE false // if true, make sure to put a port in the ../port/port.txt file

	#define  PORT 5816 // set this to the port you want to use IF you're not using the port from a file

	#define  HOST_IP_ADDRESS "127.0.0.1" // set this to the IP address you want to use
   ```

4. Navigate to the `scripts/compile/` folder (you can also use the command from one of the source file folders):
	```bash
	cd ../../scripts/compile/
	```
5. Now run the `compile.bat` script.

Now the project should be compiled!

5. Go to the path (from the compile folder):
	```bash
	cd ../../output/
	``` 
And now you are all set to get up and running!
### How to run:
1. Run the `server.exe` file
2. Initialize the server by typing `1` or `0`
3. Then, run the `client.exe` file 

And that's it! If working correctly, the client should be connected to the server. Now take some time to explore the features of the CLI in the server.
