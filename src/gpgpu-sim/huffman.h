#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility> // for std::pair
#include <queue> // for priority_queue
#include <memory> // for shared_ptr
#include <bitset>

/**
 * Class for tracking symbol frequencies with automatic eviction
 * when the table grows beyond a specified size.
 */
class frequency_value_table {
private:
    // Key parameters
    size_t m_checking_table_size;  // Size threshold that triggers eviction
    size_t m_final_table_size;     // Size after eviction
    size_t m_symbol_length;        // Length of symbols in bits
    
    // Map to store symbol frequencies
    std::unordered_map<uint64_t, uint64_t> m_frequency_map;

    // Helper method to extract a symbol from a bit position in a byte array
    uint64_t extract_symbol_from_bitstream(const unsigned char* data, size_t size, size_t& bit_pos);

public:
    /**
     * Constructor for frequency_value_table
     * 
     * @param checking_table_size Size threshold that triggers eviction
     * @param final_table_size Size to reduce table to after eviction
     * @param symbol_length Length of symbols in bits
     */
    frequency_value_table(size_t checking_table_size, size_t final_table_size, size_t symbol_length);
    
    /**
     * Update the frequency of a symbol
     * 
     * @param symbol Symbol to update
     * @param increment Amount to increment (default 1)
     * @return Current frequency after update
     */
    uint64_t update_frequency(uint64_t symbol, uint64_t increment = 1);
    
    /**
     * Process a data entry by breaking it into m_symbol_length segments
     * and updating the frequency of each segment.
     * 
     * Designed for GPGPU-sim memory access patterns where data is read as bytes.
     * Each byte is treated as 8 bits, and bits are processed in sequence to form symbols
     * of length m_symbol_length.
     * 
     * @param data Pointer to the data block (array of bytes from memory)
     * @param size Size of the data block in bytes (typically 128 bytes for L2 cache lines)
     * @return Number of symbols processed
     */
    size_t process_data_block(const unsigned char* data, size_t size = 128);
    
    /**
     * Get the frequency of a symbol
     * 
     * @param symbol Symbol to check
     * @return Current frequency
     */
    uint64_t get_frequency(uint64_t symbol) const;
    
    /**
     * Evict entries to reduce table to final size
     */
    void evict_entries();
    
    /**
     * Get the current table size
     * 
     * @return Current number of entries
     */
    size_t size() const;
    
    /**
     * Get the checking table size
     * 
     * @return Checking table size
     */
    size_t checking_size() const;
    
    /**
     * Get the final table size
     * 
     * @return Final table size
     */
    size_t final_size() const;
    
    /**
     * Get the symbol length
     * 
     * @return Symbol length in bits
     */
    size_t symbol_length() const;

    // --------- Interface for Huffman code generation ---------

    /**
     * Get a sorted vector of symbol-frequency pairs
     * Sorted by frequency in ascending order (lowest frequency first)
     * 
     * @return Vector of pairs containing (symbol, frequency)
     */
    std::vector<std::pair<uint64_t, uint64_t>> get_sorted_frequencies() const;
    
    /**
     * Get the top most frequent entries, limited to final_table_size
     * If the current table size is larger than final_table_size, only return
     * the final_table_size most frequent entries.
     * 
     * @return Vector of pairs containing (symbol, frequency), sorted by frequency (highest first)
     */
    std::vector<std::pair<uint64_t, uint64_t>> get_top_frequencies() const;
    
    /**
     * Get an unsorted vector of symbol-frequency pairs
     * 
     * @return Vector of pairs containing (symbol, frequency)
     */
    std::vector<std::pair<uint64_t, uint64_t>> get_frequency_pairs() const;

    /**
     * Get the sum of all symbol frequencies currently stored in the table.
     *
     * @return Total number of observed symbols
     */
    uint64_t get_total_frequency() const;

    /**
     * Compute the Shannon entropy of the current symbol distribution.
     *
     * @return Entropy in bits per symbol
     */
    double compute_entropy() const;
    
    /**
     * Get a reference to the internal frequency map
     * 
     * @return Const reference to the frequency map
     */
    const std::unordered_map<uint64_t, uint64_t>& get_frequency_map() const;
    
    /**
     * Clear all frequencies in the table
     */
    void clear();

    /**
     * Age the tracked history by dividing every symbol count by the given
     * factor and dropping symbols whose counts become zero.
     *
     * @param divisor Value to divide each frequency by
     */
    void scale_down(uint64_t divisor);
    
