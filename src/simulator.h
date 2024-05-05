#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using Register = long long;

// lw   rd,rs,imm
// sw   rd,rs,imm
// addi rd,rs,imm
// subi rd,rs,imm
// add  rd,rs,rt
// sub  rd,rs,rt
// beqz rd,label
// bnez rd,label

enum class InstructionOp {
  LOAD,  // lw
  STORE, // sw
  ADDI,  // addi
  SUBI,  // subi
  ADD,   // add
  SUB,   // sub
  BEQZ,  // beqz
  BNEZ,  // bnez
};

enum class PipeLine {
  IF = 0,
  ID = 1,
  EX = 2,
  MEM = 3,
  WB = 4,
};

const std::vector<std::string> pipeline_name = {"IF", "ID", "EX", "MEM", "WB"};

class Instruction {
public:
  InstructionOp instruction_op_;
  size_t rd_;
  size_t rs_or_label_;
  int rt_or_imm_;
  bool is_breakpoint_;
  bool is_finished_;
  Instruction() = delete;
  inline Instruction(InstructionOp instruction_op, size_t rd, size_t rs,
                     int rt_or_imm, bool is_breakpoint = false,
                     bool is_finished = false)
      : instruction_op_(instruction_op), rd_(rd), rs_or_label_(rs),
        rt_or_imm_(rt_or_imm), is_breakpoint_(is_breakpoint),
        is_finished_(is_finished) {}
  ~Instruction() = default;
};

class Simulator {
private:
  std::vector<long> memory_;
  std::vector<Register> register_;
  std::vector<Instruction> instructions_;
  std::vector<std::string> instruction_text_;
  std::set<size_t> breakpoints_;
  std::vector<size_t> pipeline_;
  size_t pc_{0};
  size_t cycle_clocks_{0};
  size_t raw_stalls_{0};
  size_t control_stalls_{0};
  bool enable_forwarding_{false};

public:
  Simulator();
  Simulator(std::vector<long> init_memory,
            std::vector<Instruction> instructions,
            std::vector<std::string> instruction_text,
            bool enable_forwarding = false);
  ~Simulator() = default;

  bool IsFinished() const;
  void PrintInstructions() const;
  void PrintPipelines() const;
  void PrintRegisters() const;
  void PrintBreakpoints() const;
  void PrintMemory() const;
  void PrintStatistics() const;

  void SetBreakpoint(size_t instruction_index);
  bool SingleCycle();
  void RunToStop();

  static inline void PrintUsage() {
    std::cout << "Usage: \n"
              << "v [i | p | r | b | m | s]: "
                 "display instructions | pipelines | registers | breakpoints | "
                 "memory | statistics\n"
              << "b [instruction index] : set breakpoint at instruction of the "
                 "index\n"
              << "s : single cycles\n"
              << "r : run to the end or breakpoint\n"
              << "q : quit the simulator\n";
  }

  static inline bool HasHazard(Instruction &new_inst, Instruction &old_inst) {
    switch (new_inst.instruction_op_) {
    case InstructionOp::LOAD:
    case InstructionOp::ADDI:
    case InstructionOp::SUBI:
      return !IsBrachInst(old_inst) && !IsStoreInst(old_inst) &&
             new_inst.rs_or_label_ == old_inst.rd_;
      break;
    case InstructionOp::ADD:
    case InstructionOp::SUB:
      return !IsBrachInst(old_inst) && !IsStoreInst(old_inst) &&
             (new_inst.rs_or_label_ == old_inst.rd_ ||
              new_inst.rt_or_imm_ == old_inst.rd_);
      break;
    case InstructionOp::STORE:
    case InstructionOp::BEQZ:
    case InstructionOp::BNEZ:
      return !IsBrachInst(old_inst) && !IsStoreInst(old_inst) &&
             (new_inst.rd_ == old_inst.rd_ || new_inst.rd_ == old_inst.rd_);
      break;
    }
  }

  static inline bool IsBrachInst(Instruction &inst) {
    return inst.instruction_op_ == InstructionOp::BEQZ or
           inst.instruction_op_ == InstructionOp::BNEZ;
  }
  static inline bool IsStoreInst(Instruction &inst) {
    return inst.instruction_op_ == InstructionOp::STORE;
  }
};