//
// Created by iisus on 24.04.2017.
//

#include "config.h"

using namespace std;


std::string get_data(Header& h, std::ifstream& in){
    string res = "";
    in.seekg(h.data_begin, ios::beg);
    for (int32_t i = 0; i < h.data_len; i++) {
        int8_t c;
        in.read((char *) &c, sizeof(int8_t));
        res += c;
    }
    return res;
}

int32_t get_index_of_id(vector<pair<int32_t, int32_t>>& data, int32_t id){
    for(int32_t i = 0; i < data.size(); i++){
        if(data[i].first == id){
            return i;
        }
    }
    return -1;
}

void comp_time(pair<uint32_t, uint32_t>& t, pair<uint32_t, uint32_t>& min, pair<uint32_t, uint32_t>& max){
    //if(t.first <)
    pair<uint32_t, uint32_t>f = std::min(t, min);
    min.first = f.first;
    min.second = f.second;
    pair<uint32_t, uint32_t>s = std::max(t, max);
    max.first = s.first;
    max.second = s.second;
}

void Config::parseChunk(Chunk& ch, ifstream& in){

    ifstream unarch(ch.unarch_file, ios::binary);
    unarch.seekg(0, ios::end);
    ifstream* real_in;
    if(ch.compression == "bz2")
        real_in = &unarch;
    else
        real_in = &in;
    ch.data_len = 0;
    for(int32_t i = 0; i < ch.connections_info.size(); i++){
        int32_t id = ch.connections_info[i].conn;
        if(take[id] || !correct_conditions) {
            Index_data& idh = ch.connections_info[i];
            ch.info.start_time.first = UINT32_MAX;
            ch.info.start_time.second = UINT32_MAX;
            ch.info.end_time.first = 0;
            ch.info.end_time.second = 0;
            for(size_t j = 0; j < idh.data.size(); j++){
                int64_t  check_pos = ch.compression == "bz2" ? idh.data[j].second : idh.data[j].second + ch.data_begin;
                if((check_message(idh.data[j].first, *real_in, check_pos)^ (!correct_conditions)) || (take[id] && !correct_conditions)){
                    comp_time(idh.data[j].first, ch.info.start_time, ch.info.end_time);
                    if(ch.compression == "bz2"){
                        unarch.seekg(idh.data[j].second, ios::beg);
                        ch.data_len += skip_header(unarch);
                    }
                    else{
                        in.seekg(idh.data[j].second + ch.data_begin, ios::beg);
                        ch.data_len += skip_header(in);
                    }
                }
                else{
                    idh.data.erase(idh.data.begin() + j--);
                    idh.count--;
                    idh.data_len -= 12;
                    ch.info.data[get_index_of_id(ch.info.data, id)].second--;
                }
            }
            if (!taken[id] && idh.data.size()) {
                taken[id] = true;
                ch.connections_to_write[id] = true;
                ch.data_len += unique_connections[id].all_size();
            }
        }
        else{
            ch.connections_info.erase(ch.connections_info.begin() + i--);
            ch.info.count--;
            ch.info.data_len -= 8;
        }

    }
    for(size_t i = 0; i < ch.info.data.size(); i++){
        if(ch.info.data[i].second == 0)
            ch.info.data.erase(ch.info.data.begin() + i--);
    }
    ch.size = ch.data_len;
}

bool Config::check_message(pair<uint32_t, uint32_t> time, ifstream& in, int64_t pos){
    if(!((time >= start_time) && (time <= end_time)))
        return false;
    Message_header mh;
    //cerr << "seek";
    in.seekg(0, ios::end);
    //cerr << in.tellg() << "||\n";
    in.seekg(pos, ios::beg);
    in >> mh;
    //cerr << "1\n";
    regex message_r(message_regexp);
    //if(!regex_match(get_data(mh, in), message_r))
    //    cout << "RRR" <<  get_data(mh, in);
    return regex_match(get_data(mh, in), message_r)|| message_regexp == "";
}

int64_t string_to_long_long(string s){
    int64_t res = 0;
    int64_t dec = 1;
    for(int i = s.size() - 1; i >= 0; i--){
        res += dec * (int64_t)(s[i] - '0');
        dec *= 10;
    }
    return res;
}

