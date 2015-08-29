/**
 * Simulates an ultra minimalistic 16-bit computer, specifically a Minimal
 * Instruction Set Computer (MISC).
 *
 * Created by: Benjamin M. Singleton
 * Created: 08-26-2015
 * Modified: 08-27-2015
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
const unsigned int LOAD_WORD = 11;
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
const unsigned int RET = 12;
const unsigned int NOP = 0;

class miscCore
{
public:
	miscCore();
	unsigned int getRegister(unsigned int operand);
	void setRegister(unsigned int operand, unsigned int value);
	void clockPulse();
	void reset();
	unsigned int* getRRegisters();
	unsigned int* getSRegisters();
	unsigned int* getTRegisters();
	void setRRegisters(unsigned int* i);
	void setSRegisters(unsigned int* i);
	void setTRegisters(unsigned int* i);
	unsigned int* getMemory();
	void setMemory(unsigned int* i);
private:
	unsigned int* r;
	unsigned int* s;
	unsigned int* t;
	unsigned int instruction;
	unsigned int stackFirst;
	unsigned int stackLast;
	unsigned int flags;
	unsigned int* memory;
};

miscCore::miscCore()
{
	r = new unsigned int[16];
	s = new unsigned int[16];
	t = new unsigned int[16];
	instruction = 0;
	flags = 0;
	stackFirst = 0;
	stackLast = 0;

	for (int i = 0; i < 8; i++)
	{
		r[i] = 0;
		s[i] = 0;
		t[i] = 0;
	}

	memory = new unsigned int[MEMORY_BYTES];
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
unsigned int miscCore::getRegister(unsigned int operand)
{
	unsigned int ret = 0;
	if ((operand & (3 << 4)) == 0)
	{
		// we're looking for an R register
		//cout << "Retrieving $r" << (operand & 15) << endl;
		ret = r[operand & 15];
	}else if ((operand & (3 << 4)) == (1 << 4))
	{
		// we're looking for an S register
		//cout << "Retrieving $s" << (operand & 15) << endl;
		ret = s[operand & 15];
	}else if ((operand & (3 << 4)) == (2 <<4 ))
	{
		// we're looking for a T register
		//cout << "Retrieving $t" << (operand & 15) << endl;
		ret = t[operand & 15];
	}else
	{
		// unsupported
		cout << "Unsupported operand " << operand << endl;
	}

	return ret;
}

void miscCore::setRegister(unsigned int operand, unsigned int value)
{
	if ((operand & (3 << 4)) == 0)
	{
		// we're setting an R register
		//cout << "Storing " << value << " in $r" << (operand & 15) << endl;
		r[operand & 15] = value;
	}else if ((operand & (3 << 4)) == (1 << 4))
	{
		// we're setting an S register
		//cout << "Storing " << value << " in $s" << (operand & 15) << endl;
		s[operand & 15] = value;
	}else if ((operand & (3 << 4)) == (2 <<4 ))
	{
		// we're setting a T register
		//cout << "Storing " << value << " in $t" << (operand & 15) << endl;
		t[operand & 15] = value;
	}else
	{
		// unsupported
		cout << "Cannot set unsupported operand " << operand << endl;
	}
}

/**
 * Simulates a clock pulse, causing the core to execute the
 * instruction at instruction pointer.
 */
