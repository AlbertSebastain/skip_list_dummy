#include <iostream>
#include <chrono>
#include <cstdlib>
# include <fstream>
#include <pthread.h>
# include <thread>
#include <time.h>
# include <vector>

#include "../skiplist.h"
# include "config.h"

# define SAVE_PATH "/home/billy/code/skiplist_data/performance_test_result"
using namespace std;

skiplist<string> skipList(18);
void insertElement(int threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insert_item(rand() % TEST_COUNT, "a"); 
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
void test_insert(ostream& writer)
{
    vector<thread*> threads(NUM_THREADS);
    int i;
    auto start = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        threads[i] = new thread(insertElement, i);
    }
    for(int i=0;i<NUM_THREADS;i++)
    {
        if(threads[i]->joinable())
        {
            threads[i]->join();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    writer<< " elapsed:" << elapsed.count() << std::endl;
}
void test_search(ofstream& writer)
{
    for(int i=0;i<TEST_COUNT;i++)
    {
        skipList.insert_item(i,to_string(i));
    }
    vector<thread*> threads(NUM_THREADS);
    int i;
    auto start = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        threads[i] = new thread(getElement, i);
    }
    for(int i=0;i<NUM_THREADS;i++)
    {
        if(threads[i]->joinable())
        {
            threads[i]->join();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    writer << "elapsed:" << elapsed.count() << std::endl;
}
void test_search_fast(ofstream& writer)
{
    for(int i=0;i<TEST_COUNT;i++)
    {
        skipList.insert_item(i,to_string(i));
    }
    vector<thread*> threads(NUM_THREADS);
    int i;
    auto start = std::chrono::high_resolution_clock::now();
    for( i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        threads[i] = new thread(getElement_fast, i);
    }
    for(int i=0;i<NUM_THREADS;i++)
    {
        if(threads[i]->joinable())
        {
            threads[i]->join();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    writer<< " elapsed:" << elapsed.count() << std::endl;
}
int main() {
    ofstream writer;
    writer.open(SAVE_PATH,ios::app);
    # ifdef INSERT
    writer<<NUM_THREADS<<"threads"<<" ,"<<TEST_COUNT<<"times insert";
    test_insert(writer);
    # else 
        # ifdef SEARCH_FAST
        writer<<NUM_THREADS<<"threads"<<","<<TEST_COUNT<<"times search fast";
        test_search_fast(writer);
        # else
        writer<<NUM_THREADS<<"threads"<<","<<TEST_COUNT<<"times search";
        test_search(writer);
        #endif
    #endif
    srand (time(NULL));  
    
    return 0;

}