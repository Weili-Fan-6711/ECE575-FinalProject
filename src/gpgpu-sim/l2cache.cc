// Copyright (c) 2009-2011, Tor M. Aamodt
// The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution. Neither the name of
// The University of British Columbia nor the names of its contributors may be
// used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>
#include <set>
#include <sstream> 
#include <iomanip> 
#include <string>

#include "../abstract_hardware_model.h"
#include "../option_parser.h"
#include "../statwrapper.h"
#include "dram.h"
#include "gpu-cache.h"
#include "gpu-sim.h"
#include "histogram.h"
#include "l2cache.h"
#include "l2cache_trace.h"
#include "mem_fetch.h"
#include "mem_latency_stat.h"
#include "shader.h"
#include "huffman.h"


#include "../cuda-sim/memory.h"
#include "similaritycheck.h"


#define SIMILARITY_CACHE_SIZE 64 // 1KB per partition 
#define COMPRESSOR_LATENCY 46
#define DECOMPRESSOR_LATENCY 82
#define TOTAL_NUMBER_OF_COMPRESSOR 68
#define TOTAL_NUMBER_OF_DECOMPRESSOR 120
#define PWD 4 //parallel decoding way
#define PWD_DECOMPRESSOR_LATENCY DECOMPRESSOR_LATENCY/PWD
#define PWD_TOTAL_NUMBER_OF_DECOMPRESSOR TOTAL_NUMBER_OF_DECOMPRESSOR/PWD
#define HUFFMAN_SYMBOL_LENGTH 16 //NOTE: in bits
#define MAG 32 // Memory Access Granularity in bytes
#define MEMORY_REQUEST_SIZE 128 //in byte
#define SAMPLING_DURATION 30000 //cycle
#define HUFFMAN_TABLE_SIZE 1000

mem_fetch *partition_mf_allocator::alloc(new_addr_type addr,
                                         mem_access_type type, unsigned size,
                                         bool wr,
                                         unsigned long long cycle) const {
  assert(wr);
  mem_access_t access(type, addr, size, wr, m_memory_config->gpgpu_ctx);
  mem_fetch *mf = new mem_fetch(access, NULL, WRITE_PACKET_SIZE, -1, -1, -1,
                                m_memory_config, cycle);
  return mf;
}

//Sitao:interface for m_compression_storage
void memory_partition_unit::update_compression_storage(new_addr_type addr, compression_info info){
  m_compression_storage[addr] = info;
}

compression_info memory_partition_unit::get_compression_info(new_addr_type addr){
  auto it = m_compression_storage.find(addr);
  if (it != m_compression_storage.end()){
    return it->second;
  }
  else{
    compression_info default_info;
    default_info.is_compressed = false;
    return default_info;
  }
}

memory_partition_unit::memory_partition_unit(unsigned partition_id,
                                             const memory_config *config,
                                             class memory_stats_t *stats,
                                             class gpgpu_sim *gpu)
    : m_id(partition_id),
      m_config(config),
      m_stats(stats),
      m_arbitration_metadata(config),
      m_gpu(gpu) {
  m_dram = new dram_t(m_id, m_config, m_stats, this, gpu);

  m_sub_partition = new memory_sub_partition
      *[m_config->m_n_sub_partition_per_memory_channel];
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    unsigned sub_partition_id =
        m_id * m_config->m_n_sub_partition_per_memory_channel + p;
    m_sub_partition[p] =
        new memory_sub_partition(sub_partition_id, m_config, stats, gpu);
  }
  //Sitao
  m_similarity_cache = new similarity_cache(SIMILARITY_CACHE_SIZE);
  m_huffman_codebook = new huffman_codebook(HUFFMAN_TABLE_SIZE*1.2, HUFFMAN_TABLE_SIZE, HUFFMAN_SYMBOL_LENGTH, MAG);
  m_metadata_interface = new metadata_cache_interface(this);
  m_metadata_allocator = new partition_mf_allocator(m_config);
  m_metadata_cache = new l2_cache("metadata_cache", m_config->m_huffman_metadata_config, -1, -1, m_metadata_interface,
                                   m_metadata_allocator, IN_PARTITION_COMPRESSOR_MISS_QUEUE, m_gpu);

   m_huffman_to_dram_queue = new fifo_pipeline<mem_fetch>("compressor_to_dram_queue", 0, 1000);
   m_L2_to_metadata_translater_read_queue = new fifo_pipeline<mem_fetch>("L2_to_metadata_translater_read_queue", 0, 1000);
   m_L2_to_compressor_queue = new fifo_pipeline<mem_fetch>("L2_to_compressor_queue", 0, 1000);
   m_to_metadata_cache_queue = new fifo_pipeline<mem_fetch>("to_metadata_cache_queue", 0, 1000);
   m_from_metadata_cache_queue = new fifo_pipeline<mem_fetch>("from_metadata_cache_queue", 0, 1000);
   m_metadata_cache_to_dram_queue = new fifo_pipeline<mem_fetch>("metadata_cache_to_dram_queue", 0, 1000);
   m_dram_to_metadata_cache_queue = new fifo_pipeline<mem_fetch>("dram_to_metadata_cache_queue", 0, 1000);
   m_decompressor_to_L2_queue = new fifo_pipeline<mem_fetch>("decompressor_to_L2_queue", 0, 1000);
   
   
}

void memory_partition_unit::handle_memcpy_to_gpu(
    size_t addr, unsigned global_subpart_id, mem_access_sector_mask_t mask) {
  unsigned p = global_sub_partition_id_to_local_id(global_subpart_id);
  std::string mystring = mask.to_string<char, std::string::traits_type,
                                        std::string::allocator_type>();
  MEMPART_DPRINTF(
      "Copy Engine Request Received For Address=%zx, local_subpart=%u, "
      "global_subpart=%u, sector_mask=%s \n",
      addr, p, global_subpart_id, mystring.c_str());
  m_sub_partition[p]->force_l2_tag_update(
      addr, m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle, mask);
}

memory_partition_unit::~memory_partition_unit() {
  delete m_dram;
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    delete m_sub_partition[p];
  }
  delete[] m_sub_partition;
  //Sitao
  delete m_similarity_cache;
  delete m_huffman_to_dram_queue;
  delete m_L2_to_metadata_translater_read_queue;
  delete m_L2_to_compressor_queue;
  delete m_huffman_codebook;
  delete m_metadata_cache;
  delete m_to_metadata_cache_queue;
  delete m_from_metadata_cache_queue;
  delete m_metadata_cache_to_dram_queue;
  delete m_dram_to_metadata_cache_queue;
  delete m_metadata_allocator;
  delete m_metadata_interface;
  delete m_decompressor_to_L2_queue;
}

memory_partition_unit::arbitration_metadata::arbitration_metadata(
    const memory_config *config)
    : m_last_borrower(config->m_n_sub_partition_per_memory_channel - 1),
      m_private_credit(config->m_n_sub_partition_per_memory_channel, 0),
      m_shared_credit(0) {
  // each sub partition get at least 1 credit for forward progress
  // the rest is shared among with other partitions
  m_private_credit_limit = 1;
  m_shared_credit_limit = config->gpgpu_frfcfs_dram_sched_queue_size +
                          config->gpgpu_dram_return_queue_size -
                          (config->m_n_sub_partition_per_memory_channel - 1);
  if (config->seperate_write_queue_enabled)
    m_shared_credit_limit += config->gpgpu_frfcfs_dram_write_queue_size;
  if (config->gpgpu_frfcfs_dram_sched_queue_size == 0 or
      config->gpgpu_dram_return_queue_size == 0) {
    m_shared_credit_limit =
        0;  // no limit if either of the queue has no limit in size
  }
  assert(m_shared_credit_limit >= 0);
}