void miscCore::clockPulse()
{
	unsigned int ins[2];
	ins[0] = memory[instruction];
	ins[1] = memory[instruction+1];
	unsigned int opcode = ins[0] & (15 << 12);	// ignore the top 4 bits
	opcode = opcode >> 12;
	unsigned int operand1 = ins[0] & (63 << 6);	// want next 6 bits
	operand1 = operand1 >> 6;
	unsigned int operand2 = ins[0] & 63;		// last 6 bits are operand2
	unsigned int operand3 = ins[1];

	unsigned int op1 = getRegister(operand1);
	unsigned int op2 = getRegister(operand2);
	unsigned int op3 = getRegister(operand3 >> 10);

	cout << "Received clock pulse." << endl;
	cout << "At memory address " << instruction << endl;
	//cout << "Opcode operand1 operand2 operand3:" << endl;
	//cout << opcode << " " << operand1 << " " << operand2 << " " << operand3 << endl;
	//cout << "Opcode op1 op2 op3:" << endl;
	//cout << opcode << " " << op1 << " " << op2 << " " << op3 << endl << endl;

	switch(opcode)
	{
		case LOAD_WORD:
			setRegister(operand1, memory[op2]);
			break;
		case STORE_WORD:
			memory[op2] = op1;
			break;
		case NAND_REG:
			setRegister(operand1, ~(op2 & op3));
			break;
		case NAND_CONST:
			// treat operand3 as a constant, not a register
			setRegister(operand1, ~(op2 & operand3));
			break;
		case NOT_REG:
			//cout << "NOT_REG" << endl;
			setRegister(operand1, ~op2);
			break;
		case NOT_CONST:
			// treat operand3 as a constant, not a register
			//cout << "NOT_CONST" << endl;
			setRegister(operand1, ~operand3);
			break;
		case BRANCH_EQUAL:
			if (op1 == op2)
			{
				memory[stackLast] = instruction;
				stackLast++;
				instruction = operand3;
			}
			break;
		case BRANCH_NOT_EQUAL:
			if (op1 != op2)
			{
				memory[stackLast] = instruction;
				stackLast++;
				instruction = operand3;
			}
			break;
		case BRANCH_LESS:
			if (op1 < op2)
			{
				memory[stackLast] = instruction;
				stackLast++;
				instruction = operand3;
			}
			break;
		case BRANCH_GREATER:
			if (op1 > op2)
			{
				memory[stackLast] = instruction;
				stackLast++;
				instruction = operand3;
			}
			break;
		case JUMP:
			// don't store our current address, just jump blindly
			instruction = operand3;
			break;
		case RET:
			instruction = memory[stackLast];
			if (stackLast > stackFirst)
			{
				stackLast--;
			}
			break;
		case NOP:
			break;
		default:
			break;
	}

	instruction += 2;
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

unsigned int* miscCore::getRRegisters()
{
	return r;
}

unsigned int* miscCore::getSRegisters()
{
	return s;
}

unsigned int* miscCore::getTRegisters()
{
	return t;
}

void miscCore::setRRegisters(unsigned int* c)
{
	for (int i = 0; i < 16; i++)
	{
		r[i] = c[i];
	}
}

void miscCore::setSRegisters(unsigned int* c)
{
	for (int i = 0; i < 16; i++)
	{
		s[i] = c[i];
	}
}

void miscCore::setTRegisters(unsigned int* c)
{
	for (int i = 0; i < 16; i++)
	{
		t[i] = c[i];
	}
}

unsigned int* miscCore::getMemory()
{
	return memory;
}

void miscCore::setMemory(unsigned int* c)
{
	for (int i = 0; i < MEMORY_BYTES; i++)
	{
		memory[i] = c[i];
	}
}

unsigned int* loadMachineCodeFile(char* fileName)
{
	ifstream infile;
	infile.open(fileName, ios::binary);
	if (!infile)
	{
		cerr << "Could not open file " << fileName << endl;
		return 0;
	}

	unsigned int* ret;

	streampos size = infile.tellg();
	ret = new unsigned int[size];
	infile.seekg(0, ios::beg);
	infile.read(ret, size);
	infile.close();

	return ret;
}

int main(int argc, char* argv[])
{
	// very simple test case
	unsigned int* m = new unsigned int[MEMORY_BYTES];
	for (int i = 0; i < MEMORY_BYTES; i++)
	{
		m[i] = 0;
	}

	// store 32 in $r0 by doing $r0 = ~(~32)
	m[0] = NOT_CONST << 12;	// NOT opcode
	m[0] |= 0;		// r0 destination
	m[1] = 35;		// const 32

	m[2] = NOT_REG << 12;	// NOT opcode
	m[2] |= 0;		// r0 destination
	m[2] |= 0;		// r0 operand2
	m[3] = 0;		// unused

	// store 8 in $s0 by doing $s0 = ~(~32)
	m[4] = NOT_CONST << 12;	// NOT opcode
	m[4] |= (1 << 4) << 6;	// s0 destination
	m[5] = 3;		// const 3

	m[6] = NOT_REG << 12;	// NOT opcode
	m[6] |= (1 << 4) << 6;	// s0 destination
	m[6] |= (1 << 4);	// s0 operand2
	m[7] = 0;		// unused

	// now nand the two together, into s1
	m[8] = NAND_REG << 12;	// NAND opcode
	m[8] |= 17 << 6;	// s1 destination
	m[8] |= 0;		// r0 operand2
	m[9] = 1 << 14;		// s0 operand3

	m[10] = NOT_REG << 12;	// NOT opcode
	m[10] |= 17 << 6;	// s1 destination
	m[10] |= 17;		// s1 operand2
	m[11] = 0;		// unused

	// this whole thing should store the result of 3 in $s1

	miscCore* myCore = new miscCore();
	myCore->setMemory(m);
	for (int i = 0; i < 6; i++)
	{
		if (i > 0)
		{
			cout << endl;
		}
		cout << "Clock cycle " << i << endl;
		myCore->clockPulse();
		unsigned int* r = myCore->getRRegisters();
		unsigned int* s = myCore->getSRegisters();
		for (int j = 0; j < 16; j++)
		{
			cout << "$r" << j << " = " << r[j] << endl;
		}
		for (int j = 0; j < 16; j++)
		{
			cout << "$s" << j << " = " << s[j] << endl;
		}
	}

	return 0;
}

