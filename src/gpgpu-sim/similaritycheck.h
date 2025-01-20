#ifndef SIMILARITY_CHECK_H
#define SIMILARITY_CHECK_H

#include <unordered_map>
#include <string>
#include <list>

class similarity_cache {
public:
    similarity_cache(size_t cache_size) : max_cache_size(cache_size), m_total_request(0), num_hit(0) {};
    void check_and_update(std::string mem_str);
    int get_total_request(){ return m_total_request; } // Marked as const
    int get_num_hit(){ return num_hit; }
    int get_size(){ return max_cache_size; }
    void reset() { m_total_request = 0;num_hit = 0; }
    void clear() { cache_list.clear(); cache_map.clear(); }






private:
    std::unordered_map<std::string, std::list<std::string>::iterator> cache_map;
    std::list<std::string> cache_list;
    int m_total_request;
    int num_hit;
    int max_cache_size;
    void evict_LRU();




   /*similarity_cache() { m_total_request = 0; }
    //~similarity_cache(); // Uncomment if a custom destructor is needed
    int check_and_update(std::string mem_str);
    int total_request() const { return m_total_request; } // Marked as const
    int get_aggregate();
    int get_aggregate_without_zero();
    int get_count(std::string mem_str);
    void reset(){m_total_request = 0;}
    void clear(){ hash_table.clear();}
    // Hash table to store unique memory content (hashed as strings) and their occurrence counts
    std::unordered_map<std::string, int> hash_table;

private:

    // Helper function to convert memory content to a string
    std::string memory_to_string(const unsigned char* data, size_t size);

    int m_total_request;*/
};

#endif // SIMILARITY_CHECK_H
