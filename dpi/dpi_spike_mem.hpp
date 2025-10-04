#ifndef DPI_SPIKE_MEM_HPP
#define DPI_SPIKE_MEM_HPP

#include <riscv/devices.h>

#include <unordered_map>
#include <cstring>

static const size_t PAGE_SIZE = 4096;

class dpi_spike_mem_t : public abstract_mem_t
{
public:
  dpi_spike_mem_t(size_t size);

  ~dpi_spike_mem_t();

  bool load(reg_t addr, size_t len, uint8_t* bytes) override;
  bool store(reg_t addr, size_t len, const uint8_t* bytes) override;
  char* contents(reg_t addr) override;
  inline reg_t size() override { return mem_size; }
  void dump(std::ostream& o) override;

  void write_byte(reg_t addr, uint8_t data);
  uint8_t read_byte(reg_t addr);

private:
  reg_t mem_size;

  std::unordered_map<reg_t, uint8_t*> pages;
  uint8_t* get_page(reg_t page_addr);
};

#endif // DPI_SPIKE_MEM_HPP