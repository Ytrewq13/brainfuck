#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <assert.h>

#define DATA_SIZE 65536;
#define SOURCE_FILE "bf.txt"


typedef unsigned uint;
typedef unsigned char uchr;

void arrow(int dir, uchr * memory, uchr ** point, uint mem_size)
{
	/* dir should be -1 or 1.
	 * -1 -> left arrow  "<"
	 * 1 --> right arrow ">"
	 */
	assert(dir == -1 || dir == 1);
	*point += dir;
	// Wraparound if necessary.
	if (*point < memory)
	{
//		printf("pointer: %p\n", point);
		*point = memory + mem_size;
//		printf("new pointer: %p\n", memory + memSize);
//		printf("Wrapping pointer to top...\n");
	} else if (*point > memory + mem_size)
	{
//		printf("pointer: %p\n", point);
		*point = memory;
//		printf("Wrapping pointer to bottom...\n");
	}
}

void add(int amount, uchr * point)
{
	/* amount will be either -1 or 1:
	 * 1 --> plus symbol  "+"
	 * -1 -> minus symbol "-"
	 */
	assert(amount == -1 || amount == 1);
	*point += amount; // Do the increment/decrement.
}

void output(uchr * point)
{
	printf("%c", *point);
}

void input(uchr * point)
{
	*point = getchar();
}

void bracket(int dir, uchr *ptr, uint *pc, uint match_indx)
{
	assert(dir == -1 || dir == 1);
	if (dir == -1)
	{
		// "["
		if (*ptr == 0)
		{
			// Skip to matching "]"
			*pc = match_indx;
		} else {
			// Value isn't zero, enter loop.
		}
	} else {
		// "]"
		if (*ptr != 0)
		{
			// Go back to matching "[".
			*pc = match_indx;
		} else {
			// Continue.
		}
	}
}

uint find_match(int dir, char *program, uint this_index, uint prog_len)
{
	assert((dir == -1 && this_index < prog_len-1) || (dir == 1 && this_index > 0));
	uint i;
	int layers = 0; // Ogres are like onions.
	
	if (dir == -1)
	{
		// "[" - check forwards.
		for (i = this_index+1; i < prog_len; i++)
		{
			assert(layers >-1); // They stink?
			if (program[i] == ']')
			{
				if (layers == 0) // Yes. No.
				{
					return i;
				} else {
					layers--; // Oh, they make you cry.
				}
			} else if (program[i] == '[')
			{
				layers++; // No.
			}
		}
	} else {
		// "]" - check backwards.
		for (i = this_index-1; i > 0; i--)
		{
			assert(layers >-1); // Oh, you leave em out in the sun, they get all brown, start sproutin’ little white hairs.
			if (program[i] == '[')
			{
				if (layers == 0) // No. Layers. Onions have layers. Ogres have layers. Onions have layers. You get it? We both have layers.
				{
					return i;
				} else {
					layers--; // Oh, you both have layers. Oh. You know, not everybody like onions.
				}
			} else if (program[i] == ']')
			{
				layers++; // Layers!
			}
		}
	}
}

void execute(uchr **ptr, uint *pc, char *prog_source, uchr *mem, uint mem_size, uint prog_len)
{
	uint match;
	char oc = prog_source[*pc]; // Opcode
	switch (oc)
	{
		case '+':
			add(1, *ptr);
			break;
		case '-':
			add(-1, *ptr);
			break;
		case '.':
			output(*ptr);
			break;
		case ',':
			input(*ptr);
			break;
		case '<':
			arrow(-1, mem, ptr, mem_size);
			break;
		case '>':
			arrow(1, mem, ptr, mem_size);
			break;
		case '[':
			match = find_match(-1, prog_source, *pc, prog_len);
			bracket(-1, *ptr, pc, match);
			break;
		case ']':
			match = find_match(1, prog_source, *pc, prog_len);
			bracket(1, *ptr, pc, match);
			break;
		default:
			return;
	}
}

uint get_pos(uchr * memory, uchr * point)
{
	return point - memory;
}

uchr get_value(uchr * point)
{
	return * point;
}

int main(int argc, char* argv[]) {
	uint i;
	uint mem_size;
	uchr *data, *p;
	uint pc; // Program counter - index in file string.
	mem_size = DATA_SIZE;
	
	// Load the program into memory.
	//
	FILE *f = fopen(SOURCE_FILE, "r");
	assert(f);
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f); // Size of the file.
	fseek(f, 0, SEEK_SET);

	char *bf_code = malloc(fsize + 1);
	fread(bf_code, fsize, 1, f);
	fclose(f);
	//
	// Finished loading the program.
	
	/* Initialise the memory array.
	 * This is the array to store values in,
	 * accessed by the pointer "p" variable.
	 */
	data = malloc(mem_size * sizeof (data)); // Allocate memory.
	p = data; // Init. pointer to start of memory.
	
	// Execute the code from the file.
	//
	for (pc = 0; pc < fsize; pc++)
	{
		execute(&p, &pc, bf_code, data, mem_size, fsize);
	}
	
	// Free all allocated memory.
	//
	free(bf_code); // "Don't forget to free() the string."
	free(data);
	
	return (EXIT_SUCCESS);
}

// TODO: Extensive testing.
