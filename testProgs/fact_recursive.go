package main
func factorial(n int) (int) {
dec_n := n - 1
if n < 2 {
return 1
} else {
tmp := 0
tmp = factorial(dec_n)
return n * tmp
}
}
func main() {
print("Zadejte cislo pro vypocet faktorialu: ")
a := 0
err := 0
a = 5
if err == 0 {
if a < 0 {
print("Faktorial nejde spocitat!", "\n")
} else {
vysl := 0
vysl = factorial(a)
print("Vysledek je ", vysl, "\n")
}
} else {
print("Chyba pri nacitani celeho cisla!\n")
}
}
