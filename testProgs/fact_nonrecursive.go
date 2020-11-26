package main

func main() {
	print("Cislo pre vypocet faktorialu: 4\n")
	a := 0
	a = 4
	if a < 0 {
		print("Faktorial nejde spocitat!\n")
	} else {
		vysl := 1
		for ; a > 0; a = a - 1 {
			vysl = vysl * a
		}
		print("Vysledok je ", vysl, "\n")
	}
}