    /**
     * Check if a specific symbol exists in the table
     * 
     * @param symbol Symbol to check
     * @return True if the symbol exists, false otherwise
     */
    bool contains(uint64_t symbol) const;
    
    /**
     * Get all symbols in the table
     * 
     * @return Vector of symbols
     */
    std::vector<uint64_t> get_symbols() const;
    
    /**
     * Get iterator to the beginning of the frequency map
     * Useful for C++11 range-based for loops
     * 
     * @return Iterator to the beginning
     */
    std::unordered_map<uint64_t, uint64_t>::const_iterator begin() const;
    
    /**
     * Get iterator to the end of the frequency map
     * Useful for C++11 range-based for loops
     * 
     * @return Iterator to the end
     */
    std::unordered_map<uint64_t, uint64_t>::const_iterator end() const;
};

/**
 * Structure to hold compression results including raw and effective metrics
 */
struct compress_outcome {
    size_t raw_compressed_size_bits;   // Size of the compressed data in bits
    double raw_compression_ratio;      // Original size / raw compressed size
    size_t mag_compressed_size_bytes;  // Size rounded up to MAG boundary
    size_t burst_size;                 // Rounded up multiple of MAG
    double effective_compression_ratio; // Original size / effective size
    
    // Default constructor
    compress_outcome() : 
        raw_compressed_size_bits(0),
        raw_compression_ratio(1.0),
        mag_compressed_size_bytes(0),
        burst_size(0),
        effective_compression_ratio(1.0) {}
};

/**
 * Class for creating and managing Huffman codes based on symbol frequencies.
 * Uses canonical Huffman coding for more efficient representation.
 */
class huffman_codebook {
private:
    // Node structure for building Huffman tree
    struct huffman_node {
        uint64_t symbol;           // Symbol value (only valid for leaf nodes)
        uint64_t frequency;        // Frequency of the symbol
        bool is_leaf;              // Whether this is a leaf node
        std::shared_ptr<huffman_node> left;  // Left child (0 bit)
        std::shared_ptr<huffman_node> right; // Right child (1 bit)
        
        // Constructor for internal nodes
        huffman_node(std::shared_ptr<huffman_node> l, std::shared_ptr<huffman_node> r)
            : symbol(0), frequency(l->frequency + r->frequency), is_leaf(false), left(l), right(r) {}
        
        // Constructor for leaf nodes
        huffman_node(uint64_t sym, uint64_t freq)
            : symbol(sym), frequency(freq), is_leaf(true), left(nullptr), right(nullptr) {}
        
        // Comparison operator for priority queue
        bool operator>(const huffman_node& other) const {
            return frequency > other.frequency;
        }
    };
    
    // Code entry containing code bits and code length
    struct code_entry {
        std::vector<bool> bits;   // Bit representation of the code (true=1, false=0)
        size_t length;            // Length of the code in bits
        
        // Default constructor
        code_entry() : length(0) {}
        
        // Constructor with bit vector
        code_entry(const std::vector<bool>& b) : bits(b), length(b.size()) {}
    };
    
    // Map of symbol to code
    std::unordered_map<uint64_t, code_entry> m_codes;
    
    // Maximum code length in the codebook
    size_t m_max_code_length;
    
    // Memory access granularity in bytes (default 32 bytes for GPU)
    size_t m_mag;
    
    // Build the Huffman tree from frequency pairs
    std::shared_ptr<huffman_node> build_tree(const std::vector<std::pair<uint64_t, uint64_t>>& freq_pairs);
    
    // Generate codes from the Huffman tree (non-canonical)
    void generate_codes_from_tree(std::shared_ptr<huffman_node> node, std::vector<bool>& code);
    
    // Shannon-Fano encoding helper function (recursive)
    void shannon_fano_encode(std::vector<std::pair<uint64_t, uint64_t>>& symbols, size_t start, size_t end, std::vector<bool> code);
    
    // Convert to canonical Huffman codes
    void make_canonical_codes();
    
public:
    /**
     * Compressor class for encoding data using Huffman codes
     */
    class compressor {
    private:
        const huffman_codebook& m_codebook;  // Reference to the parent codebook
        
    public:
        /**
         * Constructor for the compressor
         * 
         * @param codebook Reference to the parent huffman_codebook
         */
        compressor(const huffman_codebook& codebook) : m_codebook(codebook) {}
        
        /**
         * Compress a data block using the Huffman codebook
         * 
         * @param data Pointer to the data block (array of bytes)
         * @param size Size of the data block in bytes
         * @return Compression outcome with statistics
         */
        compress_outcome compress_data_block(const unsigned char* data, size_t size) const;
        
