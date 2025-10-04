#include "config.h"
#include "dpi_spike_mmu.hpp"

#include <vpi_user.h>
#include <svdpi.h>

dpi_spike_mmu_t::dpi_spike_mmu_t(simif_t* sim, endianness_t endianness,
                                 processor_t* proc, reg_t cache_blocksz)
: mmu_t(sim, endianness, proc, cache_blocksz) {}