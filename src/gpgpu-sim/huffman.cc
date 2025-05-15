#include "huffman.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include <cmath>

// Constructor implementation
frequency_value_table::frequency_value_table(size_t checking_table_size, size_t final_table_size, size_t symbol_length)
    : m_checking_table_size(checking_table_size),
      m_final_table_size(final_table_size),
      m_symbol_length(symbol_length) {
    
    // Validate parameters
    if (final_table_size >= checking_table_size) {
        std::cerr << "Warning: final_table_size should be less than checking_table_size" << std::endl;
    }
    
    if (symbol_length > 64) {
        std::cerr << "Error: symbol_length cannot exceed 64 bits" << std::endl;
        throw std::invalid_argument("symbol_length too large");
    }
}

// Extract a symbol from a byte array starting at a specific bit position
uint64_t frequency_value_table::extract_symbol_from_bitstream(const unsigned char* data, size_t size, size_t& bit_pos) {
    uint64_t symbol = 0;
    
    for (size_t j = 0; j < m_symbol_length; ++j) {
        // Calculate which byte and which bit within that byte
        size_t byte_pos = bit_pos / 8;
        size_t bit_offset = 7 - (bit_pos % 8);  // MSB first (bit 7 is the highest bit in a byte)
        
        // Make sure we don't go beyond the data boundary
        if (byte_pos >= size) {
            bit_pos += (m_symbol_length - j);  // Skip remaining bits
            break;
        }
        
        // Extract the bit and add it to our symbol
        if (data[byte_pos] & (1 << bit_offset)) {
            symbol |= (1ULL << (m_symbol_length - 1 - j));
        }
        
        bit_pos++;
    }
    
    return symbol;
}

// Process a data block by segmenting into symbols
size_t frequency_value_table::process_data_block(const unsigned char* data, size_t size) {
    if (!data) {
        std::cerr << "Error: Null data pointer provided" << std::endl;
        return 0;
    }
    
    // Calculate total bits available in the data
    size_t total_bits = size * 8;
    
    // Calculate how many complete symbols we can extract
    size_t num_symbols = total_bits / m_symbol_length;
    if (total_bits % m_symbol_length != 0) {
        std::cerr << "Warning: Total bits not a multiple of symbol length" << std::endl;
    }
    
    // Track our position in the bitstream and count processed symbols
    size_t bit_pos = 0;
    size_t symbols_processed = 0;
    
    // Process each symbol
    for (size_t i = 0; i < num_symbols; ++i) {
        // Extract a symbol from the bitstream
        uint64_t symbol = extract_symbol_from_bitstream(data, size, bit_pos);
        
        // Update the frequency
        update_frequency(symbol);
        symbols_processed++;
    }
    
    return symbols_processed;
}

// Update frequency
uint64_t frequency_value_table::update_frequency(uint64_t symbol, uint64_t increment) {
    m_frequency_map[symbol] += increment;
    
    // Check if eviction is needed
    if (m_frequency_map.size() >= m_checking_table_size) {
        evict_entries();
    }
    
    return m_frequency_map[symbol];
}

// Get frequency
uint64_t frequency_value_table::get_frequency(uint64_t symbol) const {
    auto it = m_frequency_map.find(symbol);
    if (it != m_frequency_map.end()) {
        return it->second;
    }
    return 0;
}

// Evict entries
void frequency_value_table::evict_entries() {
    if (m_frequency_map.size() <= m_final_table_size) {
        return; // No need to evict
    }
    
    // Create vector of pairs (symbol, frequency)
    std::vector<std::pair<uint64_t, uint64_t>> freq_pairs;
    for (const auto& entry : m_frequency_map) {
        freq_pairs.push_back(entry);
    }
    
    // Sort by frequency (ascending)
    std::sort(freq_pairs.begin(), freq_pairs.end(), 
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) { return a.second < b.second; });
    
    // Clear the map
    m_frequency_map.clear();
    
    // Keep only the top m_final_table_size entries
    for (size_t i = freq_pairs.size() - m_final_table_size; i < freq_pairs.size(); ++i) {
        m_frequency_map[freq_pairs[i].first] = freq_pairs[i].second;
    }
}

