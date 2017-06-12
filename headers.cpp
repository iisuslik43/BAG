//
// Created by iisus on 23.04.2017.
//

#include "headers.h"


using namespace std;


//--------------------------

void write_time(std::string s, std::pair<uint32_t, uint32_t > time, std::ofstream& out){
    int32_t size = s.size() + 8;
    out.write((char*)&size, sizeof(int32_t));
    for(int i = 0; i < s.size(); i++){
        out.write(&s[i], sizeof(char));
    }
    out.write((char*)&time.first, sizeof(uint32_t));
    out.write((char*)&time.second, sizeof(uint32_t));
}

int8_t get_op(ifstream& in)
{

    int64_t header_begin = in.tellg();
    int32_t header_len;
    int8_t op;
    in.read((char*)&header_len, sizeof(int32_t));
    int32_t j = 0;
    while(j < header_len)
    {
        int32_t field_len;
        in.read((char*)&field_len, sizeof(int32_t));
        string name;
        for(int i = 0; i < 3; i++)
        {
            int8_t next;
            in.read((char*)&next, sizeof(int8_t));
            name += next;
        }
        //cout << name << endl;
        if(name == "op=")
        {
            in.read((char*)&op, sizeof(int8_t));
            break;
        }
        in.seekg( field_len - 3, ios::cur);
        j += 4 + field_len;

    }
    in.seekg(header_begin, ios::beg);
    //cout << "0x" << hex << (int)op << endl;
    //cout << dec;
    return op;
}

void print_op(int8_t op)
{
    cout << "0x" << hex << (int)op << endl;
    cout << dec;
}

string get_name(ifstream& in)
{
    string name;
    size_t i = 0;
    for(;; i++)
    {
        int8_t next;
        in.read((char*)&next, sizeof(int8_t));
        if(next == '=')
        {
            break;
        }
        else
        {
            name += next;
        }
    }
    return name;
}
void print_time(std::pair<uint32_t, uint32_t > time, ostream& out){
    out << "sec: " << time.first << " nsec: " << time.second << endl;

}
int64_t skip_header(ifstream& in)
{
    int32_t temp;
    int64_t size = 0;
    in.read((char*)&temp, sizeof(int32_t));
    in.seekg(temp, ios::cur);
    size += temp + 4;
    in.read((char*)&temp, sizeof(int32_t));
    in.seekg(temp, ios::cur);
    size += temp + 4;
    return size;
}

void write_string(string& s, ofstream& out){
    int32_t size = s.size();
    out.write((char*)&size, sizeof(int32_t));
    for(int i = 0; i < s.size(); i++){
        out.write(&s[i], sizeof(char));
    }
}

void write_l(string& s, int32_t  f, ofstream& out){
    int32_t size = s.size() + 4;
    if(s != "")
        out.write((char*)&size, sizeof(int32_t));
    for(int i = 0; i < s.size(); i++){
        out.write(&s[i], sizeof(char));
    }
    out.write((char*)&f, sizeof(int32_t));
}

void write_ll(string& s, int64_t f, ofstream& out){
    int32_t size = s.size() + 8;
    out.write((char*)&size, sizeof(int32_t));
    for(int i = 0; i < s.size(); i++){
        out.write(&s[i], sizeof(char));
    }
    out.write((char*)&f, sizeof(int64_t));
}
//--------------------------


ifstream& operator>>(ifstream& in, Chunk_info & cih)
{
    in.read((char*)&cih.header_len, sizeof(int32_t));
    for(int g = 0; g < cih.header_len;)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        g += 4;
        string name = get_name(in);
        if(name == "op")
        {
            in.read((char*)&cih.op, sizeof(uint8_t));
        }
        else if(name == "ver")
        {
            in.read((char*)&cih.ver, sizeof(int32_t));
        }
        else if(name == "chunk_pos")
        {
            in.read((char*)&cih.chunk_pos, sizeof(int64_t));
        }
        else if(name == "start_time")
        {
            in.read((char*)&cih.start_time.first, sizeof(uint32_t));
            in.read((char*)&cih.start_time.second, sizeof(uint32_t));
        }
        else if(name == "end_time")
        {
            in.read((char*)&cih.end_time.first, sizeof(uint32_t));
            in.read((char*)&cih.end_time.second, sizeof(uint32_t));
        }
        else if(name == "count")
        {
            in.read((char*)&cih.count, sizeof(int32_t));
        }
        g += len;

    }
    in.read((char*)&cih.data_len, sizeof(int32_t));
    cih.data_begin = in.tellg();
    for(int32_t i = 0; i < cih.count; i++){
        int32_t conn;
        int32_t count;
        in.read((char*)&conn, sizeof(int32_t));
        in.read((char*)&count, sizeof(int32_t));
        cih.data.push_back({conn, count});
    }
    return in;
}

