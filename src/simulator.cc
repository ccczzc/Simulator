#include "simulator.h"
#include <cstddef>
#include <iostream>

Simulator::Simulator()
    : memory_(10, 0), register_(32, 0), pipeline_(5, -1) {}

Simulator::Simulator(std::vector<long> init_memory,
                     std::vector<Instruction> instructions,
                     std::vector<std::string> instruction_text)
    : memory_(init_memory), register_(32, 0), pipeline_(5, -1) {
  memory_.resize(10);
  instructions_ = std::move(instructions);
  instruction_text_ = std::move(instruction_text);
}

void Simulator::PrintInstructions() const {
  for (size_t i = 0; i < instruction_text_.size(); ++i) {
    std::cout << i << '\t' << instruction_text_[i] << '\n';
  }
}
void Simulator::PrintPipelines() const {
  for (size_t ppl_idx = 0; ppl_idx < 5; ++ppl_idx) {
    std::cout << pipeline_name[ppl_idx] << '\t';
    if (pipeline_[ppl_idx] == (unsigned long)-1) {
      std::cout << "nop\n";
    } else {
      std::cout << instruction_text_[pipeline_[ppl_idx]] << '\n';
    }
  }
}
void Simulator::PrintRegisters() const {
  for (size_t reg_idx = 0; reg_idx < 32; ++reg_idx) {
    std::cout << 'R' << reg_idx << '\t' << register_[reg_idx] << '\t';
    if ((reg_idx + 1) % 4 == 0) {
      std::cout << '\n';
    }
  }
}
void Simulator::PrintBreakpoints() const {}

void Simulator::SetBreakpoint(size_t instruction_index) {
  instructions_[instruction_index].is_breakpoint_ = true;
}

void Simulator::PrintUsage() {
  std::cout
      << "Usage: \n"
      << "v [i | p | r | b]: print "
         "instructions/pipelines/registers/breakpoints\n"
      << "b [instruction index] : set breakpoint at instruction of the index\n"
      << "s : single cycles\n"
      << "r : run to the end or breakpoint\n"
      << "q : quit the simulator\n";
}

bool Simulator::IsFinished() const { return false; }

bool Simulator::SingleCycle() {
  if (pipeline_[4] != -1) {
    instructions_[pipeline_[4]].is_finished_ = true;
  }
  
  pipeline_[4] = pipeline_[3];  // update WB
  pipeline_[3] = pipeline_[2];  // update MEM
  pipeline_[2] = pipeline_[1];  // update EX
  size_t old_IF = pipeline_[0];
  bool should_stall = false;
  if (old_IF != -1) {
    auto& old_IF_Inst = instructions_[old_IF];
    if (pipeline_[2] != -1) {   // check EX
      auto& EX_Inst = instructions_[pipeline_[2]];
      if (EX_Inst.rd_ == old_IF_Inst.rs_or_label_ or EX_Inst.rd_ == old_IF_Inst.rt_or_imm_) {
        should_stall = true;
      }
    }
    if (!should_stall and pipeline_[3] != -1) {  // check MEM
      auto& MEM_Inst = instructions_[pipeline_[3]];
      if (MEM_Inst.rd_ == old_IF_Inst.rs_or_label_ or MEM_Inst.rd_ == old_IF_Inst.rt_or_imm_) {
        should_stall = true;
      }
    }
    if (should_stall) {
      pipeline_[1] = -1;
    } else {
      pipeline_[1] = old_IF;
    }
  } else {
    pipeline_[1] = -1;
  }
  if (should_stall) {

  } else if (pc_ >= instructions_.size()) {
    pipeline_[0] = -1;
  } else {
    pipeline_[0] = pc_;
    ++pc_;
  }
  return true;
}
bool Simulator::RunToStop() {
  return true;
}