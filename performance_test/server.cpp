# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstdio>
# include <unistd.h>
# include <cassert>
# include <signal.h>
# include <stdlib.h>
# include <cctype>
# include <errno.h>
# include <sstream>
# include <vector>
# include <iostream>
# include <string> 

# include  "skiplist.h"

# define SAVE_PATH "./save.config"
# define PORT 1232

bool valid_string(string&);

bool stop = false;

void signal_handler(int signal)
{
    stop = true;
}
void add_signal(int signal)
{
    struct sigaction act;
    act.sa_handler = signal_handler;
    if(sigaction(signal, &act, NULL) < 0)
    {
        exit(1);
    }
}
int parse_command(skiplist<string>* ptr_list, string command, string& value)
{
    stringstream str(command);
    vector<string> arg;
    while(!str.eof())
    {
        string cur;
        str>>cur;
        arg.push_back(cur);
    }
    int size;
    float key = 0.0;
    int res;
    size = arg.size();
    if(arg.empty())
    {
        return 0;
    }
    if(size >=  2)
    {
        if(valid_string(arg[1]))
        {
            stringstream str_num;
            str_num.str(arg[1]);
            str_num>>key;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "SEARCH_KEY")
    {
        if(size == 2)
        {
            res = ptr_list->search_key(key, value);
            return res;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "SEARCH_KEY_FAST")
    {
        if(size == 2)
        {
            res = ptr_list->search_key_fast(key, value);
            return res;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "INSERT_ITEM")
    {
        if(size == 3)
        {
            res = ptr_list->insert_item(key, arg[2]);
            return res;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "DELETE_ITEM")
    {
        if(size == 2)
        {
            res = ptr_list->delete_item(key);
            return res;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "SHOW_SKIPLIST")
    {
        if(size == 1)
        {
            ptr_list->show_skiplist();
            return 1;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "SIZE")
    {
        if(size == 1)
        {
            int val;
            val = ptr_list->size();
            value = to_string(val);
            return 1;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "SAVE_FILE")
    {
        if(size == 1)
        {
            ptr_list->save_file(SAVE_PATH);
            return 1;
        }
        else
        {
            return -1;
        }
    }
    if(arg[0] == "LOAD_FILE")
    {
        if(size == 1)
        {
            ptr_list->load_file(SAVE_PATH);
            return 1;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}
bool valid_string(string& str)
{
    int point = 0;
    for(char s:str)
    {
        if(isdigit(s))
        {
            continue;
        }
        else if(s == '.' && point == 0)
        {
            point = 1;
        }
        else
        {
            return false;
        }
    }
    return true;
}
int main()
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    int sock_fd;
    add_signal(SIGINT);
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);
    int ret;
    ret = 0;
    ret = bind(sock_fd, (struct sockaddr*) &address, sizeof(address));
    assert(ret != -1);
    ret = listen(sock_fd, 5);
    int conn_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    conn_fd = accept(sock_fd,(struct sockaddr*)&client_addr, &client_len);
    assert(ret != -1);
    skiplist<string> SkipListData(30);
    SkipListData.set_server_mode(conn_fd);
    char buffer[1024];
    while(! stop)
    {
        bzero(buffer, sizeof(buffer));
        ret = recv(conn_fd, buffer, sizeof(buffer),0);
        if(ret >= 0)
        {
            string str(buffer);
            while(*str.rbegin() == '\r' || *str.rbegin() == '\n')
            {
                str.pop_back();
            }
            // intermid<<buffer;
            // intermid>>str;
            // cout<<str<<endl;
            if(str == "quit")
            {
                stop = true;
            }
            else
            {
                 string value;
                int res;
                res = parse_command(&SkipListData, str, value);
                if(res == 1 && value.size() > 0)
                {
                    value += "\n";
                    send(conn_fd, value.c_str(),value.size(), 0);
                }
                else if(res == -1)
                {
                    char* send_i = (char*)"Invalid command.\n";
                    send(conn_fd, send_i, strlen(send_i),0);
                }
            }
        }
        else
        {
            stop = true;
        }
        
    }
    close(conn_fd);
    close(sock_fd);
}