package main

func main() {
	height := 0
	weight := 0
	height, weight = 5 + 10, 6 / 3
	print(height, "\n")
	print(weight, "\n")

	height, weight = test(40)
	print(height, "\n")
	print(weight, "\n")
	for i := 0; i < height; i = i + 2 {
		for j := 0; j < (height - i) / 2; j = j + 1 {
			def := 0
			hoho := 0
			def, hoho = test(i)
			print(def, " ", hoho, "\n")
		} 
	}
}

func test(a int) (int, int){
	return a/2, a *2
}

func test2(a int) (int){
	return 1
}