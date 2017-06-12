//
// Created by iisus on 24.04.2017.
//

#ifndef BAG2_CONFIG_H
#define BAG2_CONFIG_H
#include "headers.h"

class Config{
public:
    Config(int argc, char** argv);
    void read_options(std::ifstream& in);
    void parseTime(std::string time);
    int32_t parseConnections(std::ifstream& in);
    bool parseConnection(Connection& ch, std::ifstream& in);
    bool check_message(std::pair<uint32_t, uint32_t> time, std::ifstream& in, int64_t pos);
    void parseChunk(Chunk& ch, std::ifstream& in);
    void parseBag(std::vector<Chunk>& chunks, Bag_header& bh, std::ifstream& in);

    std::pair<uint32_t, uint32_t > start_time;
    std::pair<uint32_t, uint32_t > end_time;
    std::string topic_regexp;
    std::string message_regexp;
    std::string connection_regexp;
    std::string in_filename;
    std::string out_filename_correct;
    std::string out_filename_other;
    std::map<int32_t, bool> taken;
    std::map<int32_t, bool> take;
    std::map<int32_t, Connection> unique_connections;
    bool correct_conditions;
};

std::string get_data(Header& h, std::ifstream& in);
int64_t string_to_long_long(std::string s);;
#endif //BAG2_CONFIG_H
