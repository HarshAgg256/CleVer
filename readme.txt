Dear Evaluator,

All the servers(primary and secondary) along with load balancer may print absurd values and strings which is just to keep track of how the threads synchronize along with mutexes and semaphores. The client however prints all the necessary stuff required. For evaluation please keep that in mind while evaluating.

The correct sequence of running the files is as

load_balancer -> any server -> client -> cleanup
