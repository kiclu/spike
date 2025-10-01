#ifndef DPI_SPIKE_SIM_HPP
#define DPI_SPIKE_SIM_HPP

#include "dpi_spike_mmu.hpp"
#include "dpi_spike_proc.hpp"

#include <riscv/sim.h>

class dpi_spike_sim_t : public sim_t {
public:
  dpi_spike_sim_t(const cfg_t* cfg, bool halted,
                  std::vector<std::pair<reg_t, abstract_mem_t*>> mems,
                  const std::vector<device_factory_sargs_t>& plugin_device_factories,
                  const std::vector<std::string>& args,
                  const debug_module_config_t &dm_config,
                  const char *log_path,
                  bool dtb_enabled, const char *dtb_file,
                  bool socket_enabled,
                  FILE *cmd_file, // needed for command line option --cmd
                  std::optional<unsigned long long> instruction_limit);
  ~dpi_spike_sim_t();

  inline dpi_spike_proc_t* get_core(size_t i) { return static_cast<dpi_spike_proc_t*>(procs.at(i)); }

public:
  dpi_spike_mmu_t* debug_mmu;
};

#endif // DPI_SPIKE_SIM_HPP