#include "simulator.h"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "You did not provide MIPS file.\nYou should directly input "
                 "your MIPS code:\n";
  }
  std::ifstream fin(argv[1]);
  std::unordered_map<std::string, size_t> symbol_to_memory;
  std::unordered_map<std::string, size_t> label_to_inst_idx;
  std::vector<long> init_memory;
  std::vector<Instruction> instructions;
  std::vector<std::string> instruction_text;

  size_t memo_top = 0;
  std::string curr_tok;
  fin >> curr_tok;

  if (curr_tok == ".data") {
    std::cout << curr_tok << '\n';
    while ((fin >> curr_tok) && curr_tok != ".text") {
      curr_tok.pop_back();
      symbol_to_memory[curr_tok] = memo_top;
      long val;
      fin >> val;
      std::cout << curr_tok << " " << val << '\n';
      init_memory.emplace_back(val);
      memo_top++;
    }
  }
  size_t inst_num = 0;
  while ((fin >> curr_tok)) {
    if (curr_tok.back() == ':') {
      curr_tok.pop_back();
      label_to_inst_idx[curr_tok] = inst_num;
      std::cout << curr_tok << " : Label\n";
    } else {
      std::string opname = std::move(curr_tok);
      std::string optors;
      fin >> optors;
      std::string inst_text = opname + '\t' + optors;
      instruction_text.push_back(inst_text);
      std::string::size_type first_comma_idx = optors.find(',');
      std::string optor1 = optors.substr(0, first_comma_idx);
      std::string::size_type second_comma_idx =
          optors.find(',', first_comma_idx + 1);
      std::string optor2, optor3;
      if (second_comma_idx == std::string::npos) {
        optor2 = optors.substr(first_comma_idx + 1);
      } else {
        optor2 = optors.substr(first_comma_idx + 1,
                               second_comma_idx - first_comma_idx - 1);
        optor3 = optors.substr(second_comma_idx + 1);
      }

      std::cout << inst_text << '\n';
      if (opname == "lw" or opname == "sw") {
        size_t rd = std::stoi(optor1.substr(1));
        size_t rs = std::stoi(optor2.substr(1));
        size_t imm = symbol_to_memory[optor3];
        instructions.emplace_back(opname == "lw" ? InstructionOp::LOAD
                                                 : InstructionOp::STORE,
                                  rd, rs, imm);
      } else if (opname == "addi" or opname == "subi") {
        size_t rd = std::stoi(optor1.substr(1));
        size_t rs = std::stoi(optor2.substr(1));
        size_t imm = std::stoi(optor3);
        instructions.emplace_back(opname == "addi" ? InstructionOp::ADDI
                                                   : InstructionOp::SUBI,
                                  rd, rs, imm);
      } else if (opname == "add" or opname == "sub") {
        size_t rd = std::stoi(optor1.substr(1));
        size_t rs = std::stoi(optor2.substr(1));
        size_t rt = std::stoi(optor3.substr(1));
        instructions.emplace_back(opname == "add" ? InstructionOp::ADD
                                                  : InstructionOp::SUB,
                                  rd, rs, rt);
      } else if (opname == "beqz" or opname == "bnez") {
        size_t rd = std::stoi(optor1.substr(1));
        size_t label_inst_idx = label_to_inst_idx[optor2];
        // BUG TODO:
        // the label may not appear yet
        instructions.emplace_back(opname == "beqz" ? InstructionOp::BEQZ
                                                  : InstructionOp::BNEZ,
                                  rd, label_inst_idx, -1);
      }
      inst_num++;
    }
  }

  Simulator mysim;
  return 0;
}