// Get size
size_t frequency_value_table::size() const {
    return m_frequency_map.size();
}

// Get checking size
size_t frequency_value_table::checking_size() const {
    return m_checking_table_size;
}

// Get final size
size_t frequency_value_table::final_size() const {
    return m_final_table_size;
}

// Get symbol length
size_t frequency_value_table::symbol_length() const {
    return m_symbol_length;
}

// --------- Interface for Huffman code generation ---------

// Get a sorted vector of symbol-frequency pairs
std::vector<std::pair<uint64_t, uint64_t>> frequency_value_table::get_sorted_frequencies() const {
    // Create vector of pairs (symbol, frequency)
    std::vector<std::pair<uint64_t, uint64_t>> freq_pairs;
    freq_pairs.reserve(m_frequency_map.size());
    
    for (const auto& entry : m_frequency_map) {
        freq_pairs.push_back(entry);
    }
    
    // Sort by frequency (ascending)
    std::sort(freq_pairs.begin(), freq_pairs.end(), 
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) { return a.second < b.second; });
    
    return freq_pairs;
}

// Get the top most frequent entries, limited to final_table_size
std::vector<std::pair<uint64_t, uint64_t>> frequency_value_table::get_top_frequencies() const {
    // Create vector of pairs (symbol, frequency)
    std::vector<std::pair<uint64_t, uint64_t>> freq_pairs;
    freq_pairs.reserve(m_frequency_map.size());
    
    for (const auto& entry : m_frequency_map) {
        freq_pairs.push_back(entry);
    }
    
    // Sort by frequency (descending - highest frequency first)
    std::sort(freq_pairs.begin(), freq_pairs.end(), 
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) { return a.second > b.second; });
    
    // If we have more entries than final_table_size, trim the vector
    if (freq_pairs.size() > m_final_table_size) {
        freq_pairs.resize(m_final_table_size);
    }
    
    return freq_pairs;
}

// Get an unsorted vector of symbol-frequency pairs
std::vector<std::pair<uint64_t, uint64_t>> frequency_value_table::get_frequency_pairs() const {
    std::vector<std::pair<uint64_t, uint64_t>> freq_pairs;
    freq_pairs.reserve(m_frequency_map.size());
    
    for (const auto& entry : m_frequency_map) {
        freq_pairs.push_back(entry);
    }
    
    return freq_pairs;
}

// Get a reference to the internal frequency map
const std::unordered_map<uint64_t, uint64_t>& frequency_value_table::get_frequency_map() const {
    return m_frequency_map;
}

// Clear all frequencies in the table
void frequency_value_table::clear() {
    m_frequency_map.clear();
}

// Check if a specific symbol exists in the table
bool frequency_value_table::contains(uint64_t symbol) const {
    return m_frequency_map.find(symbol) != m_frequency_map.end();
}

// Get all symbols in the table
std::vector<uint64_t> frequency_value_table::get_symbols() const {
    std::vector<uint64_t> symbols;
    symbols.reserve(m_frequency_map.size());
    
    for (const auto& entry : m_frequency_map) {
        symbols.push_back(entry.first);
    }
    
    return symbols;
}

// Get iterator to the beginning of the frequency map
std::unordered_map<uint64_t, uint64_t>::const_iterator frequency_value_table::begin() const {
    return m_frequency_map.begin();
}

// Get iterator to the end of the frequency map
std::unordered_map<uint64_t, uint64_t>::const_iterator frequency_value_table::end() const {
    return m_frequency_map.end();
}

// ------------- Huffman Codebook Implementation -------------

