package main

func main() {
	print("Zadaj retazec: ")
	str := ""
	str, _ = inputs()
	print("Zadaj poziciu znaku: ")
	cislo := 0
	cislo, _ = inputi()
	ascii := 0
	ascii, _ = ord(str, cislo)
	print("Ordinalna hodnota znaku je: ", ascii, "\n")
	print("Zadaj ordinalnu hodnotu znaku: ")
	str2 := 0
	str2, _ = inputi()
	znak := ""
	znak, _ = chr(str2)
	print("Je to znak: ", znak, "\n")
}