bool memory_partition_unit::arbitration_metadata::has_credits(
    int inner_sub_partition_id) const {
  int spid = inner_sub_partition_id;
  if (m_private_credit[spid] < m_private_credit_limit) {
    return true;
  } else if (m_shared_credit_limit == 0 ||
             m_shared_credit < m_shared_credit_limit) {
    return true;
  } else {
    return false;
  }
}

void memory_partition_unit::arbitration_metadata::borrow_credit(
    int inner_sub_partition_id) {
  int spid = inner_sub_partition_id;
  if (m_private_credit[spid] < m_private_credit_limit) {
    m_private_credit[spid] += 1;
  } else if (m_shared_credit_limit == 0 ||
             m_shared_credit < m_shared_credit_limit) {
    m_shared_credit += 1;
  } else {
    assert(0 && "DRAM arbitration error: Borrowing from depleted credit!");
  }
  m_last_borrower = spid;
}

void memory_partition_unit::arbitration_metadata::return_credit(
    int inner_sub_partition_id) {
  int spid = inner_sub_partition_id;
  if (m_private_credit[spid] > 0) {
    m_private_credit[spid] -= 1;
  } else {
    m_shared_credit -= 1;
  }
  assert((m_shared_credit >= 0) &&
         "DRAM arbitration error: Returning more than available credits!");
}

void memory_partition_unit::arbitration_metadata::print(FILE *fp) const {
  fprintf(fp, "private_credit = ");
  for (unsigned p = 0; p < m_private_credit.size(); p++) {
    fprintf(fp, "%d ", m_private_credit[p]);
  }
  fprintf(fp, "(limit = %d)\n", m_private_credit_limit);
  fprintf(fp, "shared_credit = %d (limit = %d)\n", m_shared_credit,
          m_shared_credit_limit);
}

bool memory_partition_unit::busy() const {
  bool busy = false;
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    if (m_sub_partition[p]->busy()) {
      busy = true;
    }
  }
  return busy;
}

void memory_partition_unit::cache_cycle(unsigned cycle) {
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    m_sub_partition[p]->cache_cycle(cycle);
  }
}

void memory_partition_unit::visualizer_print(gzFile visualizer_file) const {
  m_dram->visualizer_print(visualizer_file);
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    m_sub_partition[p]->visualizer_print(visualizer_file);
  }
}

// determine whether a given subpartition can issue to DRAM
bool memory_partition_unit::can_issue_to_dram(int inner_sub_partition_id) {
  int spid = inner_sub_partition_id;
  bool sub_partition_contention = m_sub_partition[spid]->dram_L2_queue_full();
  bool has_dram_resource = m_arbitration_metadata.has_credits(spid);

  MEMPART_DPRINTF(
      "sub partition %d sub_partition_contention=%c has_dram_resource=%c\n",
      spid, (sub_partition_contention) ? 'T' : 'F',
      (has_dram_resource) ? 'T' : 'F');

  return (has_dram_resource && !sub_partition_contention);
}

int memory_partition_unit::global_sub_partition_id_to_local_id(
    int global_sub_partition_id) const {
  return (global_sub_partition_id -
          m_id * m_config->m_n_sub_partition_per_memory_channel);
}

void memory_partition_unit::simple_dram_model_cycle() {
  // pop completed memory request from dram and push it to dram-to-L2 queue
  // of the original sub partition
  if (!m_dram_latency_queue.empty() &&
      ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle) >=
       m_dram_latency_queue.front().ready_cycle)) {
    mem_fetch *mf_return = m_dram_latency_queue.front().req;
    if (mf_return->get_access_type() != L1_WRBK_ACC &&
        mf_return->get_access_type() != L2_WRBK_ACC) {
      mf_return->set_reply();

      unsigned dest_global_spid = mf_return->get_sub_partition_id();
      int dest_spid = global_sub_partition_id_to_local_id(dest_global_spid);
      assert(m_sub_partition[dest_spid]->get_id() == dest_global_spid);
      if (!m_sub_partition[dest_spid]->dram_L2_queue_full()) {
        if (mf_return->get_access_type() == L1_WRBK_ACC) {
          m_sub_partition[dest_spid]->set_done(mf_return);
          delete mf_return;
        } else {
          m_sub_partition[dest_spid]->dram_L2_queue_push(mf_return);
          mf_return->set_status(
              IN_PARTITION_DRAM_TO_L2_QUEUE,
              m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          m_arbitration_metadata.return_credit(dest_spid);
          MEMPART_DPRINTF(
              "mem_fetch request %p return from dram to sub partition %d\n",
              mf_return, dest_spid);
        }
        m_dram_latency_queue.pop_front();
      }

    } else {
      this->set_done(mf_return);
      delete mf_return;
      m_dram_latency_queue.pop_front();
    }
  }

  // mem_fetch *mf = m_sub_partition[spid]->L2_dram_queue_top();
  // if( !m_dram->full(mf->is_write()) ) {
  // L2->DRAM queue to DRAM latency queue
  // Arbitrate among multiple L2 subpartitions
  int last_issued_partition = m_arbitration_metadata.last_borrower();
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    int spid = (p + last_issued_partition + 1) %
               m_config->m_n_sub_partition_per_memory_channel;
    if (!m_sub_partition[spid]->L2_dram_queue_empty() &&
        can_issue_to_dram(spid)) {
      mem_fetch *mf = m_sub_partition[spid]->L2_dram_queue_top();
      if (m_dram->full(mf->is_write())) break;

      m_sub_partition[spid]->L2_dram_queue_pop();
      MEMPART_DPRINTF(
          "Issue mem_fetch request %p from sub partition %d to dram\n", mf,
          spid);
      dram_delay_t d;
      d.req = mf;
      d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                      m_config->dram_latency;
      m_dram_latency_queue.push_back(d);
      mf->set_status(IN_PARTITION_DRAM_LATENCY_QUEUE,
                     m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      m_arbitration_metadata.borrow_credit(spid);
      break;  // the DRAM should only accept one request per cycle
    }
  }
  //}
}