        /**
         * Extract symbols from a data block (similar to frequency_value_table::process_data_block)
         * 
         * @param data Pointer to the data block
         * @param size Size of the data block in bytes
         * @return Vector of extracted symbols
         */
        std::vector<uint64_t> extract_symbols(const unsigned char* data, size_t size) const;
        
        /**
         * Extract a symbol from a bit position in a byte array
         * (similar to frequency_value_table::extract_symbol_from_bitstream)
         * 
         * @param data Pointer to the data
         * @param size Size of the data in bytes
         * @param bit_pos Current bit position (will be updated)
         * @param symbol_length Length of symbol in bits
         * @return Extracted symbol
         */
        uint64_t extract_symbol_from_bitstream(const unsigned char* data, size_t size, 
                                              size_t& bit_pos, size_t symbol_length) const;
    };
    
    // Frequency table for symbol tracking
    frequency_value_table freq_table;
    
    // Compressor instance
    compressor m_compressor;
    
    /**
     * Constructor for huffman_codebook
     * 
     * @param checking_table_size Size threshold that triggers eviction
     * @param final_table_size Size to reduce table to after eviction
     * @param symbol_length Length of symbols in bits
     * @param mag Memory access granularity in bytes (default 32)
     */
    huffman_codebook(size_t checking_table_size, size_t final_table_size, size_t symbol_length, size_t mag = 32);
    
    /**
     * Generate Shannon-Fano codes based on the current frequency table
     * Uses get_top_frequencies() to focus on the most frequent symbols
     * 
     * @return Number of codes generated
     */
    size_t generate_shannon_fano_codes();
    
    /**
     * Generate Shannon codes based on the current frequency table
     * Assigns code lengths based on -log2(probability) and creates canonical prefix codes
     * Uses get_top_frequencies() to focus on the most frequent symbols
     * 
     * @return Number of codes generated
     */
    size_t generate_shannon_codes();
    
    /**
     * Generate Huffman codes based on the current frequency table
     * Uses get_top_frequencies() to focus on the most frequent symbols
     * 
     * @return Number of codes generated
     */
    size_t generate_huffman_codes();
    
    /**
     * Get the memory access granularity (MAG)
     * 
     * @return MAG in bytes
     */
    size_t get_mag() const;
    
    /**
     * Set the memory access granularity (MAG)
     * 
     * @param mag New MAG value in bytes
     */
    void set_mag(size_t mag);
    
    /**
     * Compress a data block using the current Huffman codebook
     * 
     * @param data Pointer to the data block
     * @param size Size of the data block in bytes
     * @return Compression outcome with statistics
     */
    compress_outcome compress_block(const unsigned char* data, size_t size) const;
    
    /**
     * Check if a symbol has a code in the codebook
     * 
     * @param symbol Symbol to check
     * @return True if the symbol has a code, false otherwise
     */
    bool has_code(uint64_t symbol) const;
    
    /**
     * Get the code for a symbol
     * 
     * @param symbol Symbol to look up
     * @return Bit vector containing the code (empty if symbol not found)
     */
    const std::vector<bool>& get_code_bits(uint64_t symbol) const;
    
    /**
     * Get the length of a symbol's code in bits
     * 
     * @param symbol Symbol to look up
     * @return Length of the code in bits (0 if symbol not found)
     */
    size_t get_code_length(uint64_t symbol) const;
    
    /**
     * Get a string representation of a symbol's code
     * 
     * @param symbol Symbol to look up
     * @return String of '0's and '1's representing the code (empty if symbol not found)
     */
    std::string get_code_string(uint64_t symbol) const;
    
    /**
     * Get the maximum code length in the codebook
     * 
     * @return Maximum code length in bits
     */
    size_t get_max_code_length() const;
    
    /**
     * Get all symbols that have codes in the codebook
     * 
     * @return Vector of symbols
     */
    std::vector<uint64_t> get_coded_symbols() const;
    
    /**
     * Get all codes in the codebook as (symbol, code_string) pairs
     * 
     * @return Vector of pairs containing (symbol, code_string)
     */
    std::vector<std::pair<uint64_t, std::string>> get_all_codes() const;
    
    /**
     * Clear the codebook (codes only, frequencies remain)
     */
    void clear_codes();
    
    /**
     * Clear both the codebook and frequency table
     */
    void clear_all();
    
    /**
     * Get the number of codes in the codebook
     * 
     * @return Number of codes
     */
    size_t size() const;
};




#endif
