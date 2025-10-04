#include "config.h"
#include "dpi_spike_mem.hpp"

dpi_spike_mem_t::dpi_spike_mem_t(size_t size)
: mem_size(size) {}

dpi_spike_mem_t::~dpi_spike_mem_t() {}

bool dpi_spike_mem_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  if (addr + len > mem_size) return false;

  for (size_t i = 0; i < len; ++i)
  {
    bytes[i] = read_byte(addr + i);
  }
  return true;
}

bool dpi_spike_mem_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  if (addr + len > mem_size) return false;

  for (size_t i = 0; i < len; ++i)
  {
    write_byte(addr + i, bytes[i]);
  }
  return true;
}

char* dpi_spike_mem_t::contents(reg_t addr)
{
  reg_t page_addr = addr & ~(PAGE_SIZE - 1);
  return (char*)(get_page(page_addr) + (addr - page_addr));
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

void dpi_spike_mem_t::write_byte(reg_t addr, uint8_t data)
{
  reg_t page_addr = addr & ~(PAGE_SIZE - 1);
  reg_t page_offset = addr - page_addr;
  uint8_t* page = get_page(page_addr);
  page[page_offset] = data;
}

uint8_t dpi_spike_mem_t::read_byte(reg_t addr)
{
  reg_t page_addr = addr & ~(PAGE_SIZE - 1);
  reg_t page_offset = addr - page_addr;
  uint8_t* page = get_page(page_addr);
  return page[page_offset];
}

uint8_t* dpi_spike_mem_t::get_page(reg_t page_addr)
{
  auto it = pages.find(page_addr);
  if (it == pages.end())
  {
    uint8_t* page = new uint8_t[PAGE_SIZE]();
    pages[page_addr] = page;
    return page;
  }
  return it->second;
}