#include <cstddef>
#include <iostream>
#include <set>
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
  LOAD,     // lw
  STORE,    // sw
  ADDI,     // addi
  SUBI,     // subi
  ADD,      // add
  SUB,      // sub
  BEQZ,     // beqz
  BNEZ,     // bnez
};

class Instruction {
public:
  InstructionOp instruction_op_;
  size_t rd_;
  size_t rs_or_label_;
  int rt_or_imm_;
  inline Instruction(InstructionOp instruction_op, size_t rd, size_t rs,
                     int rt_or_imm)
      : instruction_op_(instruction_op), rd_(rd), rs_or_label_(rs), rt_or_imm_(rt_or_imm){}
};

class Simulator {
private:
  std::vector<long> memory_;
  std::vector<Register> register_;
  std::vector<Instruction> instruction_;
  std::set<size_t> breakpoints_;
  std::vector<size_t> pipeline_;

public:
  Simulator();
  Simulator(std::vector<long> init_memory,
            std::vector<Instruction> instruction);
  ~Simulator() = default;
};