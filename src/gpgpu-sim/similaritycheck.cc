#include "similaritycheck.h"

#include <sstream> 
#include <iomanip> 
#include <unordered_map>
#include <string>
#include <list>
/*
std::string similarity_cache::memory_to_string(const unsigned char* data, size_t size){
    bool all_zero = true;
    for (size_t i = 0; i < size; i++) {
        if (data[i] != 0) {
            all_zero = false;
            break;
        }
    }

    // If all elements are zero, return "0"
    if (all_zero) {
        return "0";
    }
    std::ostringstream oss;
    for (size_t i = 0; i < size; i++) {
        oss << std::hex << static_cast<int>(data[i]);
    }
    return oss.str();
}


int similarity_cache::check_and_update(std::string mem_str){
    //std::string mem_str = memory_to_string(data, size);
    if (hash_table.find(mem_str) != hash_table.end()) {
        hash_table[mem_str]++;
    } else {
        hash_table[mem_str] = 1;
    }
    if (mem_str != "0"){
        m_total_request++;
    }
    return hash_table[mem_str];


}

int similarity_cache::get_count(std::string mem_str){
    //std::string mem_str = memory_to_string(data, size);
    auto it = hash_table.find(mem_str);
    return (it != hash_table.end())?it->second : 0;

}

int similarity_cache::get_aggregate(){
    int aggregate = 0;
    for (auto it = hash_table.begin(); it != hash_table.end(); ++it) {
    if (it->second != 0) {  
        aggregate += 1;
    }
    }
    return aggregate;
}

int similarity_cache::get_aggregate_without_zero(){
    int aggregate = 0;
    for (auto it = hash_table.begin(); it != hash_table.end(); ++it) {
    if ((it->second != 0)&(it->first != "0")) {  
        aggregate += 1;
    }
    }
    return aggregate;
}*/

void similarity_cache::evict_LRU(){
    //if (cache_list.size() >= max_cache_size){
    if (!cache_list.empty() && cache_list.size() >  max_cache_size) {
        std::string lru_key = cache_list.back();
        //printf("size of cache_list is %d\n", cache_list.size());
        //printf("evicting %s\n", lru_key.c_str());
        cache_list.pop_back();
        cache_map.erase(lru_key);}
    //}
}

void similarity_cache::check_and_update(std::string mem_str){
    if (mem_str != "0"){
        m_total_request++;
    auto it = cache_map.find(mem_str);

    if (it != cache_map.end()) {
        num_hit++;
        // If the key exists, move it to the front of the list
        cache_list.erase(it->second);
        cache_list.push_front(mem_str);
        cache_map[mem_str] = cache_list.begin();
    } else {
        // If the key does not exist, add it to the front of the list
        cache_list.push_front(mem_str);
        cache_map[mem_str] = cache_list.begin();
    }
    evict_LRU();
    }
}