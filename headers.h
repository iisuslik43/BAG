//
// Created by iisus on 23.04.2017.
//

#ifndef HEADERS_H
#define HEADERS_H

#include <regex>
#include <iostream>
#include <cstdio>
#include <stack>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include <fstream>
#include <string>


class Header
{
public:
    int32_t header_len;
    int32_t data_len;
    int64_t data_begin;
    int8_t op;
    int32_t all_size(){
        return 8 + header_len + data_len;
    }
};




//--------------------------
int8_t get_op(std::ifstream& in);
void print_op(int8_t op);
int64_t skip_header(std::ifstream& in);
void print_time(std::pair<uint32_t, uint32_t >, std::ostream& out);
void write_time(std::string s, std::pair<uint32_t, uint32_t > time, std::ofstream& out);
void write_string(std::string& s, std::ofstream& out);
void write_l(std::string& s, int32_t  f, std::ofstream& out);
void write_ll(std::string& s, int64_t f, std::ofstream& out);
//--------------------------

class Message_header:public Header
{
public:
    friend std::ifstream& operator>>(std::ifstream& in, Message_header& mh);
    friend std::ostream& operator<<(std::ostream& out, Message_header& mh);
    friend std::ofstream& operator<<(std::ofstream& out, Message_header& mh);

    int32_t conn;
    std::pair<uint32_t, uint32_t > time;
};
std::ifstream& operator>>(std::ifstream& in, Message_header& mh);
std::ostream& operator<<(std::ostream& out, Message_header& mh);
std::ofstream& operator<<(std::ofstream& out, Message_header& mh);

class Chunk_info:public Header
{
public:
    friend std::ifstream& operator>>(std::ifstream& in, Chunk_info & cih);
    friend std::ostream& operator<<(std::ostream& out, Chunk_info & cih);
    friend std::ofstream& operator<<(std::ofstream& out, Chunk_info& cih);
    int32_t ver;
    int64_t chunk_pos;
    std::pair<uint32_t, uint32_t > start_time;
    std::pair<uint32_t, uint32_t > end_time;
    int32_t count;
    std::vector<std::pair<int32_t, int32_t>> data;
};
std::ifstream& operator>>(std::ifstream& in, Chunk_info & cih);
std::ostream& operator<<(std::ostream& out, Chunk_info & cih);
std::ofstream& operator<<(std::ofstream& out, Chunk_info& cih);



class Index_data:public Header{;

public:
    friend std::ifstream& operator>>(std::ifstream& in, Index_data & idh);
    friend std::ostream& operator<<(std::ostream& out, Index_data& idh);
    friend std::ofstream& operator<<(std::ofstream& out, Index_data& idh);

    int32_t ver;
    int32_t conn;
    int32_t count;
    std::vector<std::pair<std::pair<uint32_t, uint32_t>, int32_t> > data;
};
std::ifstream& operator>>(std::ifstream& in, Index_data & idh);
std::ostream& operator<<(std::ostream& out, Index_data & idh);
std::ofstream& operator<<(std::ofstream& out, Index_data& idh);



class Connection:public Header
{
public:
    friend std::ifstream& operator>>(std::ifstream& in, Connection & coh);
    friend std::ostream& operator<<(std::ostream& out, Connection& coh);
    friend std::ofstream& operator<<(std::ofstream& out, Connection& ch);

    bool operator<(Connection & coh)
    {
        return conn < coh.conn;
    }
    void print_data(std::istream& in)
    {
        int64_t in_now = in.tellg();
        in.seekg(data_begin, std::ios::beg);
        for(int32_t i = 0; i < data_len; i++)
        {
            int8_t next;
            in.read((char*)& next, sizeof(int8_t));
            std::cout << next;
        }
        in.seekg(in_now, std::ios::beg);
    }
    std::string topic;
    int32_t conn;
};
std::ifstream& operator>>(std::ifstream& in, Connection& coh);
std::ostream& operator<<(std::ostream& out, Connection& coh);
std::ofstream& operator<<(std::ofstream& out, Connection& ch);




class Bag_header:public Header
{
public:
    friend std::ifstream& operator>>(std::ifstream& in, Bag_header& bh);
    friend std::ostream& operator<<(std::ostream& out, Bag_header& bh);
    friend std::ofstream& operator<<(std::ofstream& out, Bag_header& bh);

    int32_t chunk_count;
    int32_t conn_count;
    int64_t index_pos;
};
std::ostream& operator<<(std::ostream& out, Bag_header& bh);
std::ifstream& operator>>(std::ifstream& in, Bag_header& bh);
std::ofstream& operator<<(std::ofstream& out, Bag_header& bh);




class Chunk:public Header
{
public:
    friend std::ifstream& operator>>(std::ifstream& in, Chunk & ch);
    friend std::ostream& operator<<(std::ostream& out, Chunk& ch);
    friend std::ofstream& operator<<(std::ofstream& out, Chunk& ch);
    void read_connections(std::ifstream& in)
    {
        int64_t size = 0;
        while(size < data_len) //connection
        {
            op = get_op(in);
            if(op == 7)
            {
                Connection coh;
                in >> coh;
                std::cout << coh;
                in.seekg(coh.data_len, std::ios::cur);
                size += coh.data_len + coh.header_len + 8;
                connections.push_back(coh);
            }
            else if(op == 2) //message
            {
                size += skip_header(in);
            }
            else
            {
                std::cout << "Chunk has strange header" << std::endl;
            }
        }
        std::sort(connections.begin(), connections.end());
        connections_info.resize(connections.size());
    }


    std::vector<Connection> connections;
    std::vector<Index_data> connections_info;
    std::map<int32_t, bool> connections_to_write;
    Chunk_info info;
    int32_t size;
    std::string compression;
    std::string unarch_file;
    //std::ifstream unarch;
};
std::ifstream& operator>>(std::ifstream& in, Chunk& ch);
std::ostream& operator<<(std::ostream& out, Chunk& ch);
std::ofstream& operator<<(std::ofstream& out, Chunk& ch);

#endif

