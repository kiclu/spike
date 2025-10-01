#ifndef DPI_SPIKE_PROC_HPP
#define DPI_SPIKE_PROC_HPP

#include <riscv/processor.h>

#include "dpi_spike_mmu.hpp"

class dpi_spike_proc_t : public processor_t {
public:
  dpi_spike_proc_t() = default;
  dpi_spike_proc_t(const char* isa_str, const char* priv_str,
                   const cfg_t* cfg,
                   simif_t* sim, uint32_t id, bool halt_on_reset);
  ~dpi_spike_proc_t() override;

  void step(uint64_t addr, uint32_t insn, uint64_t* next_pc);

  insn_func_t decode_insn(insn_t insn);

  inline dpi_spike_mmu_t* get_mmu() { return dpi_mmu; }

  void register_base_instructions();

  bool get_rd_we();
  uint8_t get_rd_addr();
  uint64_t get_rd_data();

  uint64_t get_pc();

  uint64_t get_ireg(uint8_t addr);

  void vpi_log_disasm(insn_t& insn, state_t& state);
  void vpi_log_trap(trap_t& trap);
private:
  dpi_spike_mmu_t* dpi_mmu;
};

#endif // DPI_SPIKE_PROC_HPP