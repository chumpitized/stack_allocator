#include <assert.h>
#include <cstring>
#include <iostream>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT sizeof(void *)
#endif

typedef struct Arena Arena;
struct Arena {
	unsigned char* buffer;
	size_t length;
	size_t curr_offset;
	size_t prev_offset;
};

typedef struct Arena_Header Arena_Header;
struct Arena_Header {
	size_t prev_offset;
};

void init_arena(Arena *a, void *buffer, size_t length) {
	a->buffer = (unsigned char *)buffer;
	a->length = length;
	a->curr_offset = 0;
	a->prev_offset = 0;
}

bool is_power_of_two(size_t x) {
	return (x & (x - 1)) == 0;
}

uintptr_t align_with_header(uintptr_t ptr, size_t align) {
	assert(is_power_of_two(align));

	uintptr_t p, a, modulo;

	p = ptr + sizeof(Arena_Header);
	a = (uintptr_t)align;
	modulo = p & (a - 1);

	if (modulo != 0) {
		p += a - modulo;
	}

	return p;
}

void *arena_push(Arena *a, size_t size, size_t align) {
	Arena_Header* header;

	uintptr_t curr_ptr 			= (uintptr_t)a->buffer + (uintptr_t)a->curr_offset;
	uintptr_t offset 			= align_with_header(curr_ptr, align);
	uintptr_t relative_offset  	= offset - (uintptr_t)a->buffer;

	if (relative_offset + size <= a->length) {
		header = (Arena_Header *)(offset - sizeof(Arena_Header));
		header->prev_offset = a->prev_offset;

		void *ptr = &a->buffer[relative_offset];
		a->prev_offset = relative_offset - sizeof(Arena_Header);
		a->curr_offset = relative_offset + size;

		memset(ptr, 0, size);
		return ptr;
	}

	return NULL;
}

void arena_pop(Arena *a) {
	size_t clear_size = a->curr_offset - a->prev_offset;

	Arena_Header *header = (Arena_Header *)(uintptr_t)a->buffer + (uintptr_t)a->prev_offset;

	a->curr_offset = a->prev_offset;
	a->prev_offset = header->prev_offset;

	memset(header, 0, clear_size);
}

int main() {
	
	unsigned char buffer[256];
	Arena a = {0};
	init_arena(&a, buffer, 256);
	int *five = (int *)arena_push(&a, sizeof(int), DEFAULT_ALIGNMENT);
	*five = 5;

	//is 5
	std::cout << *five << std::endl;

	arena_pop(&a);

	//is 0
	std::cout << *five << std::endl;

	return 1;
}