// Constructor
huffman_codebook::huffman_codebook(size_t checking_table_size, size_t final_table_size, size_t symbol_length, size_t mag)
    : m_max_code_length(0), 
      m_mag(mag),
      freq_table(checking_table_size, final_table_size, symbol_length),
      m_compressor(*this) {
}

// Get MAG value
size_t huffman_codebook::get_mag() const {
    return m_mag;
}

// Set MAG value
void huffman_codebook::set_mag(size_t mag) {
    m_mag = mag;
}

// Compress a data block using the codebook
compress_outcome huffman_codebook::compress_block(const unsigned char* data, size_t size) const {
    return m_compressor.compress_data_block(data, size);
}

// Extract a symbol from a bitstream - compressor implementation
uint64_t huffman_codebook::compressor::extract_symbol_from_bitstream(
    const unsigned char* data, size_t size, size_t& bit_pos, size_t symbol_length) const {
    
    uint64_t symbol = 0;
    
    for (size_t j = 0; j < symbol_length; ++j) {
        // Calculate which byte and which bit within that byte
        size_t byte_pos = bit_pos / 8;
        size_t bit_offset = 7 - (bit_pos % 8);  // MSB first (bit 7 is the highest bit in a byte)
        
        // Make sure we don't go beyond the data boundary
        if (byte_pos >= size) {
            bit_pos += (symbol_length - j);  // Skip remaining bits
            break;
        }
        
        // Extract the bit and add it to our symbol
        if (data[byte_pos] & (1 << bit_offset)) {
            symbol |= (1ULL << (symbol_length - 1 - j));
        }
        
        bit_pos++;
    }
    
    return symbol;
}

// Extract symbols from a data block - compressor implementation
std::vector<uint64_t> huffman_codebook::compressor::extract_symbols(
    const unsigned char* data, size_t size) const {
    
    if (!data) {
        std::cerr << "Error: Null data pointer provided to compressor" << std::endl;
        return {};
    }
    
    // Get the symbol length from the codebook's frequency table
    size_t symbol_length = m_codebook.freq_table.symbol_length();
    
    // Calculate total bits available in the data
    size_t total_bits = size * 8;
    
    // Calculate how many complete symbols we can extract
    size_t num_symbols = total_bits / symbol_length;
    if (total_bits % symbol_length != 0) {
        std::cerr << "Warning: Total bits not a multiple of symbol length" << std::endl;
    }
    
    // Track position in the bitstream and extract symbols
    size_t bit_pos = 0;
    std::vector<uint64_t> symbols;
    symbols.reserve(num_symbols);
    
    // Process each symbol
    for (size_t i = 0; i < num_symbols; ++i) {
        // Extract a symbol from the bitstream
        uint64_t symbol = extract_symbol_from_bitstream(data, size, bit_pos, symbol_length);
        symbols.push_back(symbol);
    }
    
    return symbols;
}