void Config::parseTime(std::string time){
    vector<string > times(4);
    if(time.empty())
        return;
    int ind = 0;
    string str_start_time, str_end_time;
    for(int i = 1; i < time.size()-1; i++){
        if(time[i] <= '9' && time[i] >= '0'){
            times[ind] += time[i];
        }
        else{
            ind++;
        }
    }
    start_time.first = (uint32_t)string_to_long_long(times[0]);
    start_time.second = (uint32_t)string_to_long_long(times[1]);
    end_time.first = (uint32_t)string_to_long_long(times[2]);
    end_time.second = (uint32_t)string_to_long_long(times[3]);
}

void Config::read_options(ifstream& in){
    string option;
    while(!in.eof()){
        getline(in, option);
        if(option == "time"){
            string time;
            getline(in, time);
            parseTime(time);
        }
        else if(option == "topic"){
            getline(in, topic_regexp);
        }
        else if(option == "message"){
            getline(in, message_regexp);
        }
        else if(option == "connection"){
            getline(in, connection_regexp);
        }
    }
}

Config::Config(int argc, char** argv):start_time({0,0}),end_time({UINT32_MAX, UINT32_MAX}),
                                      topic_regexp(""), message_regexp(""), connection_regexp(""){
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "--fromfile")){
            ifstream in(argv[++i]);
            read_options(in);
        }
        else if(!strcmp(argv[i], "--time")){
            parseTime(argv[++i]);
        }
        else if(!strcmp(argv[i], "--topic")){
            topic_regexp = argv[++i];
        }
        else if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")){
            i++;
            if(i >= argc){
                cout << "No in_file" << endl;
            }
            in_filename = argv[i];
        }
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--output_correct")){
            i++;
            if(i >= argc){
                cout << "No out_file" << endl;
            }
            out_filename_correct = argv[i];
        }
        else if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output_other")){
            i++;
            if(i >= argc){
                cout << "No out_file" << endl;
            }
            out_filename_other = argv[i];
        }
        else if(!strcmp(argv[i], "--mes")){
            message_regexp = argv[++i];
        }
        else if(!strcmp(argv[i], "--conn")){
            connection_regexp = argv[++i];
        }
        else{
            cout << argv[i] << " - Wrong Option" << endl;
        }
    }
}

bool Config::parseConnection(Connection& ch, ifstream& in){
    regex topic_r(topic_regexp);
    regex conn_r(connection_regexp);
    if(!regex_match(ch.topic, topic_r) && topic_regexp != "")
        return false;
    //if(!regex_match(get_data(ch, in), conn_r))
    //    cout << "RRR" <<  get_data(ch, in);
    return regex_match(get_data(ch, in), conn_r) || connection_regexp == "";
}

int32_t Config::parseConnections(ifstream& in){
    int32_t conn_count = 0;
    for(auto i = unique_connections.begin(); i != unique_connections.end(); ++i){
        int32_t id = i->second.conn;
        taken[id] = false;
        take[id] = parseConnection(i->second, in) ^ (!correct_conditions);
        if(take[id]){
            conn_count++;
        }
    }
    return conn_count;
}

void Config::parseBag(std::vector<Chunk>& chunks, Bag_header& bh, std::ifstream& in){
    bh.conn_count = parseConnections(in);
    bh.conn_count = 0;
    int64_t chunks_size = 0;
    for(int i = 0; i < chunks.size(); i++){
        Chunk& ch = chunks[i];
        parseChunk(chunks[i], in);
        if(chunks[i].size == 0){
            chunks.erase(chunks.begin() + i--);
            continue;
        }
        chunks_size += ch.all_size();
        for(int32_t j = 0; j < ch.connections_info.size(); j++){
            chunks_size += chunks[i].connections_info[j].all_size();
        }
    }
    for(auto it = unique_connections.begin(); it!= unique_connections.end();++it){
        if(taken[it->first])
            bh.conn_count++;
    }
    bh.index_pos = 13 + chunks_size + bh.all_size();
    bh.chunk_count = chunks.size();
}