ostream& operator<<(ostream& out, Chunk_info & cih)
{
    out << "ver: " << cih.ver << endl;
    out << "chunk_pos: " << cih.chunk_pos << endl;
    out << "start_time: ";
    print_time(cih.start_time, cout);
    out << "end_time: ";
    print_time(cih.end_time, cout);
    out << "count: " << cih.count << endl;
}

std::ofstream& operator<<(std::ofstream& out, Chunk_info& cih){
    out.write((char*)&cih.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)cih.op;
    write_string(s, out);
    s = "ver=";
    write_l(s, cih.ver, out);

    s = "chunk_pos=";
    write_ll(s, cih.chunk_pos, out);

    s = "start_time=";
    write_time(s, cih.start_time, out);

    s = "end_time=";
    write_time(s, cih.end_time, out);

    s = "count=";
    write_l(s, cih.count, out);
    out.write((char*)&cih.data_len, sizeof(int32_t));
    return out;
}


ifstream& operator>>(ifstream& in, Index_data & idh)
{
    in.read((char*)&idh.header_len, sizeof(int32_t));
    for(int g = 0; g < idh.header_len;)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        g += 4;
        string name = get_name(in);
        if(name == "op")
        {
            in.read((char*)&idh.op, sizeof(uint8_t));
        }
        else if(name == "ver")
        {
            in.read((char*)&idh.ver, sizeof(int32_t));
        }
        else if(name == "conn")
        {
            in.read((char*)&idh.conn, sizeof(int32_t));
        }
        else if(name == "count")
        {
            in.read((char*)&idh.count, sizeof(int32_t));
        }
        g += len;

    }
    in.read((char*)&idh.data_len, sizeof(int32_t));
    idh.data_begin = in.tellg();
    for(int32_t i = 0; i < idh.count; i++){
        pair<uint32_t , uint32_t > time;
        int32_t offset;
        in.read((char*)&time.first, sizeof(uint32_t));
        in.read((char*)&time.second, sizeof(uint32_t));
        in.read((char*)&offset, sizeof(int32_t));
        idh.data.push_back({time, offset});
    }
    return in;
}

ostream& operator<<(ostream& out, Index_data & idh)
{
    out << "ver: " << idh.ver << endl;
    out << "conn: " << idh.conn << endl;
    out << "count: " << idh.count << endl;
}

std::ofstream& operator<<(std::ofstream& out, Index_data& idh){
    out.write((char*)&idh.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)idh.op;
    write_string(s, out);
    s = "ver=";
    write_l(s, idh.ver, out);
    s = "conn=";
    write_l(s, idh.conn, out);
    s = "count=";
    write_l(s, idh.count, out);
    out.write((char*)&idh.data_len, sizeof(int32_t));
    return out;
}



ifstream& operator>>(ifstream& in, Connection& coh)
{
    in.read((char*)&coh.header_len, sizeof(int32_t));
    for(int g = 0; g < coh.header_len;)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        g += 4;
        string name = get_name(in);
        if(name == "op")
        {
            in.read((char*)&coh.op, sizeof(uint8_t));
        }
        else if(name == "topic")
        {
            for(int i = name.size() + 1; i < len; i++)
            {
                int8_t next;
                in.read((char*)&next, sizeof(int8_t));
                coh.topic += next;
            }
        }
        else if(name == "conn")
        {
            in.read((char*)&coh.conn, sizeof(int32_t));
        }
        g += len;

    }
    in.read((char*)&coh.data_len, sizeof(int32_t));
    coh.data_begin = in.tellg();
    return in;
}

ostream& operator<<(ostream& out, Connection& coh)
{
    out << "topic: " << coh.topic << endl;
    out << "conn: " << coh.conn << endl;
}

std::ofstream& operator<<(std::ofstream& out, Connection& ch){
    out.write((char*)&ch.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)ch.op;
    write_string(s, out);
    s = "conn=";
    write_l(s, ch.conn, out);
    s = "topic=";
    s += ch.topic;
    write_string(s, out);
    out.write((char*)&ch.data_len, sizeof(int32_t));
    return out;
}



