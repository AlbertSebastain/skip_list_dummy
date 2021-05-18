#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
# include <thread>
#include <time.h>
# include <vector>

# include "locker.h"
#include "skiplist.h"

#define NUM_THREADS 6
#define TEST_COUNT 100
using namespace std;

skiplist<string> skipList(18);
sem m_sem;
void insertElement(int threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insert_item(rand() % TEST_COUNT, "a"); 
	}
    if(threadid == NUM_THREADS-1)
    {
        m_sem.post();
    }
}

void getElement(int threadid) {
    long tid; 
    tid = (long)threadid;
    string value;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.search_key(rand() % TEST_COUNT,value); 
	}
}
void getElement_fast(int threadid) {
    long tid; 
    tid = (long)threadid;
    string value;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.search_key_fast(rand() % TEST_COUNT,value); 
	}
}

int main() {
    srand (time(NULL));  

    vector<thread*> threads(NUM_THREADS);
    int i;
    auto start = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        threads[i] = new thread(insertElement, i);
        threads[i]->detach();
    }
    m_sem.wait();
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    return 0;

}