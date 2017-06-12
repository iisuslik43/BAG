//
// Created by iisus on 30.04.2017.
//

#include "headers.h"
#include "config.h"


#ifndef BAG2_WRITING_H
#define BAG2_WRITING_H


void writing_chunk(Chunk& ch, std::ifstream& in, std::ofstream& out, std::map<int32_t, Connection>& unique_connections);
void write_data(Header& h, std::ifstream& in, std::ofstream& out);
void write_chunk_info_data(Chunk_info& cih, std::ifstream& in, std::ofstream& out);
void write_index_data(Index_data& idh, std::ifstream& in, std::ofstream& out);
void writingBag(std::vector<Chunk> chunks, Bag_header bh, Config conf, std::ifstream& in, std::ofstream& out);
int32_t get_file_size(std::string filename);
void read_BAG(Config& conf, std::vector<Chunk>& chunks, Bag_header& bh, std::ifstream& in, std::ofstream& out);
#endif //BAG2_WRITING_H