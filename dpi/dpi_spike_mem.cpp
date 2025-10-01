#include "dpi_spike_mem.hpp"

dpi_spike_mem_t::dpi_spike_mem_t(size_t size)
: mem_size(size)
{
}

dpi_spike_mem_t::~dpi_spike_mem_t()
{
}

bool dpi_spike_mem_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  return true;
}

bool dpi_spike_mem_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  return true;
}

char* dpi_spike_mem_t::contents(reg_t addr)
{
 return nullptr;
}

void dpi_spike_mem_t::dump(std::ostream& o)
{
  //for(reg_t i = 0; i < mem_size; i += PAGE_SIZE)
  //{
  //  reg_t ppn = i >> PGSHIFT;
  //  auto search = sparse_memory_map.find(ppn);
  //  if(search == sparse_memory_map.end())
  //  {
  //    o.write(empty, PAGE_SIZE);
  //  }
  //}
}