void memory_partition_unit::dram_cycle() {
  // pop completed memory request from dram and push it to dram-to-L2 queue
  // of the original sub partition
  
  mem_fetch *mf_return = m_dram->return_queue_top();

  if (mf_return){
    //Sitao: Huffman code logic
    if (mf_return -> is_meta_data_request){
      //return to metadata cache
      m_dram_to_metadata_cache_queue->push(mf_return);
      m_dram->return_queue_pop();
    }
    else {
      if ((mf_return->get_access_type() == GLOBAL_ACC_R || mf_return->get_access_type() == L2_WR_ALLOC_R)&&m_huffman_enabled==true){
      //decompressor logic
        if (m_decompressor_latency_queue.size() <= ((PWD_TOTAL_NUMBER_OF_DECOMPRESSOR + m_config->m_n_mem - 1) / m_config->m_n_mem)){
          //reuse dram_delay_t
          dram_delay_t d;
          d.req = mf_return;
          d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                          PWD_DECOMPRESSOR_LATENCY;
          m_decompressor_latency_queue.push_back(d);
          m_dram->return_queue_pop();}
      }
      else{
        //push to decompressor to L2 queue
        if (!m_decompressor_to_L2_queue->full()){
        m_decompressor_to_L2_queue->push(mf_return);
        m_dram->return_queue_pop();
        }
      }    
    }
  }
  else{
    m_dram->return_queue_pop();
  }

    //complete from latency queue
    if (!m_decompressor_latency_queue.empty() &&
        ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle) >=
        m_decompressor_latency_queue.front().ready_cycle)){
      mem_fetch *mf_return = m_decompressor_latency_queue.front().req;
      m_decompressor_latency_queue.pop_front();
      if (mf_return->m_compressed){
      mf_return->set_data_size(mf_return->m_original_size);}
      m_decompressor_to_L2_queue->push(mf_return);
    }
    
    //going from decompressor, insert into subpartition
    if (!m_decompressor_to_L2_queue->empty()){
      mem_fetch *mf_return = m_decompressor_to_L2_queue->top();
      if (mf_return) {
      unsigned dest_global_spid = mf_return->get_sub_partition_id();
      int dest_spid = global_sub_partition_id_to_local_id(dest_global_spid);
      assert(m_sub_partition[dest_spid]->get_id() == dest_global_spid);
      if (!m_sub_partition[dest_spid]->dram_L2_queue_full()) {
        if (mf_return->get_access_type() == L1_WRBK_ACC) {
          m_sub_partition[dest_spid]->set_done(mf_return);
          delete mf_return;
        } else {
          m_sub_partition[dest_spid]->dram_L2_queue_push(mf_return);
          mf_return->set_status(IN_PARTITION_DRAM_TO_L2_QUEUE,
                                m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          m_arbitration_metadata.return_credit(dest_spid);
          MEMPART_DPRINTF(
              "mem_fetch request %p return from dram to sub partition %d\n",
              mf_return, dest_spid);
        }
        //m_dram->return_queue_pop();
        m_decompressor_to_L2_queue->pop();
      }
    } 
    else {
      //m_dram->return_queue_pop();
      m_decompressor_to_L2_queue->pop();
    }
    }
  
 /*//////////////original code////////////////
  // pop completed memory request from dram and push it to dram-to-L2 queue
  // of the original sub partition
  mem_fetch *mf_return = m_dram->return_queue_top();
  if (mf_return) {
    unsigned dest_global_spid = mf_return->get_sub_partition_id();
    int dest_spid = global_sub_partition_id_to_local_id(dest_global_spid);
    assert(m_sub_partition[dest_spid]->get_id() == dest_global_spid);
    if (!m_sub_partition[dest_spid]->dram_L2_queue_full()) {
      if (mf_return->get_access_type() == L1_WRBK_ACC) {
        m_sub_partition[dest_spid]->set_done(mf_return);
        delete mf_return;
      } else {
        m_sub_partition[dest_spid]->dram_L2_queue_push(mf_return);
        mf_return->set_status(IN_PARTITION_DRAM_TO_L2_QUEUE,
                              m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        m_arbitration_metadata.return_credit(dest_spid);
        MEMPART_DPRINTF(
            "mem_fetch request %p return from dram to sub partition %d\n",
            mf_return, dest_spid);
      }
      m_dram->return_queue_pop();
    }
  } else {
    m_dram->return_queue_pop();
  }
  //////////////original code////////////////*/

  m_dram->cycle();
  m_dram->dram_log(SAMPLELOG);

  /*
  int last_issued_partition = m_arbitration_metadata.last_borrower();
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    int spid = (p + last_issued_partition + 1) %
               m_config->m_n_sub_partition_per_memory_channel;
    if (!m_sub_partition[spid]->L2_dram_queue_empty() &&
        can_issue_to_dram(spid)) {
      mem_fetch *mf = m_sub_partition[spid]->L2_dram_queue_top();
      if (m_dram->full(mf->is_write())) break;

      m_sub_partition[spid]->L2_dram_queue_pop();
      MEMPART_DPRINTF(
          "Issue mem_fetch request %p from sub partition %d to dram\n", mf,
          spid);
      dram_delay_t d;
      d.req = mf;
      d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                      m_config->dram_latency;
      m_dram_latency_queue.push_back(d);
      mf->set_status(IN_PARTITION_DRAM_LATENCY_QUEUE,
                     m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      m_arbitration_metadata.borrow_credit(spid);
      break;  // the DRAM should only accept one request per cycle
    }
  }
  //}

  // DRAM latency queue
  if (!m_dram_latency_queue.empty() &&
      ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle) >=
       m_dram_latency_queue.front().ready_cycle) &&
      !m_dram->full(m_dram_latency_queue.front().req->is_write())) {
    mem_fetch *mf = m_dram_latency_queue.front().req;
    m_dram_latency_queue.pop_front();
    m_dram->push(mf);
  }
  */

  
  // mem_fetch *mf = m_sub_partition[spid]->L2_dram_queue_top();
  // if( !m_dram->full(mf->is_write()) ) {
  // L2->DRAM queue to DRAM latency queue
  // Arbitrate among multiple L2 subpartitions
  int last_issued_partition = m_arbitration_metadata.last_borrower();
  mem_fetch *mf_to_be_classified = nullptr;
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    int spid = (p + last_issued_partition + 1) %
               m_config->m_n_sub_partition_per_memory_channel;
    if (!m_sub_partition[spid]->L2_dram_queue_empty() &&
        can_issue_to_dram(spid)) {
      mf_to_be_classified = m_sub_partition[spid]->L2_dram_queue_top();
      if (m_dram->full(mf_to_be_classified->is_write())){
        mf_to_be_classified = nullptr;
        break;}  //check whether the DRAM is full for request
      MEMPART_DPRINTF(
      "Issue mem_fetch request %p from sub partition %d to dram\n", mf_to_be_classified,
      spid);
      m_sub_partition[spid]->L2_dram_queue_pop();
      m_arbitration_metadata.borrow_credit(spid);
      break;  // the DRAM should only accept one request per cycle
    }
  }
  //}

  
  //////////////////////////////////////////////////////////////////////////////////////////////
  //Sitao's modification: going into huffman compressor
  if (m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle >= SAMPLING_DURATION)//20 Million cycles
  { 
    if ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle)%5000 == 0 ){
      printf("\n compression phase: cycle %llu\n", m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);}
    if (m_huffman_enabled == false){
    //generate huffman codebook
   //m_huffman_codebook->generate_huffman_codes();
    //m_huffman_codebook->generate_shannon_fano_codes();
    m_huffman_codebook->generate_shannon_codes();
    m_huffman_enabled = true;
    printf("\n=========================huffman codebook generated=========================\n");
    }
      /////////////////////huffman code logic starts////////////////////////////////////////
      if (mf_to_be_classified!=nullptr){
      if (mf_to_be_classified->get_access_type() == GLOBAL_ACC_R | mf_to_be_classified->get_access_type() == L2_WR_ALLOC_R){

        //L2_to_metadata_translater_read queue push
        m_L2_to_metadata_translater_read_queue->push(mf_to_be_classified);

      }
      else if (mf_to_be_classified->get_access_type() == L2_WRBK_ACC){
        //L2_to_compressor queue push
        m_L2_to_compressor_queue->push(mf_to_be_classified);
      }
      else{
        //just enter huffman to DRAM (latency) queue
        m_huffman_to_dram_queue->push(mf_to_be_classified); 
      }}

      //handling read request
      //pop from L2_to_metadata_translater_read queue
      if (!m_L2_to_metadata_translater_read_queue->empty()){
        mem_fetch *mf = m_L2_to_metadata_translater_read_queue->top();
        m_L2_to_metadata_translater_read_queue->pop();
        mf->m_compressed = true;
        mf->m_original_size = mf->get_data_size();
        mf->m_original_addr = mf->get_addr();
        mf->m_metadata_addr = mf->m_original_addr >> 7;
        mf->set_addr(mf->m_metadata_addr);
        mf->is_meta_data_request = true;
        //push the altered request to to_meatadata_cache_queue
        mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
                       m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        m_to_metadata_cache_queue->push(mf);
        /*
        //access compression_storage to see if the data is compressed
        compression_info info = get_compression_info(m_config->m_L2_config.block_addr(mf->get_addr()));
        if (!info.is_compressed){
          //if not compressed, just enter huffman to DRAM (latency) queue
          m_huffman_to_dram_queue->push(mf);
        }
        else{
          //if compressed, altering the memory request to access the meta data (storing original address
          mf->m_compressed = true;
          mf->m_original_addr = mf->get_addr();
          mf->m_original_size = mf->get_data_size();
          // Calculate metadata address by shifting original address right by 7 bits (mapping)
          mf->m_metadata_addr = mf->m_original_addr >> 7;
          mf->set_addr(mf->m_metadata_addr);
          mf->is_meta_data_request = true;
          //push the altered request to to_meatadata_cache_queue
          m_to_metadata_cache_queue->push(mf);}*/
      }

      //pop from_metadata_cache_queue
      if (!m_from_metadata_cache_queue->empty()){
        mem_fetch *mf = m_from_metadata_cache_queue->top();
        assert(mf->is_meta_data_request);
        mf->is_meta_data_request = false;
        m_from_metadata_cache_queue->pop();
        if (mf->m_compressed && (mf->get_access_type() == GLOBAL_ACC_R 
        || mf->get_access_type() == L2_WR_ALLOC_R)){

          //alter the request to access the original data
          mf->set_addr(mf->m_original_addr);
          //access the compression_storage to get the compressed (burst) size
          //compression_info info = get_compression_info(m_config->m_L2_config.block_addr(mf->get_addr()));

          //Assuming assuming all read requests are compressed, so we call compressor
          //call compressor
          new_addr_type probe_pointer = m_config->m_L2_config.block_addr(mf->get_addr());   
          unsigned char *data = new unsigned char[MEMORY_REQUEST_SIZE];
          for (int i = 0; i<MEMORY_REQUEST_SIZE; i++){
                  m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
                  //char x = data[i];
                  //fprintf(stdout, " %u", x & 0x00FF);
            }
          compress_outcome outcome = m_huffman_codebook->m_compressor.compress_data_block(data, 128);
          delete[] data;
          data = nullptr;

          //update the compression_storage
          compression_info info;
          info.is_compressed = true;
          info.original_size = mf->get_data_size();
          info.rawcompressed_size = outcome.raw_compressed_size_bits;
          info.burst_size = outcome.burst_size;
          update_compression_storage(m_config->m_L2_config.block_addr(mf->get_addr()), info);
          //gathering statistics, weighted average
          m_compression_stats.total_raw_compression_ratio = 
          (m_compression_stats.total_raw_compression_ratio*m_compression_stats.total_compression_requests + outcome.raw_compression_ratio)/(m_compression_stats.total_compression_requests + 1);
          m_compression_stats.total_effective_compression_ratio = 
          (m_compression_stats.total_effective_compression_ratio*m_compression_stats.total_compression_requests + outcome.effective_compression_ratio)/(m_compression_stats.total_compression_requests + 1);
          m_compression_stats.total_compression_requests++;
            
          //assert(info != NULL);
          mf->set_data_size(std::min<unsigned int>(outcome.burst_size * MAG, static_cast<unsigned int>(MEMORY_REQUEST_SIZE)));
          //push the altered request to huffman to DRAM (latency) queue
          mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
            m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          m_huffman_to_dram_queue->push(mf);
          }}



      //handling write request
      //pop from L2_to_compressor queue
      if (!m_L2_to_compressor_queue->empty()){
        mem_fetch *mf = m_L2_to_compressor_queue->top();
        if ( m_compressor_latency_queue.size() <= ((TOTAL_NUMBER_OF_COMPRESSOR + m_config->m_n_mem - 1) / m_config->m_n_mem)){
      assert(mf->get_access_type() == L2_WRBK_ACC);
      //create a compressor delay packet, reuse dram_delay_t
      m_L2_to_compressor_queue->pop();
      dram_delay_t d;
      d.req = mf;
      d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                      COMPRESSOR_LATENCY;
      m_compressor_latency_queue.push_back(d);
      //push the delay packet to compressor latency queue
      }
      }

      //Get the request from compressor latency queue to compressor to DRAM queue
      if (!m_compressor_latency_queue.empty()&&
      ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle) >= m_compressor_latency_queue.front().ready_cycle)){
        mem_fetch *mf = m_compressor_latency_queue.front().req;
        m_compressor_latency_queue.pop_front();
        m_huffman_to_dram_queue->push(mf);

        //call compressor
        new_addr_type probe_pointer = m_config->m_L2_config.block_addr(mf->get_addr());   
        unsigned char *data = new unsigned char[MEMORY_REQUEST_SIZE];
        for (int i = 0; i<MEMORY_REQUEST_SIZE; i++){
                m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
                //char x = data[i];
                //fprintf(stdout, " %u", x & 0x00FF);
          }
        //m_huffman_codebook->freq_table.process_data_block(data,MEMORY_REQUEST_SIZE);
        compress_outcome outcome = m_huffman_codebook->m_compressor.compress_data_block(data, 128);
        delete[] data;
        data = nullptr;

        //update the compression_storage
        compression_info info;
        info.is_compressed = true;
        info.original_size = mf->get_data_size();
        info.rawcompressed_size = outcome.raw_compressed_size_bits;
        info.burst_size = outcome.burst_size;
        update_compression_storage(m_config->m_L2_config.block_addr(mf->get_addr()), info);
        //gathering statistics, weighted average
        m_compression_stats.total_raw_compression_ratio = 
        (m_compression_stats.total_raw_compression_ratio*m_compression_stats.total_compression_requests + outcome.raw_compression_ratio)/(m_compression_stats.total_compression_requests + 1);
        m_compression_stats.total_effective_compression_ratio = 
        (m_compression_stats.total_effective_compression_ratio*m_compression_stats.total_compression_requests + outcome.effective_compression_ratio)/(m_compression_stats.total_compression_requests + 1);
        m_compression_stats.total_compression_requests++;


        //create a write request to the metadata cache//ignore for now, #TODO
      }

      ////////////////////////metadata cache cycle/////////////////////////////////////////////////////
      //metadata fill response
      if (m_metadata_cache->access_ready() && !m_from_metadata_cache_queue->full()){
        mem_fetch *mf = m_metadata_cache->next_access();
        if ((mf->get_access_type() == L2_WR_ALLOC_R) || (mf->get_access_type() == GLOBAL_ACC_R)){
          m_from_metadata_cache_queue->push(mf);
          mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
            m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        }
        else{
          delete mf;
        }
      }
      //DRAM to metadata cache queue
      if (!m_dram_to_metadata_cache_queue->empty()){
        mem_fetch *mf = m_dram_to_metadata_cache_queue->top();
        if (m_metadata_cache->waiting_for_fill(mf)){
          m_metadata_cache->fill(mf,m_gpu->gpu_sim_cycle+m_gpu->gpu_tot_sim_cycle);
          mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
            m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          m_dram_to_metadata_cache_queue->pop();
        }
        else{
          m_dram_to_metadata_cache_queue->pop();
          mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
            m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        }
      }

      //prior meatadata misses inserted into m_huffman_to_dram_queue
      m_metadata_cache->cycle();

      //new metadata cache access
      if (!m_to_metadata_cache_queue->empty()){
        mem_fetch *mf = m_to_metadata_cache_queue->top();
        bool outputfull = m_from_metadata_cache_queue->full();
        bool portfree = m_metadata_cache->data_port_free();
        if (!outputfull && portfree){
          std::list<cache_event> events;
          enum cache_request_status status = m_metadata_cache->access(mf->get_addr(),mf,
                                   m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle,
                                   events);
          mf->set_status(IN_HUFFMAN_META_DATA_CACHE,m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          bool write_sent = was_write_sent(events);
          bool read_sent = was_read_sent(events);
          if (status == HIT){
            if(!write_sent){
              if (mf->get_access_type() == L2_WR_ALLOC_R || mf->get_access_type() == GLOBAL_ACC_R){
                mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
                  m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
                m_from_metadata_cache_queue->push(mf);
              }
              else{
                //#TODO: incomplete, have not support other read operations
                assert(!write_sent);
              }
            }
            else{
              //#TODO: incomplete, have not support write operations to metadata cache
              assert(write_sent);
            }
            m_to_metadata_cache_queue->pop();
          }
          else if (status != RESERVATION_FAIL){
            //#TODO: incomplete, have not support write operations
            assert(status == MISS);
            m_to_metadata_cache_queue->pop();
          }
          else{
            //metadata cache lock up
            assert(status == RESERVATION_FAIL);
          }

      
        }
      }
      
  


      //from huffman to DRAM latency queue
      if(!m_huffman_to_dram_queue->empty()){
        mem_fetch *mf = m_huffman_to_dram_queue->top();
        m_huffman_to_dram_queue->pop();
        dram_delay_t d;
        d.req = mf;
        d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                        m_config->dram_latency;
        m_dram_latency_queue.push_back(d);
        mf->set_status(IN_PARTITION_DRAM_LATENCY_QUEUE,
                        m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);}
  


  }

  /////////////////////huffman code logic ends/////////////////////////////////////////////////////

  ////////////////////////sampling phase starts/////////////////////////////////////////////////////
  else{ 
    if ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle)%5000 == 0 ){
    printf("\n sampling phase: cycle %llu\n", m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);}
    //else: sampling phase
    //check if the request is a read request
    if (mf_to_be_classified!=nullptr){
      
    if (mf_to_be_classified->get_access_type() == GLOBAL_ACC_R || mf_to_be_classified->get_access_type() == L2_WR_ALLOC_R){
      new_addr_type probe_pointer = m_config->m_L2_config.block_addr(mf_to_be_classified->get_addr());   
      unsigned char *data = new unsigned char[MEMORY_REQUEST_SIZE];
      for (int i = 0; i<MEMORY_REQUEST_SIZE; i++){
              m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
              //char x = data[i];
          //fprintf(stdout, " %u", x & 0x00FF);
        }
        m_huffman_codebook->freq_table.process_data_block(data,MEMORY_REQUEST_SIZE);
        delete[] data;
        data = nullptr;
    }
    
    dram_delay_t d;
    d.req = mf_to_be_classified;
    d.ready_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                    m_config->dram_latency;
    m_dram_latency_queue.push_back(d);
    mf_to_be_classified->set_status(IN_PARTITION_DRAM_LATENCY_QUEUE,
                    m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);}
    }


    ////////////////////////sampling phase ends/////////////////////////////////////////////////////


  //mf_to_be_classified = nullptr;





  // DRAM latency queue
  if (!m_dram_latency_queue.empty() &&
      ((m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle) >=
       m_dram_latency_queue.front().ready_cycle) &&
      !m_dram->full(m_dram_latency_queue.front().req->is_write())) {
    mem_fetch *mf = m_dram_latency_queue.front().req;
    m_dram_latency_queue.pop_front();
    //Sitao: if it is metadata cache request, directly return without going to DRAM (model as fixed latency due to problem of metadata addr mapping)
    //we do not deal with write request for metadata cache as we only allows read request at the moment
    if (mf->is_meta_data_request){
      mf->set_status(IN_HUFFMAN_META_DATA_CACHE,
        m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      m_dram_to_metadata_cache_queue->push(mf);
    }
    else{
      m_dram->push(mf);
    }
  }
}

