export malloc, realloc, free

malloc(int size): void* {
	c:malloc size
}

realloc(void *ptr, int size): void* {
	c:realloc ptr size
}

free(void *ptr) {
	c:free ptr
}
