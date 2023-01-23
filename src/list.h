#pragma once

#define LINKED_LIST(type, name) \
struct name { \
	type data; \
	struct name* next; \
};