//Sitao
void memory_partition_unit::similarity_cache_cycle(){
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    if (!m_sub_partition[p]->L2_similarity_queue_empty()){
      mem_fetch *mf = m_sub_partition[p]->L2_similarity_queue_top();
      new_addr_type probe_pointer = mf->get_addr();
      unsigned char *data = new unsigned char[32];
      for (int i = 0; i<32; i++){
              m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
               //char x = data[i];
              //fprintf(stdout, " %u", x & 0x00FF);
            }
      std::string data_string = memory_to_string(data,32);
      m_similarity_cache->check_and_update(data_string);
      delete[] data; // Release the memory for data
      data = nullptr;
      m_sub_partition[p]->L2_similarity_queue_pop();
      //undeveloped logic: makeing sure only one request is check per cycle, need to add in 
      //round robin support
      //break;
    }
  }

}

void memory_partition_unit::set_done(mem_fetch *mf) {
  unsigned global_spid = mf->get_sub_partition_id();
  int spid = global_sub_partition_id_to_local_id(global_spid);
  assert(m_sub_partition[spid]->get_id() == global_spid);
  if (mf->get_access_type() == L1_WRBK_ACC ||
      mf->get_access_type() == L2_WRBK_ACC) {
    m_arbitration_metadata.return_credit(spid);
    MEMPART_DPRINTF(
        "mem_fetch request %p return from dram to sub partition %d\n", mf,
        spid);
  }
  m_sub_partition[spid]->set_done(mf);
}