// Compress a data block using the Huffman codebook - compressor implementation
compress_outcome huffman_codebook::compressor::compress_data_block(
    const unsigned char* data, size_t size) const {
    
    compress_outcome result;
    
    // If we don't have any codes or data is null, return no compression
    if (m_codebook.size() == 0 || !data) {
        result.raw_compressed_size_bits = size * 8;
        result.raw_compression_ratio = 1.0;
        result.mag_compressed_size_bytes = size;
        result.burst_size = (size + m_codebook.get_mag() - 1) / m_codebook.get_mag(); // Ceiling division
        result.effective_compression_ratio = 1.0;
        return result;
    }
    
    // Extract symbols from the data block
    std::vector<uint64_t> symbols = extract_symbols(data, size);
    
    // Original size in bits
    size_t original_size_bits = size * 8;
    
    // Compressed size in bits (starts with 0)
    size_t compressed_size_bits = 0;
    
    // Count how many symbols we couldn't encode (no code in the codebook)
    size_t uncoded_symbols = 0;
    
    // Encode each symbol and calculate the total size
    for (uint64_t symbol : symbols) {
        if (m_codebook.has_code(symbol)) {
            // Add the code length to our compressed size
            compressed_size_bits += m_codebook.get_code_length(symbol);
        } else {
            // If we don't have a code for this symbol, we'll need the original bits
            uncoded_symbols++;
            compressed_size_bits += m_codebook.freq_table.symbol_length();
        }
    }
    

    
    // Add 1 bit per symbol to indicate if it's coded or literal
    compressed_size_bits += symbols.size();

    
    // Set the raw compressed size in bits
    result.raw_compressed_size_bits = compressed_size_bits;
    
    // Calculate the raw compression ratio
    // Prevent division by zero
    if (compressed_size_bits > 0) {
        result.raw_compression_ratio = static_cast<double>(original_size_bits) / compressed_size_bits;
    } else {
        result.raw_compression_ratio = 1.0; // Default to no compression if something went wrong
    }
    
    // Calculate the MAG compressed size in bytes (rounded up to next MAG boundary)
    // First convert bits to bytes (rounding up)
    size_t compressed_size_bytes = (compressed_size_bits + 7) / 8; // Ceiling division for bits to bytes
    
    // Then round up to the next MAG boundary
    result.mag_compressed_size_bytes = 
        ((compressed_size_bytes + m_codebook.get_mag() - 1) / m_codebook.get_mag()) * m_codebook.get_mag();
    
    // Calculate the burst size (how many MAG units)
    result.burst_size = result.mag_compressed_size_bytes / m_codebook.get_mag();
    
    

    // Calculate the effective compression ratio
    if (result.mag_compressed_size_bytes > 0) {
        result.effective_compression_ratio = 
            static_cast<double>(size) / result.mag_compressed_size_bytes;
    } else {
        result.effective_compression_ratio = 1.0;
    }

    if (result.burst_size > 3) {
        //store uncompressed size
        result.burst_size = 4;
        result.mag_compressed_size_bytes = 4 * m_codebook.get_mag();
        result.effective_compression_ratio = 1.0;
        result.raw_compression_ratio = 1.0;
        result.raw_compressed_size_bits = original_size_bits;
    }
    
    return result;
}

// Build Huffman tree from frequency pairs
std::shared_ptr<huffman_codebook::huffman_node> huffman_codebook::build_tree(
    const std::vector<std::pair<uint64_t, uint64_t>>& freq_pairs) {
    
    // Define a custom comparator for the priority queue (min heap)
    auto comparator = [](const std::shared_ptr<huffman_node>& a, const std::shared_ptr<huffman_node>& b) {
        return *a > *b;
    };
    
    // Create a priority queue (min heap) of nodes
    std::priority_queue<
        std::shared_ptr<huffman_node>,
        std::vector<std::shared_ptr<huffman_node>>,
        decltype(comparator)
    > pq(comparator);
    
    // Create leaf nodes for each symbol and add to the priority queue
    for (const auto& pair : freq_pairs) {
        pq.push(std::make_shared<huffman_node>(pair.first, pair.second));
    }
    
    // Handle edge case: if only one symbol, create a special tree
    if (pq.size() == 1) {
        std::shared_ptr<huffman_node> leaf = pq.top();
        std::shared_ptr<huffman_node> root = std::make_shared<huffman_node>(0, 0);
        root->is_leaf = false;
        root->frequency = leaf->frequency;
        root->left = leaf;  // Assign single code 0
        root->right = nullptr;
        return root;
    }
    
    // Build the Huffman tree
    while (pq.size() > 1) {
        // Get the two nodes with lowest frequencies
        std::shared_ptr<huffman_node> left = pq.top();
        pq.pop();
        std::shared_ptr<huffman_node> right = pq.top();
        pq.pop();
        
        // Create a new internal node with these two as children
        std::shared_ptr<huffman_node> parent = std::make_shared<huffman_node>(left, right);
        
        // Add the new node back to the queue
        pq.push(parent);
    }
    
    // The last remaining node is the root of the Huffman tree
    return pq.top();
}

