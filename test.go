package main
func main() {
	for i := 0; i < 10; i = i + 1 {
		baf := "kokot"
		for i := 0; i < 10; i = i + 1 {
			baf := i
			print(baf)
		}
		print(baf)
		test(i)
	}
}

func test (a int) () {
	print(a)
}