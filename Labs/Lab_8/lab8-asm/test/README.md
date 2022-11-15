# Lab 8: Martin Šmelka

### Instruction set

1. Complete the conversion table with selected instructions:

   | **Instruction** | **Binary opcode** | **Hex opcode** | **Compiler Hex opcode** |
   | :-- | :-: | :-- | :-: |
   | `add r24, r0` | `0000_1100_1111_0000` | CF0 |  |
   | `com r26` |  |  |  |
   | `eor r26, r27` |  |  |  |
   | `mul r22, r20` | `1001_1100_1011_1010` | 9CBA |  |
   | `ret` | `1001_0101_0000_1000` | 9508 | 0895 |

### 4-bit LFSR

2. Complete table with 4-bit LFSR values for different Tap positions:

   | **Tap position** | **Generated values** | **Length** |
   | :-: | :-- | :-: |
   | 4, 3 |  | 15 |
   | 4, 2 |  | 6 |
   | 4, 1 |  | 15 |

### Variable number of short pulses

3. Draw a flowchart of function `void burst_c(uint8_t number)` which generates a variable number of short pulses at output pin. Let the pulse width be the shortest one. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

   ![your figure]()