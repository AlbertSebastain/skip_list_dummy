# ifndef SKIP_LIST_H
# define SKIP_LIST_H
# include <sys/socket.h>
# include <cstdio>
# include <pthread.h>
# include <time.h>
# include <string.h>
# include <typeinfo>
# include <iostream>
#  include <memory>
# include <sstream>
# include <vector>
# include <fstream>
# include <mutex>
# include <unordered_map>

# include "config.h"


using namespace std;
template<typename V> class Node;
template<typename V> class skiplist;
template<typename V> class help_send_str;

template<typename V> 
class Node
{
    public:
        Node(int);
        Node(float key, V value, int );
        ~Node();
        float show_key() const; 
        V show_value() const;
        bool set_value(V);
        bool set_key(float);
        int show_level() const;
        vector<Node<V> *> forward;
    private:
        float m_key;
        V m_value;
        int m_level;
};
template< typename V>
class skiplist
{
    public:
        skiplist(int max_level):  m_max_level(max_level), m_item_num(0),m_conn_fd(0),head(nullptr) {initial_skiplist();}
        ~skiplist();
        bool search_key(float key, V& value);
        bool search_key_fast(float key, V& value);
        bool insert_item(float key, V valule, int node_level = 0);
        bool delete_item(float key);
        void show_skiplist();
        int size();
        void initial_skiplist();
        void save_file(string);
        void load_file(string);
        void set_server_mode(int);
        friend class help_send_str<V>;
    private:
        int get_random_level();
        bool valid_string(string& str);
        bool parse_string(string& , float& , V& , int& );
        int m_max_level;
        int m_item_num;
        int m_skip_list_level;
        int m_conn_fd;
        bool server_mode;
        stringstream buffer;
        Node<V>* head;
        mutex m_mutex;
        unordered_map<float,Node<V>*> m_map;
};
template<typename V>
class help_send_str
{
    public:
        help_send_str(skiplist<V>* ptr): m_ptr(ptr) {};
        ~help_send_str()
        {
            send_str();
        }
    
    private:
        skiplist<V>* m_ptr;
        void send_str()
        {
            if((m_ptr->server_mode) && (! m_ptr->buffer.str().empty()))
            {
                send(m_ptr->m_conn_fd, m_ptr->buffer.str().c_str(),m_ptr->buffer.str().size(),0);
            }
            m_ptr->buffer.str("");
        }
};
template< typename V>
Node<V>::Node(int level): forward(level+1),  m_level(level)
{
}
template<typename V>
Node<V>::Node(float key, V value, int level): forward(level+1),  m_key(key), m_value(value),m_level(level)
{
}
template< typename V>
Node<V>::~Node()
{

}
template<typename V>
float Node<V>::show_key() const
{
    return m_key;
}
template<typename V>
V Node<V>::show_value() const
{
    return m_value;
}
template< typename V>
int Node<V>::show_level() const
{
    return m_level;
}
template< typename V>
bool Node<V>::set_key(float key)
{
    m_key = key;
    return true;
}
template<typename V>
bool Node<V>::set_value(V value)
{
    m_value = value;
    return true;
}

