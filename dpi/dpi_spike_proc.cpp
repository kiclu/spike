#include "config.h"
#include "dpi_spike_proc.hpp"

#include <svdpi.h>
#include <vpi_user.h>

#include <riscv/disasm.h>
#include <riscv/extension.h>
#include <riscv/processor.h>
#include <softfloat/softfloat.h>

#undef STATE
#define STATE state

dpi_spike_proc_t::dpi_spike_proc_t(const char* isa_str, const char* priv_str,
                                   const cfg_t* cfg,
                                   simif_t* sim, uint32_t id, bool halt_on_reset)
: processor_t(isa_str, priv_str, cfg, sim, id, halt_on_reset)
{

  if(dpi_mmu)
  {
    delete dpi_mmu;
    dpi_mmu = nullptr;
  }

  dpi_mmu = new dpi_spike_mmu_t(sim, cfg->endianness, this, cfg->cache_blocksz);

  register_base_instructions();

  disassembler = new disassembler_t(&isa);
  for (auto e : isa.get_extensions())
    register_extension(find_extension(e.c_str())());

  //set_pmp_granularity(cfg->pmpgranularity);
  //set_pmp_num(cfg->pmpregions);

  //if (isa.get_max_xlen() == 32)
  //  set_mmu_capability(IMPL_MMU_SV32);
  //else if (isa.get_max_xlen() == 64)
  //  set_mmu_capability(IMPL_MMU_SV57);

  set_impl(IMPL_MMU_ASID, true);
  set_impl(IMPL_MMU_VMID, true);

  reset();
}

dpi_spike_proc_t::~dpi_spike_proc_t() {}

void dpi_spike_proc_t::step(uint64_t addr, uint32_t insn, uint64_t* next_pc) {
  insn_fetch_t fetch = {decode_insn(insn), insn};
  state_t cur_state = *get_state();

  try
  {
    // clear register log
    state.log_reg_write.clear();

    // execute instruction
    *next_pc = fetch.func(static_cast<dpi_spike_proc_t*>(this), fetch.insn, addr);
    state.pc = *next_pc;
  }
  catch(trap_t& t)
  {
    // check for any traps
    //vpi_log_trap(t);
  }
  catch(const std::exception& e)
  {
  }

  vpi_log_disasm(fetch.insn, cur_state);
}

insn_func_t dpi_spike_proc_t::decode_insn(insn_t insn)
{
  // removed opcode cache and custom instruction support
  bool rve = extension_enabled('E');
  insn_desc_t* desc;

  auto matching = [insn_bits = insn.bits()](const insn_desc_t &d) {
    return (insn_bits & d.mask) == d.match;
  };

  auto p = std::find_if(instructions.begin(), instructions.end(), matching);
  assert(p != instructions.end());
  desc = &*p;

  std::string disasm = disassembler->disassemble(insn);
  std::string::iterator new_end =
          std::unique(disasm.begin(), disasm.end(),
          [=](char lhs, char rhs){ return (lhs == rhs) && (lhs == ' '); }
          );
  disasm.erase(new_end, disasm.end());

  vpi_printf("[SPIKE][HARTID=%2d] Decoded instruction 0x%08lx (%s)\n",
             get_id(), insn.bits(), disasm.c_str());

  return desc->func(xlen, rve, log_commits_enabled);
}

