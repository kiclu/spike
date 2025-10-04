#include "config.h"
#include "dpi_spike_sim.hpp"

#include <vpi_user.h>
#include <svdpi.h>

dpi_spike_sim_t::dpi_spike_sim_t(const cfg_t* cfg, bool halted,
                                 std::vector<std::pair<reg_t, abstract_mem_t*>> mems,
                                 const std::vector<device_factory_sargs_t>& plugin_device_factories,
                                 const std::vector<std::string>& args,
                                 const debug_module_config_t &dm_config,
                                 const char *log_path,
                                 bool dtb_enabled, const char *dtb_file,
                                 bool socket_enabled,
                                 FILE *cmd_file, // needed for command line option --cmd
                                 std::optional<unsigned long long> instruction_limit)
: sim_t(cfg, dm_config)
{

  for (auto& x : mems)
    bus.add_device(x.first, x.second);

  debug_mmu = new dpi_spike_mmu_t(this, cfg->endianness, NULL, cfg->cache_blocksz);

  // create procs
  for (size_t i = 0; i < cfg->nprocs(); ++i)
  {

    dpi_spike_proc_t* proc = new dpi_spike_proc_t(cfg->isa, cfg->priv,
                                                  cfg, this, cfg->hartids[i], halted);

    procs.push_back(proc);
    harts[cfg->hartids[i]] = proc;

    vpi_printf("[SPIKE][HARTID-%2d] Created\n", cfg->hartids[i]);

    //procs->set_pmp_num();
    //procs->set_pmp_granularity();
    //procs->set_mmu_capability();
    proc->reset();

    vpi_printf("[SPIKE][HARTID-%2d] Reset\n", cfg->hartids[i]);
  }

}

dpi_spike_sim_t::~dpi_spike_sim_t()
{
  for(size_t i = 0; i < procs.size(); ++i)
  {
    delete static_cast<dpi_spike_proc_t*>(procs[i]);
  }
  //delete debug_mmu;
}