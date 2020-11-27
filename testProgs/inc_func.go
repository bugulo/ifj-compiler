package main

func main() {
	a := "-"
	desat := 10
	for i := 1; i <= desat; {
		for j := 1; j <= 5; {
			if i <= 5 {
				print(a)
			} else {
				print("+")
			}
			j = INC(j)
		}
		i = INC(i)
		print("\n")
	}
}

func INC(i int) (int){
	return i + 1
}