ostream& operator<<(ostream& out, Bag_header& bh)
{
    out << "chunk_count: " << bh.chunk_count << endl;
    out << "conn_count: " << bh.conn_count << endl;
    out << "index_pos: " << bh.index_pos << endl;
    return out;
}

ifstream& operator>>(ifstream& in, Bag_header& bh)
{
    in.read((char*)&bh.header_len, sizeof(int32_t));
    for(int g = 0; g < 4; g++)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        string name;
        size_t i = 0;
        for(; i < len; i++)
        {
            int8_t next;
            in.read((char*)&next, sizeof(int8_t));
            if(next == '=')
            {
                break;
            }
            else
            {
                name += next;
            }
        }
        if(name == "op")
        {
            in.read((char*)&bh.op, sizeof(uint8_t));
        }
        else if(name == "chunk_count")
        {
            in.read((char*)&bh.chunk_count, sizeof(int32_t));
        }
        else if(name == "conn_count")
        {
            in.read((char*)&bh.conn_count, sizeof(int32_t));
        }
        else if(name == "index_pos")
        {
            in.read((char*)&bh.index_pos, sizeof(uint64_t));
        }
    }
    in.read((char*)&bh.data_len, sizeof(int32_t));
    for(int i = 0; i < bh.data_len; i++)
    {
        int8_t fuck;
        in.read((char*)&fuck, sizeof(int8_t));
    }
    bh.data_begin = in.tellg();
    return in;
}

ofstream& operator<<(ofstream& out, Bag_header& bh){
    out.write((char*)&bh.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)bh.op;
    write_string(s, out);
    s = "index_pos=";
    write_ll(s, bh.index_pos, out);
    s = "conn_count=";
    write_l(s, bh.conn_count, out);
    s = "chunk_count=";
    write_l(s, bh.chunk_count, out);
    out.write((char*)&bh.data_len, sizeof(int32_t));
    return out;
}

ifstream& operator>>(ifstream& in, Chunk& ch)
{
    in.read((char*)&ch.header_len, sizeof(int32_t));
    for(int g = 0; g < 3; g++)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        string name = get_name(in);
        if(name == "op")
        {
            in.read((char*)&ch.op, sizeof(uint8_t));
        }
        else if(name == "compression")
        {
            for(int i = name.size() + 1; i < len; i++)
            {
                int8_t next;
                in.read((char*)&next, sizeof(int8_t));
                ch.compression += next;
            }
        }
        else if(name == "size")
        {
            in.read((char*)&ch.size, sizeof(int32_t));
        }
    }
    in.read((char*)&ch.data_len, sizeof(int32_t));
    ch.data_begin = in.tellg();
    return in;
}

ostream& operator<<(ostream& out, Chunk& ch)
{
    out << "size: " << ch.size << endl;
    out << "compression: " << ch.compression << endl;
}

std::ofstream& operator<<(std::ofstream& out, Chunk& ch){
    out.write((char*)&ch.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)ch.op;
    write_string(s, out);
    s = "compression=" + ch.compression;
    write_string(s, out);
    s = "size=";
    write_l(s, ch.size, out);
    out.write((char*)&ch.data_len, sizeof(int32_t));
    return out;
}



std::ifstream& operator>>(std::ifstream& in, Message_header& mh){
    in.read((char*)&mh.header_len, sizeof(int32_t));
    for(int g = 0; g < 3; g++)
    {
        int32_t len;
        in.read((char*)&len, sizeof(int32_t));
        string name = get_name(in);
        if(name == "op")
        {
            in.read((char*)&mh.op, sizeof(uint8_t));
        }
        else if(name == "conn")
        {
            in.read((char*)&mh.conn, sizeof(int32_t));
        }
        else if(name == "time")
        {
            in.read((char*)&mh.time.first, sizeof(uint32_t));
            in.read((char*)&mh.time.second, sizeof(uint32_t));
        }
    }
    in.read((char*)&mh.data_len, sizeof(int32_t));
    mh.data_begin = in.tellg();
    return in;
}
std::ostream& operator<<(std::ostream& out, Message_header& mh){
    out << "conn: " << mh.conn << endl;
    print_time(mh.time, out);
    return out;
}

std::ofstream& operator<<(std::ofstream& out, Message_header& mh){
    out.write((char*)&mh.header_len, sizeof(int32_t));
    string s = "op=";
    s += (char)mh.op;
    write_string(s, out);
    s = "conn=";
    write_l(s, mh.conn, out);
    s = "time=";
    write_time(s, mh.time, out);
    out.write((char*)&mh.data_len, sizeof(int32_t));
    return out;
}