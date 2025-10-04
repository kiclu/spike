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
    target_endian<T> res;

    if(sim->mmio_load(addr, sizeof(T), (uint8_t*)&res))
    {
      MMU_OBSERVE_LOAD(addr, from_target(res), sizeof(T));
      return from_target(res);
    }
    return 0;
  }

  template<typename T>
  void ALWAYS_INLINE store(reg_t addr, T val, xlate_flags_t xlate_flags = {})
  {
    target_endian<T> data = to_target(val);

    MMU_OBSERVE_STORE(addr, val, sizeof(T));
    sim->mmio_store(addr, sizeof(T), (uint8_t*)&data);
  }

};

#endif // DPI_SPIKE_MMU_HPP