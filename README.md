# CleVer
A Client-Server based Architecture for Graph Database System.

## Overview

The Distributed Graph Database System is designed to manage and manipulate unweighted, undirected graphs in a distributed environment. This system incorporates a load balancer, primary server, secondary servers, a cleanup process, and multiple clients to handle various graph operations efficiently. It employs inter-process communication, multithreading, and advanced synchronization mechanisms to ensure robust performance and data integrity.

## Components

### Load Balancer

Developed a load balancer that acts as the central orchestrator, managing the distribution of client requests to appropriate servers. The load balancer employs dynamic scheduling algorithms to balance the load evenly across servers, enhancing system throughput and minimizing latency. It uses message queues for inter-process communication, ensuring efficient and orderly request handling.

### Primary Server

Constructed a primary server responsible for managing all write operations, including adding and modifying graphs. The primary server employs mutexes and condition variables to serialize access to shared graph data, preventing race conditions and ensuring consistency. It communicates with the load balancer and secondary servers using shared memory segments and message queues to coordinate updates.

### Secondary Servers

Enabled users to perform read operations through two secondary servers, each capable of handling multiple read requests concurrently. These servers implement thread pools to manage concurrent client connections efficiently. Semaphore mechanisms are used to control access to shared resources, ensuring data consistency while allowing high concurrency for read operations.

### Cleanup Process

Developed a cleanup process that ensures a graceful shutdown of the entire system. This process monitors the state of ongoing operations and waits for all active requests to complete before terminating the servers and load balancer. It uses signals and shared memory to coordinate the shutdown process across all components, ensuring a smooth and orderly termination.

### Clients

Created multiple client programs that interface with the load balancer to send requests for various graph operations. Clients can add new graphs, modify existing graphs, and perform DFS or BFS on stored graphs. The clients utilize sockets for communication with the load balancer, ensuring a reliable and efficient transfer of requests and responses.

## Graph Operations

The system supports the following graph operations:

1. **Add a New Graph**: Clients can create and add a new graph file to the system. The file should be named `Gx.txt` (e.g., `G1.txt`) and follow the specified format.
2. **Modify an Existing Graph**: Clients can update the adjacency matrix of an existing graph file.
3. **Perform DFS**: Clients can request a Depth-First Search traversal on an existing graph, receiving the traversal order as a response.
4. **Perform BFS**: Clients can request a Breadth-First Search traversal on an existing graph, receiving the traversal order as a response.

### Graph File Format

Each graph is represented in a text file named `Gx.txt` (e.g., `G1.txt`). The file format is as follows:

```
n
a11 a12 ... a1n
a21 a22 ... a2n
...
an1 an2 ... ann
```

- `n`: Number of nodes (≤ 30).
- `aij`: Adjacency matrix entries (0 or 1), representing the presence or absence of an edge between nodes.

## Threading and Synchronization

The system employs several threading and synchronization constructs to ensure efficient and safe concurrent operations:

- **Mutexes**: Used to protect critical sections in the primary server where shared graph data is accessed or modified, preventing race conditions.
- **Semaphores**: Used in secondary servers to control access to shared resources and manage concurrent read operations, ensuring data consistency.
- **Condition Variables**: Employed in the primary server to manage the coordination between different threads, particularly for handling write operations.
- **Thread Pools**: Implemented in secondary servers to handle multiple client connections concurrently, improving responsiveness and scalability.
- **Message Queues**: Used for inter-process communication between the load balancer, primary server, and secondary servers, enabling efficient message passing.
- **Shared Memory**: Utilized for fast data exchange between processes, reducing the overhead associated with other IPC mechanisms.

## Compilation and Execution

Open atleast 6 terminals from the cloned project directory on an Ubuntu 22.04 system and enter the compilation commands of each process in separate terminals.  

### Compilation

Compile all the components using the following commands:

```bash
gcc -o load_balancer load_balancer.c
gcc -o primary_server primary_server.c 
gcc -o secondary_server secondary_server.c 
gcc -o cleanup cleanup.c
gcc -o client client.c
```

### Execution

Run the processes in the following order:

1. **Start the Load Balancer**:
   ```bash
   ./load_balancer
   ```

2. **Start the Primary Server**:
   ```bash
   ./primary_server
   ```

3. **Start the Secondary Servers** (run this command twice for two instances):
   ```bash
   ./secondary_server
   ```

4. **Start the Cleanup Process**:
   ```bash
   ./cleanup
   ```

5. **Run the Client**:
   ```bash
   ./client
   ```

### Client Input

On the client side, users should follow and answer the prompts correctly to avoid any undesired behavior. 
For `Sequence number`, give a  unique value from 1 to 100 everytime the client wants to request something.
For `Operation number`, give any value from 1-4 depending upon the menu options.
For `File name`, specify a name of the form G<no>.txt where <no> can be any number.


## Expected Result Format

- **Add Graph**: A confirmation message indicating successful addition of the graph.
- **Modify Graph**: A confirmation message indicating successful modification of the graph.
- **DFS/BFS Result**: The traversal order of the nodes in the graph, returned in a readable format.

