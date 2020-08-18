#include "../inc/ooo_cpu.h"
#include "circular_buffer.h"
#include "signature_table.hh"

#include <list>

SignatureTable signature_table("SIGNATURE_TABLE", ST_SET, ST_WAY, ST_SET *ST_WAY);
list<uint64_t> return_address_stack;
list<uint64_t> branch_history_table;
CB::CircularBuffer<uint64_t> *circular_buffer = new CB::CircularBuffer<uint64_t>();

void O3_CPU::l1i_prefetcher_initialize()
{
}

void O3_CPU::l1i_prefetcher_branch_operate(uint64_t ip, uint8_t branch_type, uint64_t branch_target)
{
    if (branch_type == NOT_BRANCH)
        return;

    if (branch_type == BRANCH_DIRECT_CALL || branch_type == BRANCH_INDIRECT_CALL || branch_type == BRANCH_RETURN)
    {
        if (branch_type == BRANCH_DIRECT_CALL || branch_type == BRANCH_INDIRECT_CALL)
        {
            return_address_stack.push_back(ip);
            if (return_address_stack.size() > RAS_ENTRY)
                return_address_stack.pop_front();
        }

        uint64_t signature_set = 0;
        list<uint64_t>::reverse_iterator iter = return_address_stack.rbegin();
        for (uint32_t i = 0; i < RAS_TOP_N_ENTRY; i++)
            signature_set ^= *iter++;

        uint64_t signature_tag = 0;
        for (list<uint64_t>::iterator iter = branch_history_table.begin(); iter != branch_history_table.end(); iter++)
            signature_tag ^= *iter;

        uint64_t signature = (signature_set >> (64 - LOG2_ST_SET)) | (signature_tag << LOG2_ST_SET);
        //signature_table.handle_fill(signature, data, data_count);

        if (branch_type == BRANCH_RETURN)
            return_address_stack.pop_back();

        branch_history_table.clear();
    }
    else
    {
        branch_history_table.push_back(ip);
        if (branch_history_table.size() > BHT_ENTRY)
            branch_history_table.pop_front();
    }
}

void O3_CPU::l1i_prefetcher_cache_operate(uint64_t v_addr, uint8_t cache_hit, uint8_t prefetch_hit)
{
    if (cache_hit == 0)
    {
        circular_buffer->enqueue(v_addr);
    }
}

void O3_CPU::l1i_prefetcher_cycle_operate()
{
}

void O3_CPU::l1i_prefetcher_cache_fill(uint64_t v_addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_v_addr)
{
}

void O3_CPU::l1i_prefetcher_final_stats()
{
}