void memory_partition_unit::set_dram_power_stats(
    unsigned &n_cmd, unsigned &n_activity, unsigned &n_nop, unsigned &n_act,
    unsigned &n_pre, unsigned &n_rd, unsigned &n_wr, unsigned &n_req) const {
  m_dram->set_dram_power_stats(n_cmd, n_activity, n_nop, n_act, n_pre, n_rd,
                               n_wr, n_req);
}

void memory_partition_unit::print(FILE *fp) const {
  fprintf(fp, "Memory Partition %u: \n", m_id);
  for (unsigned p = 0; p < m_config->m_n_sub_partition_per_memory_channel;
       p++) {
    m_sub_partition[p]->print(fp);
  }
  fprintf(fp, "In Dram Latency Queue (total = %zd): \n",
          m_dram_latency_queue.size());
  for (std::list<dram_delay_t>::const_iterator mf_dlq =
           m_dram_latency_queue.begin();
       mf_dlq != m_dram_latency_queue.end(); ++mf_dlq) {
    mem_fetch *mf = mf_dlq->req;
    fprintf(fp, "Ready @ %llu - ", mf_dlq->ready_cycle);
    if (mf)
      mf->print(fp);
    else
      fprintf(fp, " <NULL mem_fetch?>\n");
  }
  m_dram->print(fp);
}

