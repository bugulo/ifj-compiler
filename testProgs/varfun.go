package main

func foo(x int, y int) (int, int) {
	i := x
	j := (y + 2) * 3
	i, j = j + 1, i + 1
	return i, j
}

func main() {
	a := 1
	b := 2
	a, b = foo(a, b)
	if a < b {
		print(a, "<", b, "\n")
		a := 666
		print(a, "\n")
	} else {
		print(a, ">=", b, "\n")
		a = 33
	}
	print(a, "\n")
}
