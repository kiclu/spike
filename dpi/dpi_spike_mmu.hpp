#ifndef DPI_SPIKE_MMU_HPP
#define DPI_SPIKE_MMU_HPP

#include <riscv/mmu.h>
#include <riscv/simif.h>
#include <riscv/processor.h>

class dpi_spike_mmu_t : public mmu_t
{
public:

  dpi_spike_mmu_t(simif_t* sim, endianness_t endianness, processor_t* proc, reg_t cache_blocksz);

  ~dpi_spike_mmu_t() = default;

  template<typename T>
  T load(reg_t addr, xlate_flags_t xlate_flags = {})
  {
    dbg_load();
    return 0;
  }

  template<typename T>
  void ALWAYS_INLINE store(reg_t addr, T val, xlate_flags_t xlate_flags = {})
  {
    dbg_store();
  }

  void load_slow_path(reg_t original_addr, reg_t len, uint8_t* bytes,
                      xlate_flags_t xlate_flags);

  void dbg_load();
  void dbg_store();

};

#endif // DPI_SPIKE_MMU_HPP