// Generate codes by traversing the Huffman tree
void huffman_codebook::generate_codes_from_tree(
    std::shared_ptr<huffman_node> node, std::vector<bool>& code) {
    
    if (!node) return;
    
    if (node->is_leaf) {
        // Leaf node: store the code for this symbol
        m_codes[node->symbol] = code_entry(code);
        m_max_code_length = std::max(m_max_code_length, code.size());
    } else {
        // Internal node: traverse left (add 0) and right (add 1)
        if (node->left) {
            code.push_back(false);  // 0 for left
            generate_codes_from_tree(node->left, code);
            code.pop_back();
        }
        
        if (node->right) {
            code.push_back(true);   // 1 for right
            generate_codes_from_tree(node->right, code);
            code.pop_back();
        }
    }
}

// Convert to canonical Huffman codes
void huffman_codebook::make_canonical_codes() {
    if (m_codes.empty()) return;
    
    // Create a vector of (symbol, code_length) pairs
    std::vector<std::pair<uint64_t, size_t>> code_lengths;
    for (const auto& entry : m_codes) {
        code_lengths.push_back(std::make_pair(entry.first, entry.second.length));
    }
    
    // Sort by code length, then by symbol value for deterministic output
    std::sort(code_lengths.begin(), code_lengths.end(), 
              [](const std::pair<uint64_t, size_t>& a, const std::pair<uint64_t, size_t>& b) {
                  return a.second < b.second || (a.second == b.second && a.first < b.first);
              });
    
    // Generate canonical codes
    std::unordered_map<uint64_t, code_entry> canonical_codes;
    size_t code = 0;
    size_t prev_len = code_lengths[0].second;
    
    for (const auto& pair : code_lengths) {
        uint64_t symbol = pair.first;
        size_t len = pair.second;
        
        // If moving to a longer code length, shift left
        code <<= (len - prev_len);
        prev_len = len;
        
        // Create the code bits
        std::vector<bool> bits;
        bits.reserve(len);
        
        // Convert integer code to bit vector (MSB first)
        for (size_t i = 0; i < len; ++i) {
            bits.push_back((code & (1ULL << (len - 1 - i))) != 0);
        }
        
        // Store the canonical code
        canonical_codes[symbol] = code_entry(bits);
        
        // Increment code for next symbol
        code++;
    }
    
    // Replace the original codes with canonical codes
    m_codes = std::move(canonical_codes);
}

// Generate Huffman codes
size_t huffman_codebook::generate_huffman_codes() {
    // Clear existing codes
    m_codes.clear();
    m_max_code_length = 0;
    
    // Get the top frequencies
    auto freq_pairs = freq_table.get_top_frequencies();
    
    // If no frequencies, return
    if (freq_pairs.empty()) {
        return 0;
    }
    
    // For Huffman coding, we need frequencies in ascending order
    std::sort(freq_pairs.begin(), freq_pairs.end(),
              [](const std::pair<uint64_t, uint64_t>& a, const std::pair<uint64_t, uint64_t>& b) { return a.second < b.second; });
    
    // Build the Huffman tree
    auto root = build_tree(freq_pairs);
    
    // Generate codes by traversing the tree
    std::vector<bool> code;
    generate_codes_from_tree(root, code);
    
    // Convert to canonical Huffman codes for better compression
    make_canonical_codes();
    
    return m_codes.size();
}

// Generate Shannon-Fano codes
size_t huffman_codebook::generate_shannon_fano_codes() {
    // Clear existing codes
    m_codes.clear();
    m_max_code_length = 0;
    
    // Get the top frequencies
    auto freq_pairs = freq_table.get_top_frequencies();
    
    // If no frequencies, return
    if (freq_pairs.empty()) {
        return 0;
    }
    
    // For Shannon-Fano coding, we need frequencies in descending order (highest first)
    // (they're already sorted by get_top_frequencies())
    
    // Start the recursive Shannon-Fano encoding
    std::vector<bool> code;
    shannon_fano_encode(freq_pairs, 0, freq_pairs.size() - 1, code);
    
    // Convert to canonical codes for better compression
    make_canonical_codes();
    
    return m_codes.size();
}

