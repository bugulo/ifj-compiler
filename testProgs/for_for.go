package main

func main() {
	height := 20
	for i := 0; i < height; i = i + 2 {
		for j := 0; j < (height - i) / 2; j = j + 1 {
			print(" ")
		} 
		for i := 10 - i; i < 10; i = i + 1 {
			print("*")
		}
		print("\n")
	}
}