memory_sub_partition::memory_sub_partition(unsigned sub_partition_id,
                                           const memory_config *config,
                                           class memory_stats_t *stats,
                                           class gpgpu_sim *gpu) {
  m_id = sub_partition_id;
  m_config = config;
  m_stats = stats;
  m_gpu = gpu;
  m_memcpy_cycle_offset = 0;

  assert(m_id < m_config->m_n_mem_sub_partition);

  char L2c_name[32];
  snprintf(L2c_name, 32, "L2_bank_%03d", m_id);
  m_L2interface = new L2interface(this);
  m_mf_allocator = new partition_mf_allocator(config);

  if (!m_config->m_L2_config.disabled())
    m_L2cache =
        new l2_cache(L2c_name, m_config->m_L2_config, -1, -1, m_L2interface,
                     m_mf_allocator, IN_PARTITION_L2_MISS_QUEUE, gpu);

  unsigned int icnt_L2;
  unsigned int L2_dram;
  unsigned int dram_L2;
  unsigned int L2_icnt;
  sscanf(m_config->gpgpu_L2_queue_config, "%u:%u:%u:%u", &icnt_L2, &L2_dram,
         &dram_L2, &L2_icnt);
  m_icnt_L2_queue = new fifo_pipeline<mem_fetch>("icnt-to-L2", 0, icnt_L2);
  m_L2_dram_queue = new fifo_pipeline<mem_fetch>("L2-to-dram", 0, L2_dram);
  m_dram_L2_queue = new fifo_pipeline<mem_fetch>("dram-to-L2", 0, dram_L2);
  m_L2_icnt_queue = new fifo_pipeline<mem_fetch>("L2-to-icnt", 0, L2_icnt);

//Sitao
  m_L2_similarity_queue = new fifo_pipeline<mem_fetch>("L2-to-similarity", 0, 1000);

  wb_addr = -1;
}

memory_sub_partition::~memory_sub_partition() {
  delete m_icnt_L2_queue;
  delete m_L2_dram_queue;
  delete m_dram_L2_queue;
  delete m_L2_icnt_queue;
  delete m_L2cache;
  delete m_L2interface;

  //sitao
  delete m_L2_similarity_queue;
}

