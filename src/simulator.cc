#include "simulator.h"
#include <cstddef>
#include <iostream>

Simulator::Simulator() : memory_(16, 0), register_(32, 0), pipeline_(5, -1) {}

Simulator::Simulator(std::vector<long> init_memory,
                     std::vector<Instruction> instructions,
                     std::vector<std::string> instruction_text,
                     bool enable_forwarding)
    : memory_(init_memory), register_(32, 0), pipeline_(5, -1),
      enable_forwarding_(enable_forwarding) {
  memory_.resize(16);
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
void Simulator::PrintBreakpoints() const {
  for (size_t i = 0; i < instruction_text_.size(); ++i) {
    std::cout << i << '\t' << (instructions_[i].is_breakpoint_ ? "BID\t" : "\t")
              << instruction_text_[i] << '\n';
  }
}
void Simulator::PrintMemory() const {
  std::cout
      << "Address\tValue\tAddress\tValue\tAddress\tValue\tAddress\tValue\t\n";
  for (size_t mem_idx = 0; mem_idx < memory_.size(); ++mem_idx) {
    std::cout << mem_idx << '\t' << memory_[mem_idx] << '\t';
    if ((mem_idx + 1) % 4 == 0) {
      std::cout << '\n';
    }
  }
}
void Simulator::PrintStatistics() const {
  std::cout << "Total:\n\t" << cycle_clocks_ << " cycle clocks executed\n";
  std::cout << "Stalls:\n"
            << "\tRAW stalls: " << raw_stalls_ << "\n"
            << "\tControl stalls: " << control_stalls_ << "\n"
            << "\tTotal: " << raw_stalls_ + control_stalls_ << "\n";
}

void Simulator::SetBreakpoint(size_t instruction_index) {
  instructions_[instruction_index].is_breakpoint_ = true;
  std::cout << "Set Breakpoint at:\t" << instruction_index << '\t'
            << instruction_text_[instruction_index] << '\n';
}

bool Simulator::IsFinished() const {
  return instructions_.size() == 0 or pc_ >= instructions_.size() + 5;
}

bool Simulator::SingleCycle() {
  if (IsFinished()) {
    std::cerr << "!!!All the instructions has been executed!!!\n";
    return true;
  }
  if (pipeline_[4] != -1) {
    instructions_[pipeline_[4]].is_finished_ = true;
  }

  pipeline_[4] = pipeline_[3]; // update WB
  if (pipeline_[4] != -1) {
    auto &WB_Inst = instructions_[pipeline_[3]];
    switch (WB_Inst.instruction_op_) {
    case InstructionOp::LOAD:
      register_[WB_Inst.rd_] =
          memory_[register_[WB_Inst.rs_or_label_] + WB_Inst.rt_or_imm_];
      break;
    case InstructionOp::STORE:
      memory_[register_[WB_Inst.rs_or_label_] + WB_Inst.rt_or_imm_] =
          register_[WB_Inst.rd_];
      break;
    case InstructionOp::ADDI:
      register_[WB_Inst.rd_] =
          register_[WB_Inst.rs_or_label_] + WB_Inst.rt_or_imm_;
      break;
    case InstructionOp::SUBI:
      register_[WB_Inst.rd_] =
          register_[WB_Inst.rs_or_label_] - WB_Inst.rt_or_imm_;
      break;
    case InstructionOp::ADD:
      register_[WB_Inst.rd_] =
          register_[WB_Inst.rs_or_label_] + register_[WB_Inst.rt_or_imm_];
      break;
    case InstructionOp::SUB:
      register_[WB_Inst.rd_] =
          register_[WB_Inst.rs_or_label_] - register_[WB_Inst.rt_or_imm_];
      break;
    default:
      break;
    }
  }
  pipeline_[3] = pipeline_[2]; // update MEM
  pipeline_[2] = pipeline_[1]; // update EX
  size_t old_IF = pipeline_[0];
  bool should_stall = false;
  if (old_IF != -1) {
    auto &old_IF_Inst = instructions_[old_IF];
    if (pipeline_[2] != -1) { // check EX
      auto &EX_Inst = instructions_[pipeline_[2]];
      should_stall = HasHazard(old_IF_Inst, EX_Inst);
    }
    if (!should_stall && pipeline_[3] != -1) { // check MEM
      auto &MEM_Inst = instructions_[pipeline_[3]];
      should_stall = HasHazard(old_IF_Inst, MEM_Inst);
    }
  }
  bool reached_bp = false;
  if (!should_stall) {
    if (pc_ >= instructions_.size()) {
      pipeline_[0] = -1;
    } else {
      pipeline_[0] = pc_;
    }
    ++pc_;
    pipeline_[1] = old_IF;
    size_t curr_ID = pipeline_[1];
    if (curr_ID != -1) {
      auto &ID_inst = instructions_[curr_ID];
      if (ID_inst.is_breakpoint_) {
        reached_bp = true;
        std::cout << "!!! ID-Stage: Reached at breakpoint [" << curr_ID << '\t'
                  << instruction_text_[curr_ID] << "] !!!\n";
      }
      if ((ID_inst.instruction_op_ == InstructionOp::BEQZ &&
           register_[ID_inst.rd_] == 0) or
          (instructions_[curr_ID].instruction_op_ == InstructionOp::BNEZ &&
           register_[ID_inst.rd_] != 0)) {
        pipeline_[0] = -1;
        pc_ = ID_inst.rs_or_label_;
        ++control_stalls_;
      }
    }
  } else {
    ++raw_stalls_;
    pipeline_[1] = -1;
  }
  ++cycle_clocks_;
  if (IsFinished()) {
    std::cout << "Instructions execution finished! " << cycle_clocks_
              << " cycle clocks executed!\n";
  }
  return reached_bp;
}
void Simulator::RunToStop() {
  if (IsFinished()) {
    std::cerr << "!!!All the instructions has been executed!!!\n";
    return;
  }
  while (!IsFinished()) {
    if (SingleCycle()) {
      break;
    }
  }
}