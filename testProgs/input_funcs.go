package main

func main() {
	meno := ""
	pocet := 0
	print("Zadaj pocet osob: ")
	pocet, _ = inputi()
	for i := 1; i <= pocet; i = i + 1 {
		print("Zadaj meno ", i, ". osoby: ")
		meno, _ = inputs()
		print("Ahoj ", meno, ", rad ta spoznavam.\n")
		prve := 0.0
		druhe := 0.0
		print("Zadaj float cisla ktore chces spocitat:\nPrve: ")
		prve, _ = inputf()
		print("Druhe: ")
		druhe, _ = inputf()
		vysl := 0.0
		vysl = Float(prve, druhe)
		print("Vysledok: ", vysl, "\n")
	}
}

func Float(a float64, b float64) (float64) {
	return a + b
}