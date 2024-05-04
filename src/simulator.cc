#include "simulator.h"

Simulator::Simulator() : memory_(10, 0), register_(32, 0) {}

Simulator::Simulator(std::vector<long> init_memory,
                     std::vector<Instruction> instruction)
    : memory_(10, 0), register_(32, 0) {
  memory_.insert(memory_.end(), init_memory.begin(), init_memory.end());
  instruction_ = std::move(instruction);
}