template<typename V>
bool skiplist<V>::search_key(float key, V& value)
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    Node<V>* cur;
    cur = head;
    for(int i=m_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i] != nullptr && key > cur->forward[i]->show_key())
        {
            cur = cur->forward[i];
        }
    }
    cur = cur->forward[0];
    if(cur == nullptr || cur->show_key() != key)
    {
        # ifndef SERVER
        cout<<"cannot find key"<<endl;
        # else
        buffer<<"cannot find key\n";
        #endif
        return false;
    }
    else
    {
        value = cur->show_value();
        return true;
    }
}
template< typename V>
void skiplist<V>::initial_skiplist()
{
    if(head != nullptr)
    {
        Node<V>* cur;
        cur = head;
        while(cur != nullptr)
        {
            Node<V>* pre;
            pre = cur;
            cur = cur->forward[0];
            delete pre;
        }
    }
    buffer.clear();
    m_item_num = 0;
    m_skip_list_level = 0;
    head = nullptr;
    head = new Node<V>(m_max_level);
    m_map.clear();
}
template<typename V>
int skiplist<V>::get_random_level()
{
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < m_max_level) ? k : m_max_level;
    return k;
}
template<typename V>
int skiplist<V>::size()
{
    return m_item_num;
}
template< typename V>
bool skiplist<V>::insert_item(float key, V value, int node_level)
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    lock_guard<mutex> guard(m_mutex);
    vector<Node<V>*> update(m_max_level+1);
    Node<V>* cur = head;
    if(m_map.find(key) != m_map.end())
    {
        # ifndef SERVER
        cout<<"insert item fail, key is already in skiplist"<<endl;
        # else
        buffer<<"insert item fail, key is already in skiplist\n";
        # endif
        return false;
    }
    for(int i=m_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i] != nullptr && key > cur->forward[i]->show_key())
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }
    cur = cur->forward[0];
    if(node_level == 0)
    {
    node_level = get_random_level();
    }
    Node<V>* add_node = new Node<V>(key, value, node_level);
    if(node_level > m_skip_list_level)
    {
        for(int i=m_skip_list_level+1;i<=node_level;i++)
        {
            update[i] = head;
        }
        m_skip_list_level = node_level;
    }
    for(int i=0;i<=node_level;i++)
    {
        add_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = add_node;

    }
    m_map[key] = add_node;
    m_item_num++;
    # ifndef SERVER
    cout<<"successfully add node"<<endl;
    # else
    buffer << "successfully add node\n";
    # endif
    return true;
}
template<typename V>
bool skiplist<V>::delete_item(float key)
{
    #ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    lock_guard<mutex> guard(m_mutex);
    if(m_map.find(key) == m_map.end())
    {
        # ifndef SERVER
        cout<<"delete node fail, cannot find the node"<<endl;
        # else
        buffer<< "delete node fail, cannot find the node\n";
        # endif
        return false;
    }
    vector<Node<V>*> update(m_max_level+1);
    Node<V>* cur = head;
    for(int i=m_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i] != nullptr && key > cur->forward[i]->show_key())
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }
    cur = cur->forward[0];
    if(cur != nullptr && cur->show_key() == key)
    {
        for(int i=0;i<=m_skip_list_level;i++)
        {
            if(update[i]->forward[i] == nullptr || update[i]->forward[i]->show_key() != key)
            {
                break;
            }
            update[i]->forward[i] = cur->forward[i];
        }
        while(m_skip_list_level > 0 && head->forward[m_skip_list_level] == nullptr)
        {
            m_skip_list_level--;
        }
        # ifndef SERVER
        cout<<"successfully delete node"<<endl;
        # else
        buffer<< "successfully delete node\n";
        # endif
        m_item_num--;
        Node <V>* pre_node = m_map[key];
        delete pre_node;
        m_map.erase(key);
    }
    return true;
} 
template<typename V>
void skiplist<V>::show_skiplist()
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    # ifndef SERVER
    cout<<"---------skip list-------------"<<endl;
    # else
    buffer<<"---------skip list-------------\n";
    # endif
    Node<V>* cur;
    for(int i=m_skip_list_level;i>=1;i--)
    {
        cur = head->forward[i];
        # ifndef SERVER
            cout<<"level"<<i<<":";
        # else
        buffer<< "level"<<i<<":";
        # endif
        while(cur != nullptr)
        {
            # ifndef SERVER
            cout<<cur->show_key()<<", ";
            # else
            buffer<<cur->show_key()<<",";
            # endif
            cur = cur->forward[i];
        }
        # ifndef SERVER
        cout<<"\n";
        # else
        buffer<<"\n";
        # endif
    }
    cur = head->forward[0];
    # ifndef SERVER
    cout<<endl;
    cout<<"level 0: ";
    # else
    buffer<<"\n";
    buffer<<"level 0:";
    # endif
    while(cur != nullptr)
    {  
        # ifndef SERVER
        cout<<cur->show_key()<<": "<<cur->show_value()<<", ";
        # else
        buffer<<cur->show_key()<<":"<<cur->show_value()<<",";
        # endif
        cur = cur->forward[0];
    }
    # ifndef SERVER
    cout<<endl;
    # else
    buffer<< "\n";
    # endif
    # ifndef SERVER
    cout<<"--------Skip list show finish--------"<<endl;
    # else
    buffer<< "--------Skip list show finish--------\n";
    # endif
}
template< typename V>
bool skiplist<V>::search_key_fast(float key, V& value)
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    Node<V>* node;
    if(m_map.find(key) != m_map.end())
    {
        node = m_map[key];
        value = node->show_value();
        return true;
    }
    else
    {
        # ifndef SERVER
        cout<<"cannot find the key"<<endl;
        # else
        buffer<< "cannot find the key\n";
        # endif
        return false;
    }
}
template<typename V>
void skiplist<V>::save_file(string path)
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    ofstream writer;
    writer.open(path);
    # ifndef SERVER
    cout<<"start save file in "<<path<<endl;
    # else
    buffer<< "start save file in " <<path <<"\n";
    # endif
    Node<V>* cur;
    cur = head;
    cur = cur->forward[0];
    while(cur != nullptr)
    {
        writer<<cur->show_level()<<":"<<cur->show_key()<<":"<<cur->show_value()<<"\n";
        cout<<cur->show_level()<<":"<<cur->show_key()<<":"<<cur->show_value()<<"\n";
        cur = cur->forward[0];
    }
    # ifndef SERVER
    cout<<"successfully saved the skiplist"<<endl;
    # else
    buffer<< "successfully saved the skiplist\n";
    # endif
    writer.flush();
    writer.close();
}
template<typename V>
bool skiplist<V>::parse_string(string& str, float& key, V& value, int& level)
{
    int pos = 0;
     int pos_key;
    string key_str;
    if(str.find(':') != str.npos)
    {
        pos = str.find(':');
        level = stoi(str.substr(0,pos));
    }
    else
    {
        return false;
    }
    if(str.find(':',pos+1) != str.npos)
    {
        pos_key = str.find(':',pos+1);
        key_str = str.substr(pos+1, pos_key-pos-1);
        key = atof(key_str.c_str());
    }
    else
    {
        return false;
    }
    value = str.substr(pos_key+1);
    return true;
}
template<typename V>
void skiplist<V>::load_file(string path)
{
    # ifdef SERVER
    help_send_str<V> send_str(this);
    # endif
    if(typeid(V) != typeid(string))
    {
        cout<<"when you load file you must keep the type of   value as string"<<endl;
        return;
    }
    # ifndef SERVER
    cout<<"start load file, the skip list will be initialed. All the nodes will be deleted. please  save the skip list when necessary."<<endl;
    # else
    buffer<< "start load file, the skip list will be initialed. All the nodes will be deleted. please  save the skip list when necessary.\n";
    # endif
    initial_skiplist();
    ifstream reader;
    reader.open(path);
    string str;
    float key;
    string value;
    int level;
    while(getline(reader, str))
    {
        parse_string(str,key, value, level);
        cout<<"key is "<<key<<", "<<"value is "<<value<<","<<"level is "<<level<<endl;

        insert_item(key, value,level);
    }
    # ifndef SERVER
    cout<<"load file successfully"<<endl;
    # else
    buffer<< "load file successfully\n";
    # endif
    reader.close();
}
template<typename V>
skiplist<V>::~skiplist()
{
    Node<V>* cur;
    Node<V>* pre;
    cur = head;
    while(cur != nullptr)
    {
        pre = cur;
        cur = cur->forward[0];
        delete pre;
    }
}
template<typename V>
void skiplist<V>::set_server_mode(int conn_fd)
{
    m_conn_fd = conn_fd;
    server_mode = true;
}
#endif

