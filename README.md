
# Instruction_Reordering_Experiment

## Overview

The Reorder Buffer (ROB) is a critical hardware structure used in modern out-of-order CPUs to support instruction-level parallelism while maintaining correct program behavior. It enables the CPU to execute instructions out of order for performance, but commit (retire) them in program order to preserve precise exceptions and data correctness.

This repository contains an implementation (or simulation/model) of a Reorder Buffer, demonstrating its role in a superscalar, out-of-order CPU pipeline.

## What Does the ROB Do?

- Tracks instructions after they are issued and before they are committed.

- Ensures in-order commitment of instructions even if they executed out-of-order.

- Buffers instruction results until they are ready to be committed to the architectural state (register file/memory).

- Supports precise exceptions by allowing the processor to roll back to a known good state.

### ROB Structure

Typically, each ROB entry contains:

- Instruction ID

- Destination register

- Result value (once ready)

- Status flags (issued, executed, ready, committed)

Exception information (if any)

How It Works in the Pipeline
- Issue: Instructions are decoded and entered into the ROB in program order.

- Execute: Execution may happen out of order, as soon as operands are available.

- Write-back: Results are written to the ROB (not directly to the register file).

- Commit: When the instruction reaches the head of the ROB and is ready, its result is written to the architectural state.


## Dependencies

- **C++20 compiler** (for `std::format` and modern features)
- **Standard Template Library (STL)** (for `std::vector`, `std::chrono`, etc.)
- **Custom headers**:
  - [`kaizen.h`](https://github.com/heinsaar/kaizen) (provides `zen::print`, `zen::color`, `zen::cmd_args`, and other utilities)

## Build Instructions

1. Clone the repository:
   ```
   git clone https://github.com/username/Instruction_Reordering_Experiment
   ```
2. Navigate to the repository:
   ```
   cd Instruction_Reordering_Experiment
   ```
3. Generate build files:
   ```
   cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
   ```
4. Build the project:
   ```
   cmake --build build --config Release
   ```
5. Run the executable from the build directory:
   ```
   ./build/Instruction_Reordering_Experiment
   ```

## Usage

Run the program with optional arguments to customize the experiment parameters:

```
./Instruction_Reordering_Experiment --iterations [num]  as in int
```
- `--iterations`: Number of summation iterations per trial (default: 1,000).

## Example Output

Below is sample output from running the program with ` --iterations 20000`:
```
| Not using ROB time:      |   136400.000 ns|
| Using ROB time:          |    59200.000 ns|
| Speedup Factor:          |          0.4340|
```