// Generate Shannon codes using information theory approach
size_t huffman_codebook::generate_shannon_codes() {
    // Clear existing codes
    m_codes.clear();
    m_max_code_length = 0;
    
    // Get the top frequencies
    auto freq_pairs = freq_table.get_top_frequencies();
    
    // If no frequencies, return
    if (freq_pairs.empty()) {
        return 0;
    }
    
    // Calculate total frequency to find probabilities
    uint64_t total_frequency = 0;
    for (const auto& pair : freq_pairs) {
        total_frequency += pair.second;
    }
    
    // Create a vector to store symbol and its assigned code length
    std::vector<std::pair<uint64_t, size_t>> code_lengths;
    
    // Calculate code length for each symbol based on Shannon's formula: ceil(-log2(p))
    for (const auto& pair : freq_pairs) {
        uint64_t symbol = pair.first;
        uint64_t frequency = pair.second;
        
        // Calculate probability
        double probability = static_cast<double>(frequency) / total_frequency;
        
        // Calculate optimal code length: -log2(probability), rounded up
        size_t code_length = static_cast<size_t>(std::ceil(-std::log2(probability)));
        
        // Ensure a minimum code length of 1 bit
        code_length = std::max(code_length, static_cast<size_t>(1));
        
        // Track the maximum code length
        m_max_code_length = std::max(m_max_code_length, code_length);
        
        // Store symbol and its code length
        code_lengths.push_back(std::make_pair(symbol, code_length));
    }
    
    // Sort by code length, then by symbol value (for deterministic output)
    std::sort(code_lengths.begin(), code_lengths.end(),
              [](const std::pair<uint64_t, size_t>& a, const std::pair<uint64_t, size_t>& b) {
                  return a.second < b.second || (a.second == b.second && a.first < b.first);
              });
    
    // Generate canonical codes (like Huffman canonical codes)
    std::unordered_map<uint64_t, code_entry> canonical_codes;
    size_t code = 0;
    size_t prev_len = code_lengths[0].second;
    
    for (const auto& pair : code_lengths) {
        uint64_t symbol = pair.first;
        size_t len = pair.second;
        
        // If moving to a longer code length, shift left
        code <<= (len - prev_len);
        prev_len = len;
        
        // Create the code bits
        std::vector<bool> bits;
        bits.reserve(len);
        
        // Convert integer code to bit vector (MSB first)
        for (size_t i = 0; i < len; ++i) {
            bits.push_back((code & (1ULL << (len - 1 - i))) != 0);
        }
        
        // Store the canonical code
        canonical_codes[symbol] = code_entry(bits);
        
        // Increment code for next symbol
        code++;
    }
    
    // Use the generated codes
    m_codes = std::move(canonical_codes);
    
    return m_codes.size();
}

