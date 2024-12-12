Overview

This C++ MIPS simulator executes a limited set of MIPS assembly instructions, simulating a CPU and memory while maintaining register and memory states. It supports R-type, I-type, and J-type instructions, displaying results after each step.

Features

Supported Instructions:

R-type: add, sub, and, or, slt

I-type: addi, beq, lw, sw

J-type: j

Registers: Simulates 32 registers ($zero to $ra).

Memory: Interacts with data memory using lw/sw.

Input: Reads machine code from machine_code.txt.

Usage

Prepare the machine_code.txt file with binary MIPS instructions.

Compile:

g++ -o mips_simulator mips_simulator.cpp

Run:

./mips_simulator

View the output, which includes register and memory states after each instruction.

Example

Input (machine_code.txt):

10001100000010000000000000000000  # lw $t0, num1
10001100000010010000000000000100  # lw $t1, num2
00000001000010010101000000100000  # add $t2, $t0, $t1
00000001001010000101100000100010  # sub $t3, $t1, $t0

Output:

The program prints the executed instruction, register values, and memory contents.

Customization

Base Address: Modify baseAddress for memory starting address.

Instruction Set: Extend by adding new opcodes.

Limitations

Supports a subset of MIPS instructions.

Requires binary-encoded input.

Motivation

Created as a learning tool for MIPS architecture and assembly execution.