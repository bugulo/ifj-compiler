package main

func main() {
	a := 10
	for i := a; i > 0; i = i - 1 {
		test(i)
	}
}

func test(cislo int) {
	if cislo > 5 {
		print(cislo)
	} else {
		print("0", cislo)
	}
	print("\n")
}