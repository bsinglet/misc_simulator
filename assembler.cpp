/**
 * Assembles an assembly source file into machine code for my 16-bit MISC
 * computer.
 *
 * Created by: Benjamin M. Singleton
 * Created: 08-28-2015
 * Modified:
 */

#include <iostream>
#include <utility>
#include "misc_simulator.cpp"

using namespace std;

/**
 * Splits an input string into a vector of words, based on spaces.
 */
vector<string>* splitIntoWords(string s)
{
	vector<string>* ret = new vector<string>();
	while (s.length() > 0)
	{
		if (s.find(" ") != string::npos)
		{
			// trim leading whitespace
			if (s.at(0) == ' ')
			{
				s.erase(0, 1);
				continue;
			}
			// trim trailing whitespace
			if (s.back() == ' ')
			{
				s.erase(s.length()-1, 1);
				continue;
			}
			string newWord = s.substr(0, s.find(" "));
			s.erase(0, s.find(" ")+1);
			ret->push_back(newWord);
		}else
		{
			ret->push_back(s);
			break;
		}
	}

	return ret;
}

/**
 * Given a string specifying a register, returns the code for it. There are
 * 3 sets of 16 registers, $r0-$r15, $s0-$s15, and $t0-$t15.
 */
unsigned int getRegisterNumber(string s)
{
	unsigned int ret = 0;
	if (s.length() < 3 || (s.length >= 3 && s[0] != "$")
	{
		cerr << "Error: register name has to be at least 3";
		cerr << " characters and start with $." << endl;
		cerr << s << endl;
	}

	s = s.substr(1, s.length()-1); // remove the initial $
	switch(s[0])
	{
		case 'r':
			ret = 0;
			break;
		case 's':
			ret = 16;
			break;
		case 't':
			ret = 32;
			break;
		default:
			cerr << "Invalid register name " << s << endl;
			return 0;
	}

	unsigned int regNumber = atoi(s.substr(1, s.length()-1).c_str());
	if (regNumber < 0 || regNumber > 15)
	{
		cerr << "Invalid register number " << regNumber << " in " << s << endl;
		return 0;
	}
	ret |= regNumber;

	return ret;
}

/**
 * Takes a line of assembly, and returns the 2 bytes of machine code for it.
 */
unsigned int* instructionToMachineCode(string s)
{
	string operation, operand1, operand2, operand3;
	unsigned int* ret;
	ret = new unsigned int[2];
	// transform the assembly code to lowercase
	std::transform(s.begin(), s.end(), s.begin(), std::tolower());
	vector<string>* v = splitIntoWords(s);
	if (v->size() == 0)
	{
		cerr << "Error, no words in line " << s << endl;
	}
	if (v->size() > 1)
	{
		operand1 = v->at(1);
		// remove the comma if there is one
		if (operand1.find(",") != string::npos)
		{
			operand1 = operand1.substr(0, operand1.find(","));
		}
	}
	if (v->size() > 2)
	{
		operand2 = v->at(2);
		// remove the comma if there is one
		if (operand2.find(",") != string::npos)
		{
			operand2 = operand2.substr(0, operand2.find(","));
		}
	}
	if (v->size() > 3)
	{
		operand3 = v->at(2);
	}

	if (v->size() == 1)
	{
		if (operation == "nop")
		{
			ret[0] = NOP << 12;
		}else if (operation == "ret")
		{
			ret[0] = RET << 12;
		}else
		{
			cerr << "Error: line of assembly with no arguments that isn't NOP or RET." << endl;
			cerr << s << endl;
			return 0;
		}
	}else if (v->size() == 2)
	{
		if (operation == "lw")
		{
			ret[0] = LOAD_WORD << 12;
			ret[0] |= getRegisterNumber(operand1) << 6;
			ret[0] |= getRegisterNumber(operand2);
			ret[1] = 0;
		}else if (operation == "sw")
		{
			ret[0] = STORE_WORD << 12;
			ret[0] |= getRegisterNumber(operand1) << 6;
			ret[0] |= getRegisterNumber(operand2);
			ret[1] = 0;

		}else if (operation == "jmp")
		{
			// figure out where to jump using operand1
		}else
		{
			cerr << "Error: line of assembly with one argument";
			cerr << " that isn't LW, SW, or JMP." << endl;
			cerr << s << endl;
			return 0;
		}
	}else if (v->size() == 3)
	{
		if (operation == "not")
		{
			// figure out if it's NOT_REG or NOT_CONST
		}else
		{
			cerr << "Error: line of assembly with 2 arguments that isn't NOT." << endl;
			cerr << s << endl;
			return 0;
		}
	}else if (v->size() == 4)
	{
		if (operation == "nand")
		{
			// figure out if it's NAND_REG or NAND_CONST
		}else if (operation == "beq")
		{
			ret[0] = BRANCH_EQUAL << 12;
		}else if (operation == "bne")
		{
			ret[0] = BRANCH_NOT_EQUAL << 12;
		}else if (operation == "blt")
		{
			ret[0] = BRANCH_LESS << 12;
		}else if (operation == "bgt")
		{
			ret[0] = BRANCH_GREATER << 12;
		}else
		{
			cerr << "Error: line of assembly with three arguments";
			cerr << " that isn't NAND, BEQ, BNE, BLT, or BGT." << endl;
			cerr << s << endl;
			return 0;
		}
	}else
	{
		cerr << "Error: line of assembly with more than 3 operands." << endl;
		cerr << s << endl;
		return 0;
	}

	return ret;
}

int main(int argc, char* argv[])
{

	return 0;
}
