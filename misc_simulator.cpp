/**
 * Simulates an ultra minimalistic 16-bit computer, specifically a Minimal
 * Instruction Set Computer (MISC).
 *
 * Created by: Benjamin M. Singleton
 * Created: 08-26-2015
 * Modified:
 */

#include <iostream>

using namespace std;

/**
 * need 4 bits for opcode
 * need 6 bits for each operand
 * constants need 16 bits
 * so an instruction takes up at most (4 + 6 + 6 + 16) = 32 bits if we allow
 * constants in instructions with 3 operands.  Could also load all constants
 * through the not instruction, by doing
 *		NOT $r0, 0x20
 *		NOT $r0, $r0 	# r0 now contains the hex 20
 * So, all instructions take up 4 bytes.
*/
const int MEMORY_BYTES = 65536;
const unsigned int LOAD_WORD = 0;
const unsigned int STORE_WORD = 1;
const unsigned int NAND_REG = 2;
const unsigned int NAND_CONST = 3;
const unsigned int NOT_REG = 4;
const unsigned int NOT_CONST = 5;
const unsigned int BRANCH_EQUAL = 6;
const unsigned int BRANCH_NOT_EQUAL = 7;
const unsigned int BRANCH_LESS = 8;
const unsigned int BRANCH_GREATER = 9;
const unsigned int JUMP = 10;
const unsigned int NOP = 11;

class miscCore
{
public:
	miscCore();
	int* miscCore::getRegisterPointer(int operand);
	void clockPulse();
	void reset();
	int** getRRegsiters();
	int** getSRegsiters();
	int** getTRegsiters();
	void setRRegsiters(int** c);
	void setSRegsiters(int** c);
	void setTRegsiters(int** c);
	char* getMemory();
	void setMemory(char* c);
private:
	int* r[16];
	int* s[16];
	int* t[16];
	char instruction;
	char flags;
	char* memory;
};

miscCore::miscCore()
{
	r = new int[16];
	s = new int[16];
	t = new int[16];
	instruction = 0;
	flags = 0;

	for (int i = 0; i < 8; i++)
	{
		r[i] = 0;
		s[i] = 0;
		t[i] = 0;
	}

	memory = new int[MEMORY_BYTES];
	for (int i = 0; i < MEMORY_BYTES; i++)
	{
		memory[i] = 0;
	}
}

/**
 * Operands are 6-bits long, specifying the type of register and its number.
 * Format:	   11 1111
 * First two bits: 00-R, 01-S, 10-T, 11-Unsupported
 * Last four bits: register 0-15
 */
int* miscCore::getRegisterPointer(int operand)
{
	int* ret = 0;
	if ((operand & (3 << 4)) == 0)
	{
		// we're looking for an R register
		ret = r[operand & 15];
	}else if ((operand & (3 << 4)) == (1 << 4))
	{
		// we're looking for an S register
		ret = s[operand & 15];
	}else if ((operand & (3 << 4)) == (2 <<4 ))
	{
		// we're looking for a T register
		ret = t[operand & 15];
	}else
	{
		// unsupported
	}
}

/**
 * Simulates a clock pulse, causing the core to execute the
 * instruction at instruction pointer.
 */
void miscCore::clockPulse()
{
	int* ins[] = new int[2];
	ins[0] = memory[instruction];
	ins[1] = memory[instruction+1];
	opcode = ins[0] & (15 << 12);		// ignore the top 4 bits
	int operand1 = ins[0] & (63 << 6);	// want next 6 bits
	int operand2 = ins[0] & 63;		// last 6 bits are operand2
	int operand3 = ins[1];

	int* op1 = getOperand(operand1);
	int* op2 = getOperand(operand2);
	int* op3 = getOperand(operand3);

	switch(opcode)
	{
		case LOAD_WORD:
			op1 = memory[op2];
			break;
		case STORE_WORD:
			memory[op2] = op1;
			break;
		case NAND_REG:
			op1 = ~(op2 & op3);
			break;
		case NAND_CONST:
			// treat operand3 as a constant, not a register.
			op1 = ~(op2 & operand3);
			break;
		case NOT_REG:
			op1 = ~op2;
			break;
		case NOT_CONST:
			// ?????
			break;
		case BRANCH_EQUAL:
			break;
		case BRANCH_NOT_EQUAL:
			break;
		case BRANCH_LESS:
			break;
		case BRANCH_GREATER:
			break;
		case JUMP:
			break;
		case NOP:
			break;
		default:
			break;
	}
}

void miscCore::reset()
{
	instruction = 0;
	flags = 0;

	for (int i = 0; i < 8; i++)
	{
		r[i] = 0;
		s[i] = 0;
		t[i] = 0;
	}

	for (int i = 0; i < MEMORY_BYTES; i++)
	{
		memory[i] = 0;
	}
}

char** miscCore::getRRegsiters()
{
	return r;
}

char** miscCore::getSRegsiters()
{
	return s;
}

char** miscCore::getTRegsiters()
{
	return t;
}

void miscCore::setRRegsiters(char** c)
{
	for (int i = 0; i < 16; i++)
	{
		r = c[i];
	}
}

void miscCore::setSRegsiters(char** c)
{
	for (int i = 0; i < 16; i++)
	{
		s = c[i];
	}
}

void miscCore::setTRegsiters(char** c)
{
	for (int i = 0; i < 16; i++)
	{
		t = c[i];
	}
}

char* miscCore::getMemory()
{
	return memory;
}

void setMemory(char* c)
{
	for (int i = 0; i < MEMORY_BYTES; i++)
	{
		memory[i] = c[i];
	}
}

int main(int argc, char* argv[])
{

}
