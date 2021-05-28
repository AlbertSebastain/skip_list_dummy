# Dummy skip list
The project has implemented a skip list. It is an effective tool for saving data. Each item contains a key and a value. Because the data is saved in order,  the complexities of searching insertion and deletion are O(log n). Inspired by Redis, the data will also be saved in a hash map. On one hand, the hash map can provide a lower complexity for searching. But in another hand, a hash map also increases the complexity of insertion.  
## Usage
The project has two modes. You can include skiplist.h and use the interfaces that are provided by skiplist and write your program. In another way, you can also start the server mode and use a client to add or search items.    
We have two shell files to demonstrate the two modes.  
1. You can run performance_test.sh to test the performance of skiplist. The result is saved in performance_test_result.  
```
bash ./performance_test.sh
vim performance_test_result
```  
2. You can run server_mode.sh to start the server and use a client to send commands to the server.  
```
bash ./server_mode.sh
```  
Use client : telnet  
```
telnet [your IP] 1232
```
The interfaces for server mode are:  
```
SEARCH_KEY [key]  
SEARCH_KEY_FAST [key]  
INSERT_ITEM [key] [value]  
DELETE_ITEM [key]  
SHOW_SKIPLIST  
SIZE  
SAVE_FILE  
LOAD_FILE  
QUIT  
```
SEARCH_KEY means searching key by skiplist. SEARCH_KEY_FAST means searching key by hash map.  
Fig. 1 shows an example of using   
![server_mode](./img/Server_mode.png)  
<center> FIg 1 </center>  

## Performance  
| threads | Element numbers (k) | Insert time (seconds) | Search time (seconds) | Search Fast time (seconds) | Size (k) |
|---------|----------------------------|----------------------|----------------------|---------------------------|-----------------|
| 1       | 10                         | 0.0708547            | 0.00622124            | 0.00476512                | 10              |
| 10      | 10                         | 0.0515192           | 0.00337938            | 0.00333223                | 10              |
| 1       | 100                        | 0.688948              | 0.0815376              | 0.0559066                 | 100             |
| 10      | 100                        | 0.707738             | 0.0328814              | 0.0321823                | 100             |
| 1       | 1000                        | 6.55504             |1.26442              | 0.561901               | 1000             |
| 10      | 1000                        |8.22847              | 0.361909              | 0.30143                 | 1000             |


Using hash map to search the key can save time.  
## Todo
Using thread pools  
Using  Epoll to monitor the events  
Using redirection for output instead of conditional compilation  
Automatically decide whether the hash map should be used
