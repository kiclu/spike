#include "dpi_spike_proc.hpp"
#include "decode_macros.h"
#include "arith.h"
#include "mmu.h"
#include "softfloat.h"
#include "internals.h"
#include "specialize.h"
#include "tracer.h"

// Define uint128_t for vector extension support
#if defined(__SIZEOF_INT128__)
typedef __uint128_t uint128_t;
typedef __int128_t int128_t;
#endif

#include "v_ext_macros.h"
#include "debug_defines.h"
#include "insn_macros.h"
#include <assert.h>

#define DECODE_MACRO_USAGE_LOGGED 0

#define PROLOGUE \
  reg_t npc = sext_xlen(pc + insn_length(OPCODE))

#define EPILOGUE \
  trace_opcode(p, OPCODE, insn); \
  return npc

// DPI versions using dpi_spike_proc_t* instead of processor_t*
reg_t dpi_fast_rv32i_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

reg_t dpi_fast_rv64i_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

reg_t dpi_logged_rv32i_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

reg_t dpi_logged_rv64i_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef CHECK_REG
#define CHECK_REG(reg) require((reg) < 16)

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 0

reg_t dpi_fast_rv32e_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

reg_t dpi_fast_rv64e_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

reg_t dpi_logged_rv32e_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

reg_t dpi_logged_rv64e_NAME(dpi_spike_proc_t* p, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "../riscv/insns/NAME.h"
  EPILOGUE;
  #undef xlen
}