void memory_sub_partition::cache_cycle(unsigned cycle) {
  // L2 fill responses
  if (!m_config->m_L2_config.disabled()) {
    if (m_L2cache->access_ready() && !m_L2_icnt_queue->full()) {
      mem_fetch *mf = m_L2cache->next_access();
      if (mf->get_access_type() !=
          L2_WR_ALLOC_R) {  // Don't pass write allocate read request back to
                            // upper level cache
        mf->set_reply();
        mf->set_status(IN_PARTITION_L2_TO_ICNT_QUEUE,
                       m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        m_L2_icnt_queue->push(mf);
      } else {
        if (m_config->m_L2_config.m_write_alloc_policy == FETCH_ON_WRITE) {
          mem_fetch *original_wr_mf = mf->get_original_wr_mf();
          assert(original_wr_mf);
          original_wr_mf->set_reply();
          original_wr_mf->set_status(
              IN_PARTITION_L2_TO_ICNT_QUEUE,
              m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
          m_L2_icnt_queue->push(original_wr_mf);
        }
        m_request_tracker.erase(mf);
        delete mf;
      }
    }
  }

  // DRAM to L2 (texture) and icnt (not texture)
  if (!m_dram_L2_queue->empty()) {
    mem_fetch *mf = m_dram_L2_queue->top();
    if (!m_config->m_L2_config.disabled() && m_L2cache->waiting_for_fill(mf)) {
      if (m_L2cache->fill_port_free()) {
        mf->set_status(IN_PARTITION_L2_FILL_QUEUE,
                       m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
        m_L2cache->fill(mf, m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                                m_memcpy_cycle_offset);
        m_dram_L2_queue->pop();
      }
    } else if (!m_L2_icnt_queue->full()) {
      if (mf->is_write() && mf->get_type() == WRITE_ACK)
        mf->set_status(IN_PARTITION_L2_TO_ICNT_QUEUE,
                       m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      m_L2_icnt_queue->push(mf);
      m_dram_L2_queue->pop();
    }
  }

  // prior L2 misses inserted into m_L2_dram_queue here
  if (!m_config->m_L2_config.disabled()) m_L2cache->cycle();

  // new L2 texture accesses and/or non-texture accesses
  if (!m_L2_dram_queue->full() && !m_icnt_L2_queue->empty()) {
    mem_fetch *mf = m_icnt_L2_queue->top();
    if (!m_config->m_L2_config.disabled() &&
        ((m_config->m_L2_texure_only && mf->istexture()) ||
         (!m_config->m_L2_texure_only))) {
      // L2 is enabled and access is for L2
      bool output_full = m_L2_icnt_queue->full();
      bool port_free = m_L2cache->data_port_free();
      if (!output_full && port_free) {
        std::list<cache_event> events;
        enum cache_request_status status =
            m_L2cache->access(mf->get_addr(), mf,
                              m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
                                  m_memcpy_cycle_offset,
                              events);
        bool write_sent = was_write_sent(events);
        bool read_sent = was_read_sent(events);
        MEM_SUBPART_DPRINTF("Probing L2 cache Address=%llx, status=%u\n",
                            mf->get_addr(), status);

        if (status == HIT) {
          if (!write_sent) { //write allocate request is sent and we need to wait for reply from lower level memory, not reply it now
            
          /*new_addr_type probe_pointer = mf->get_addr();
            unsigned char *data = new unsigned char[32];
            for (int i = 0; i<32; i++){
              m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
               char x = data[i];
              fprintf(stdout, " %u", x & 0x00FF);
            }
            printf("/access address is %llx/", probe_pointer);
            int type = mf->get_access_type();
            printf("/access type is %d /",type);
            unsigned int access_size = mf->get_access_size();
            printf("/access size is %u/",access_size);
            //unsigned int uid = mf->get_request_uid();
            //printf("/uid is %u/", uid);
            //unsigned int previous_uid = mf->get_original_mf()->get_request_uid();
            //printf("/previous uid is %u/", previous_uid);
            printf("===cache hit\n");*/
           

            // L2 cache replies
            assert(!read_sent);
            if (mf->get_access_type() == L1_WRBK_ACC) {
              m_request_tracker.erase(mf);
              delete mf;
            } else {
              mf->set_reply();
              mf->set_status(IN_PARTITION_L2_TO_ICNT_QUEUE,
                             m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
              m_L2_icnt_queue->push(mf);
            }
            m_icnt_L2_queue->pop();
          } else {
            assert(write_sent);
            m_icnt_L2_queue->pop();
          }
        } else if (status != RESERVATION_FAIL) {
            /*
            //Sitao
            if (status == MISS)
            {
              enum mem_access_type type = mf->get_access_type();
              if (type == GLOBAL_ACC_R && !m_L2_similarity_queue->full()){
                m_L2_similarity_queue->push(mf);
              }
            }
            /*if (status == MISS){
            
            /*
            //Sitao's modification
            new_addr_type probe_pointer = mf->get_addr();
            unsigned char *data = new unsigned char[32];
            for (int i = 0; i<32; i++){
              m_gpu->get_global_memory()->read(probe_pointer+i, 1, data+i);
               char x = data[i];
              //fprintf(stdout, " %u", x & 0x00FF);
            }
            std::string data_string = memory_to_string(data,32);
            //,,printf("string is %s",data_string.c_str());
            //printf("/access address is %llx/", probe_pointer);
            enum mem_access_type type = mf->get_access_type();
            //printf("/access type is %d /",type);
            unsigned int access_size = mf->get_access_size();
            if (type == GLOBAL_ACC_R){
              //printf("================test test test===========");
              m_gpu->m_similarity_cache->check_and_update(data_string);
            }
            };
            */
          if (mf->is_write() &&
              (m_config->m_L2_config.m_write_alloc_policy == FETCH_ON_WRITE ||
               m_config->m_L2_config.m_write_alloc_policy ==
                   LAZY_FETCH_ON_READ) &&
              !was_writeallocate_sent(events)) {
            mf->set_reply();
            mf->set_status(IN_PARTITION_L2_TO_ICNT_QUEUE,
                           m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
            m_L2_icnt_queue->push(mf);
          }
          // L2 cache accepted request
          m_icnt_L2_queue->pop();
        } else {
          assert(!write_sent);
          assert(!read_sent);
          // L2 cache lock-up: will try again next cycle
        }
      }
    } else {
      // L2 is disabled or non-texture access to texture-only L2
      mf->set_status(IN_PARTITION_L2_TO_DRAM_QUEUE,
                     m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      m_L2_dram_queue->push(mf);
      m_icnt_L2_queue->pop();
    }
  }

  // ROP delay queue
  if (!m_rop.empty() && (cycle >= m_rop.front().ready_cycle) &&
      !m_icnt_L2_queue->full()) {
    mem_fetch *mf = m_rop.front().req;
    m_rop.pop();
    m_icnt_L2_queue->push(mf);
    mf->set_status(IN_PARTITION_ICNT_TO_L2_QUEUE,
                   m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
  }
}

bool memory_sub_partition::full() const { return m_icnt_L2_queue->full(); }

bool memory_sub_partition::full(unsigned size) const {
  return m_icnt_L2_queue->is_avilable_size(size);
}

bool memory_sub_partition::L2_dram_queue_empty() const {
  return m_L2_dram_queue->empty();
}

class mem_fetch *memory_sub_partition::L2_dram_queue_top() const {
  return m_L2_dram_queue->top();
}

void memory_sub_partition::L2_dram_queue_pop() { m_L2_dram_queue->pop(); }

bool memory_sub_partition::dram_L2_queue_full() const {
  return m_dram_L2_queue->full();
}

void memory_sub_partition::dram_L2_queue_push(class mem_fetch *mf) {
  m_dram_L2_queue->push(mf);
}

//Sitao
bool memory_sub_partition::L2_similarity_queue_full() const {
  return m_L2_similarity_queue->full();
}
bool memory_sub_partition::L2_similarity_queue_empty() const {
  return m_L2_similarity_queue->empty();
}
void memory_sub_partition::L2_similarity_queue_push(class mem_fetch *mf) {
  m_L2_similarity_queue->push(mf);
}
class mem_fetch *memory_sub_partition::L2_similarity_queue_top() const {
  return m_L2_similarity_queue->top();
}
void memory_sub_partition::L2_similarity_queue_pop() {
  m_L2_similarity_queue->pop();
}


void memory_sub_partition::print_cache_stat(unsigned &accesses,
                                            unsigned &misses) const {
  FILE *fp = stdout;
  if (!m_config->m_L2_config.disabled()) m_L2cache->print(fp, accesses, misses);
}

void memory_sub_partition::print(FILE *fp) const {
  if (!m_request_tracker.empty()) {
    fprintf(fp, "Memory Sub Parition %u: pending memory requests:\n", m_id);
    for (std::set<mem_fetch *>::const_iterator r = m_request_tracker.begin();
         r != m_request_tracker.end(); ++r) {
      mem_fetch *mf = *r;
      if (mf)
        mf->print(fp);
      else
        fprintf(fp, " <NULL mem_fetch?>\n");
    }
  }
  if (!m_config->m_L2_config.disabled()) m_L2cache->display_state(fp);
}

void memory_stats_t::visualizer_print(gzFile visualizer_file) {
  gzprintf(visualizer_file, "Ltwowritemiss: %d\n", L2_write_miss);
  gzprintf(visualizer_file, "Ltwowritehit: %d\n", L2_write_hit);
  gzprintf(visualizer_file, "Ltworeadmiss: %d\n", L2_read_miss);
  gzprintf(visualizer_file, "Ltworeadhit: %d\n", L2_read_hit);
  clear_L2_stats_pw();

  if (num_mfs)
    gzprintf(visualizer_file, "averagemflatency: %lld\n",
             mf_total_lat / num_mfs);
}

void memory_stats_t::clear_L2_stats_pw() {
  L2_write_miss = 0;
  L2_write_hit = 0;
  L2_read_miss = 0;
  L2_read_hit = 0;
}

void gpgpu_sim::print_dram_stats(FILE *fout) const {
  unsigned cmd = 0;
  unsigned activity = 0;
  unsigned nop = 0;
  unsigned act = 0;
  unsigned pre = 0;
  unsigned rd = 0;
  unsigned wr = 0;
  unsigned req = 0;
  unsigned tot_cmd = 0;
  unsigned tot_nop = 0;
  unsigned tot_act = 0;
  unsigned tot_pre = 0;
  unsigned tot_rd = 0;
  unsigned tot_wr = 0;
  unsigned tot_req = 0;

  for (unsigned i = 0; i < m_memory_config->m_n_mem; i++) {
    m_memory_partition_unit[i]->set_dram_power_stats(cmd, activity, nop, act,
                                                     pre, rd, wr, req);
    tot_cmd += cmd;
    tot_nop += nop;
    tot_act += act;
    tot_pre += pre;
    tot_rd += rd;
    tot_wr += wr;
    tot_req += req;
  }
  fprintf(fout, "gpgpu_n_dram_reads = %d\n", tot_rd);
  fprintf(fout, "gpgpu_n_dram_writes = %d\n", tot_wr);
  fprintf(fout, "gpgpu_n_dram_activate = %d\n", tot_act);
  fprintf(fout, "gpgpu_n_dram_commands = %d\n", tot_cmd);
  fprintf(fout, "gpgpu_n_dram_noops = %d\n", tot_nop);
  fprintf(fout, "gpgpu_n_dram_precharges = %d\n", tot_pre);
  fprintf(fout, "gpgpu_n_dram_requests = %d\n", tot_req);
}

unsigned memory_sub_partition::flushL2() {
  if (!m_config->m_L2_config.disabled()) {
    m_L2cache->flush();
  }
  return 0;  // TODO: write the flushed data to the main memory
}

unsigned memory_sub_partition::invalidateL2() {
  if (!m_config->m_L2_config.disabled()) {
    m_L2cache->invalidate();
  }
  return 0;
}

bool memory_sub_partition::busy() const { return !m_request_tracker.empty(); }

std::vector<mem_fetch *>
memory_sub_partition::breakdown_request_to_sector_requests(mem_fetch *mf) {
  std::vector<mem_fetch *> result;

  if (mf->get_data_size() == SECTOR_SIZE &&
      mf->get_access_sector_mask().count() == 1) {
    result.push_back(mf);
  } else if (mf->get_data_size() == 128 || mf->get_data_size() == 64) {
    // We only accept 32, 64 and 128 bytes reqs
    unsigned start = 0, end = 0;
    if (mf->get_data_size() == 128) {
      start = 0;
      end = 3;
    } else if (mf->get_data_size() == 64 &&
               mf->get_access_sector_mask().to_string() == "1100") {
      start = 2;
      end = 3;
    } else if (mf->get_data_size() == 64 &&
               mf->get_access_sector_mask().to_string() == "0011") {
      start = 0;
      end = 1;
    } else if (mf->get_data_size() == 64 &&
               (mf->get_access_sector_mask().to_string() == "1111" ||
                mf->get_access_sector_mask().to_string() == "0000")) {
      if (mf->get_addr() % 128 == 0) {
        start = 0;
        end = 1;
      } else {
        start = 2;
        end = 3;
      }
    } else {
      printf(
          "Invalid sector received, address = 0x%06llx, sector mask = %s, data "
          "size = %d",
          mf->get_addr(), mf->get_access_sector_mask(), mf->get_data_size());
      assert(0 && "Undefined sector mask is received");
    }

    std::bitset<SECTOR_SIZE * SECTOR_CHUNCK_SIZE> byte_sector_mask;
    byte_sector_mask.reset();
    for (unsigned k = start * SECTOR_SIZE; k < SECTOR_SIZE; ++k)
      byte_sector_mask.set(k);

    for (unsigned j = start, i = 0; j <= end; ++j, ++i) {
      const mem_access_t *ma = new mem_access_t(
          mf->get_access_type(), mf->get_addr() + SECTOR_SIZE * i, SECTOR_SIZE,
          mf->is_write(), mf->get_access_warp_mask(),
          mf->get_access_byte_mask() & byte_sector_mask,
          std::bitset<SECTOR_CHUNCK_SIZE>().set(j), m_gpu->gpgpu_ctx);

      mem_fetch *n_mf =
          new mem_fetch(*ma, NULL, mf->get_ctrl_size(), mf->get_wid(),
                        mf->get_sid(), mf->get_tpc(), mf->get_mem_config(),
                        m_gpu->gpu_tot_sim_cycle + m_gpu->gpu_sim_cycle, mf);

      result.push_back(n_mf);
      byte_sector_mask <<= SECTOR_SIZE;
    }
  } else {
    printf(
        "Invalid sector received, address = 0x%06llx, sector mask = %d, byte "
        "mask = , data size = %u",
        mf->get_addr(), mf->get_access_sector_mask().count(),
        mf->get_data_size());
    assert(0 && "Undefined data size is received");
  }

  return result;
}

void memory_sub_partition::push(mem_fetch *m_req, unsigned long long cycle) {
  if (m_req) {
    m_stats->memlatstat_icnt2mem_pop(m_req);
    std::vector<mem_fetch *> reqs;
    if (m_config->m_L2_config.m_cache_type == SECTOR)
      reqs = breakdown_request_to_sector_requests(m_req);
    else
      reqs.push_back(m_req);

    for (unsigned i = 0; i < reqs.size(); ++i) {
      mem_fetch *req = reqs[i];
      m_request_tracker.insert(req);
      if (req->istexture()) {
        m_icnt_L2_queue->push(req);
        req->set_status(IN_PARTITION_ICNT_TO_L2_QUEUE,
                        m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      } else {
        rop_delay_t r;
        r.req = req;
        r.ready_cycle = cycle + m_config->rop_latency;
        m_rop.push(r);
        req->set_status(IN_PARTITION_ROP_DELAY,
                        m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle);
      }
    }
  }
}

mem_fetch *memory_sub_partition::pop() {
  mem_fetch *mf = m_L2_icnt_queue->pop();
  m_request_tracker.erase(mf);
  if (mf && mf->isatomic()) mf->do_atomic();
  if (mf && (mf->get_access_type() == L2_WRBK_ACC ||
             mf->get_access_type() == L1_WRBK_ACC)) {
    delete mf;
    mf = NULL;
  }
  return mf;
}

mem_fetch *memory_sub_partition::top() {
  mem_fetch *mf = m_L2_icnt_queue->top();
  if (mf && (mf->get_access_type() == L2_WRBK_ACC ||
             mf->get_access_type() == L1_WRBK_ACC)) {
    m_L2_icnt_queue->pop();
    m_request_tracker.erase(mf);
    delete mf;
    mf = NULL;
  }
  return mf;
}

void memory_sub_partition::set_done(mem_fetch *mf) {
  m_request_tracker.erase(mf);
}

void memory_sub_partition::accumulate_L2cache_stats(
    class cache_stats &l2_stats) const {
  if (!m_config->m_L2_config.disabled()) {
    l2_stats += m_L2cache->get_stats();
  }
}

void memory_sub_partition::get_L2cache_sub_stats(
    struct cache_sub_stats &css) const {
  if (!m_config->m_L2_config.disabled()) {
    m_L2cache->get_sub_stats(css);
  }
}

void memory_sub_partition::get_L2cache_sub_stats_pw(
    struct cache_sub_stats_pw &css) const {
  if (!m_config->m_L2_config.disabled()) {
    m_L2cache->get_sub_stats_pw(css);
  }
}

void memory_sub_partition::clear_L2cache_stats_pw() {
  if (!m_config->m_L2_config.disabled()) {
    m_L2cache->clear_pw();
  }
}

void memory_sub_partition::visualizer_print(gzFile visualizer_file) {
  // Support for L2 AerialVision stats
  // Per-sub-partition stats would be trivial to extend from this
  cache_sub_stats_pw temp_sub_stats;
  get_L2cache_sub_stats_pw(temp_sub_stats);

  m_stats->L2_read_miss += temp_sub_stats.read_misses;
  m_stats->L2_write_miss += temp_sub_stats.write_misses;
  m_stats->L2_read_hit += temp_sub_stats.read_hits;
  m_stats->L2_write_hit += temp_sub_stats.write_hits;

  clear_L2cache_stats_pw();
}
