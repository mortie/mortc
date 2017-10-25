import memory.*

export Vector init end set

struct 'a Vector {
	int size
	int len
	int arrptr
}

init('a Vector vec) {
	vec.size = 0
	vec.len = 0
	vec.arrptr = 0
}

end('a Vector vec) {
	free vec.arr
	vec.size = 0
	vec.len = 0
	vec.arrptr = 0
}

set('a Vector vec, int i, 'a elem) {
	if i >= vec.len {
		vec.len = + 1 i
	}

	if i >= vec.size {
		do while vec.size <= i {
			vec.size = * 2 vec.size
		}
		vec.arrptr = realloc vec.arrptr vec.size
	}

	vec.arr[i] = elem
}

get('a Vector vec, int i): 'a Maybe {
	if 
}

push('a Vector vec, 'a elem) {
	set vec vec.len elem
	vec.len += 1
}

pop('a Vector vec): 'a Maybe {
}
