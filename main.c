#define MEMORY_MAX (1 << 16)
#include <stdint.h> // C

uint16_t memory[MEMORY_MAX]; // unsigned integer of 16 bits wide


// Just enums, used to index the registers
enum // integer constants, auto increments enum values
{
    R_R0 = 0, // 8 gen purpose registers
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, // program counter
    R_COND, // conditional flag
    R_COUNT // count register. Just size not actual register
};

uint16_t reg[R_COUNT];

enum
{
    OP_BR = 0,  // branch
    OP_ADD, // add
    OP_LD, // load
    OP_ST, // store
    OP_JSR, // jump register
    OP_AND, // bitwise and
    OP_LDR, // load register
    OP_STR, // store register
    OP_RTI, // unused
    OP_NOT, // bitwise not
    OP_LDI, // load indirect
    OP_STI, // store indirect
    OP_JMP, // jump
    OP_RES, // reserved (unused)
    OP_LEA, // load effective address
    OP_TRAP // execute trap
};

enum // conditional flags
{
    FL_POS = 1 << 0, // P or 1
    FL_ZRO = 1 << 1, // Z or 2
    FL_NEG = 1 << 2, // N or 4
};

int main(int argc, const char* argv[]){
    /*
    if I did: 
        ./lc3 prog1.obj prog2.obj
    C gets:
        argc = 3
        argv[0] = "./lc3"
        argv[1] = "prog1.obj"
        argv[2] = "prog2.obj"

        if argc < 2 then there aren't any image files.
    */
    // @{load Arguments} // host-side logic here.
    if (argc < 2){
        /* Show usage string*/
        printf("lc3 [image-file1] ...\n");
        exit(2); // 2 = incorrect usage
        // in general, exit means to exit program and return msg to OS
    }
    for(int j = 1; j < argc; ++j){ // start at 1 since 0 is just program name.
        // ++j means to increment then use. There's no difference in a for loop?
        if (!read_image(argv[j])){
            printf("failed to load imageL %s\n", argv[j]);
            exit(1); // 1 = runtime failure
        }
    }
    @{Setup}

    /* Since exactly one condition flag should 
    be set at any given time, set the z flag */
    reg[R_COND] = FL_ZRO; // must always reflect the result of the
    // last operation. So the last result was zero @ the start. 

    // Set the PC to starting position
    // 0x3000 is the default
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running = 1;
    while(running){
        // FETCH
        uint16_t instr = mem_read(reg[R_PC]++); // the whole instruction might encode registers, but the opcode is just the last 4, so it increments by 12 to leave just 4 bits
        uint16_t op = instr >> 12; // other bits tell you registers, flags, offsets, etc.

        switch (op){ // decode tree
            case OP_ADD:
                @{ADD}
                break;
            case OP_AND:
                @{AND}
                break;
            case OP_NOT:
                @{NOT}
                break;
            case OP_JMP:
                @{JMP}
                break;
            case OP_JSR:
                @{JSR}
                break;
            case OP_LD:
                @{LD}
                break;
            case OP_LDI:
                @{LD}
                break;
            case OP_LDR:
                @{LDR}
                break;
            case OP_LEA:
                @{LEA}
                break;
            case OP_STI:
                @{STI}
                break;
            case OP_STR:
                @{STR}
                break;
            case OP_TRAP:
                @{TRAP}
                break;
            case OP_RES:
            case OP_RTI:
            default:
                @{BAD OPCODE}
                break;
            
        }
    }
    @{Shutdown}
}

void update_flags(uint16_t r){
    if (reg[r] == 0){ // if the register of r is 0, then 0
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15){  // if a 1 in leftmost bit
        reg[R_COND] = FL_NEG; // I think because that must mean signed
    }
    else
    {
        reg[R_COND] = FL_POS; // anything else must be pos
    }
}

// just pads a negative shorter number with 1's, and 0 for positive.
uint16_t sign_extend(uint16_t x, int bit_count)
{
    // asks if sign bit is negative or positive
    if ((x >> (bit_count - 1)) & 1) { // and isolates bit because AND returns bit value. x >> index of sign bit
        x |= (0xFFFF << bit_count); // if negative fills upper bits with 1's
        /*
        uint16 already adds 0's/ 
        0xFFFF is 16 1's, and then shifts to the left by the bit count 
        to make room for the base number. 
        
        |= is a mask, and just merges them, basically applying an or to each pos.
        */
    }
    return x;
}