void dpi_spike_proc_t::register_base_instructions()
{
  #define DECLARE_INSN(name, match, mask) \
    insn_bits_t name##_match = (match), name##_mask = (mask); \
    isa_extension_t name##_ext = NUM_ISA_EXTENSIONS; \
    bool name##_overlapping = false;

  #include <encoding.h>
  #undef DECLARE_INSN

  // Use DPI-specific instruction templates with dpi_spike_proc_t*
  #define DEFINE_INSN(name) \
    extern reg_t dpi_fast_rv32i_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_fast_rv64i_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_fast_rv32e_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_fast_rv64e_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_logged_rv32i_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_logged_rv64i_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_logged_rv32e_##name(dpi_spike_proc_t*, insn_t, reg_t); \
    extern reg_t dpi_logged_rv64e_##name(dpi_spike_proc_t*, insn_t, reg_t);
  #include "insn_list.h"
  #undef DEFINE_INSN

  #define DEFINE_INSN_UNCOND(name) { \
    insn_desc_t insn = { \
      name##_match, \
      name##_mask, \
      reinterpret_cast<insn_func_t>(dpi_fast_rv32i_##name), \
      reinterpret_cast<insn_func_t>(dpi_fast_rv64i_##name), \
      reinterpret_cast<insn_func_t>(dpi_fast_rv32e_##name), \
      reinterpret_cast<insn_func_t>(dpi_fast_rv64e_##name), \
      reinterpret_cast<insn_func_t>(dpi_logged_rv32i_##name), \
      reinterpret_cast<insn_func_t>(dpi_logged_rv64i_##name), \
      reinterpret_cast<insn_func_t>(dpi_logged_rv32e_##name), \
      reinterpret_cast<insn_func_t>(dpi_logged_rv64e_##name) \
    }; \
    register_base_insn(insn); \
  }

  // add overlapping instructions first, in order
  #define DECLARE_OVERLAP_INSN(name, ext) \
    name##_overlapping = true; \
    if (isa.extension_enabled(ext)) \
      DEFINE_INSN_UNCOND(name);
  #include "../riscv/overlap_list.h"
  #undef DECLARE_OVERLAP_INSN

  // add all other instructions.  since they are non-overlapping, the order
  // does not affect correctness, but more frequent instructions should
  // appear earlier to improve search time on opcode_cache misses.
  #define DEFINE_INSN(name) \
    if (!name##_overlapping) \
      DEFINE_INSN_UNCOND(name);
  #include "insn_list.h"
  #undef DEFINE_INSN
  #undef DEFINE_INSN_UNCOND

  // terminate instruction list with a catch-all
  register_base_insn(insn_desc_t::illegal_instruction);

  build_opcode_map();
}

bool dpi_spike_proc_t::get_rd_we()
{
  return state.log_reg_write.size() == 1;
}

uint8_t dpi_spike_proc_t::get_rd_addr()
{
  assert(state.log_reg_write.size() <= 1);
  for (const auto& e : state.log_reg_write)
    return e.first >> 4;
}

uint64_t dpi_spike_proc_t::get_rd_data()
{
  assert(state.log_reg_write.size() <= 1);
  for (const auto& e : state.log_reg_write)
    return *(uint64_t*)&e.second;
}

uint64_t dpi_spike_proc_t::get_pc()
{
  return state.pc;
}

uint64_t dpi_spike_proc_t::get_ireg(uint8_t addr)
{
  return state.XPR[addr];
}

void dpi_spike_proc_t::vpi_log_disasm(insn_t& insn, state_t& state)
{
  std::string disasm = disassembler->disassemble(insn);
  std::string::iterator new_end =
          std::unique(disasm.begin(), disasm.end(),
          [=](char lhs, char rhs){ return (lhs == rhs) && (lhs == ' '); }
          );
  disasm.erase(new_end, disasm.end());

  vpi_printf("[SPIKE][HARTID=%2d]\n", get_id());
  vpi_printf("  Executing instruction 0x%08lx (%s)\n", insn.bits(), disasm.c_str());
  vpi_printf("  RS2:  x%02ld [=0x%016lx]\n", insn.rs2(), state.XPR[insn.rs2()]);
  vpi_printf("  RS1:  x%02ld [=0x%016lx]\n", insn.rs1(), state.XPR[insn.rs1()]);
  if (get_rd_we())
  {
    vpi_printf("  RD:   x%02ld [=0x%016lx]\n", get_rd_addr(), get_rd_data());
  }
}

void dpi_spike_proc_t::vpi_log_trap(trap_t& trap)
{
  vpi_printf("  TRAP: %s\n", trap.name().c_str());
}