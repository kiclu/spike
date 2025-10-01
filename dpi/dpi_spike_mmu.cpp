#include "dpi_spike_mmu.hpp"

#include <vpi_user.h>
#include <svdpi.h>

dpi_spike_mmu_t::dpi_spike_mmu_t(simif_t* sim, endianness_t endianness,
                                 processor_t* proc, reg_t cache_blocksz)
: mmu_t(sim, endianness, proc, cache_blocksz) {}

void dpi_spike_mmu_t::load_slow_path(reg_t original_addr, reg_t len, uint8_t* bytes,
                                     xlate_flags_t xlate_flags)
{
  vpi_printf("[SPIKE][MMU] LOAD_SLOW_PATH\n");
}

void dpi_spike_mmu_t::dbg_load()
{
  vpi_printf("[SPIKE][MMU] LOAD\n");
}

void dpi_spike_mmu_t::dbg_store()
{
  vpi_printf("[SPIKE][MMU] STORE\n");
}