// Shannon-Fano encoding helper function (recursive)
void huffman_codebook::shannon_fano_encode(
    std::vector<std::pair<uint64_t, uint64_t>>& symbols, 
    size_t start, size_t end, 
    std::vector<bool> code) {
    
    // Base case: only one symbol in the range
    if (start == end) {
        uint64_t symbol = symbols[start].first;
        m_codes[symbol] = code_entry(code);
        m_max_code_length = std::max(m_max_code_length, code.size());
        return;
    }
    
    // Base case: two symbols - assign 0 to first, 1 to second
    if (start + 1 == end) {
        // First symbol gets code + 0
        std::vector<bool> code_a = code;
        code_a.push_back(false);
        uint64_t symbol_a = symbols[start].first;
        m_codes[symbol_a] = code_entry(code_a);
        m_max_code_length = std::max(m_max_code_length, code_a.size());
        
        // Second symbol gets code + 1
        std::vector<bool> code_b = code;
        code_b.push_back(true);
        uint64_t symbol_b = symbols[end].first;
        m_codes[symbol_b] = code_entry(code_b);
        m_max_code_length = std::max(m_max_code_length, code_b.size());
        
        return;
    }
    
    // Compute total frequency in the range
    uint64_t total_freq = 0;
    for (size_t i = start; i <= end; ++i) {
        total_freq += symbols[i].second;
    }
    
    // Find division point that makes the two parts as close as possible in total frequency
    uint64_t curr_freq = 0;
    size_t split_point = start;
    
    for (size_t i = start; i <= end; ++i) {
        uint64_t next_freq = curr_freq + symbols[i].second;
        
        // If adding this symbol gets us closer to half the total, update the split point
        if (std::abs(static_cast<int64_t>(next_freq * 2 - total_freq)) <= 
            std::abs(static_cast<int64_t>(curr_freq * 2 - total_freq))) {
            curr_freq = next_freq;
            split_point = i;
        } else {
            // Once we pass the halfway mark, we've found our split
            break;
        }
    }
    
    // Ensure we didn't put everything in one group
    if (split_point == end) {
        split_point = start + (end - start) / 2;
    }
    
    // Recursive call for first group (code + 0)
    std::vector<bool> code_left = code;
    code_left.push_back(false);
    shannon_fano_encode(symbols, start, split_point, code_left);
    
    // Recursive call for second group (code + 1)
    std::vector<bool> code_right = code;
    code_right.push_back(true);
    shannon_fano_encode(symbols, split_point + 1, end, code_right);
}

// Check if a symbol has a code
bool huffman_codebook::has_code(uint64_t symbol) const {
    return m_codes.find(symbol) != m_codes.end();
}

// Get code bits for a symbol
const std::vector<bool>& huffman_codebook::get_code_bits(uint64_t symbol) const {
    static const std::vector<bool> empty_code;
    auto it = m_codes.find(symbol);
    if (it != m_codes.end()) {
        return it->second.bits;
    }
    return empty_code;
}

// Get code length for a symbol
size_t huffman_codebook::get_code_length(uint64_t symbol) const {
    auto it = m_codes.find(symbol);
    if (it != m_codes.end()) {
        return it->second.length;
    }
    return 0;
}

// Get code string for a symbol
std::string huffman_codebook::get_code_string(uint64_t symbol) const {
    auto it = m_codes.find(symbol);
    if (it != m_codes.end()) {
        std::string code_str;
        code_str.reserve(it->second.length);
        for (bool bit : it->second.bits) {
            code_str.push_back(bit ? '1' : '0');
        }
        return code_str;
    }
    return "";
}

// Get maximum code length
size_t huffman_codebook::get_max_code_length() const {
    return m_max_code_length;
}

// Get all symbols with codes
std::vector<uint64_t> huffman_codebook::get_coded_symbols() const {
    std::vector<uint64_t> symbols;
    symbols.reserve(m_codes.size());
    for (const auto& entry : m_codes) {
        symbols.push_back(entry.first);
    }
    return symbols;
}

// Get all codes as (symbol, code_string) pairs
std::vector<std::pair<uint64_t, std::string>> huffman_codebook::get_all_codes() const {
    std::vector<std::pair<uint64_t, std::string>> result;
    result.reserve(m_codes.size());
    
    for (const auto& entry : m_codes) {
        std::string code_str;
        code_str.reserve(entry.second.length);
        for (bool bit : entry.second.bits) {
            code_str.push_back(bit ? '1' : '0');
        }
        result.push_back(std::make_pair(entry.first, code_str));
    }
    
    return result;
}

// Clear codes
void huffman_codebook::clear_codes() {
    m_codes.clear();
    m_max_code_length = 0;
}

// Clear all
void huffman_codebook::clear_all() {
    clear_codes();
    freq_table.clear();
}

// Get size
size_t huffman_codebook::size() const {
    return m_codes.size();
}

