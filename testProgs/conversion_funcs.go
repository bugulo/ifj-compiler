package main

func convert(integer int, float float64) (int, float64) {
	if integer != 0 {
		a := 0.0
		a = int2float(integer)
		return 0, a
	} else {
		b := 0
		b = float2int(float)
		return b, 0.0
	}
}

func main() {
	print("Zadaj typ cisla na konverziu[int/float]: ")
	str := ""
	err := 0
	str, err = inputs()
	if err != 0 {
		print("Zly input!\n")
		main()
	} else {
		if str == "int" {
			print("Zadaj int cislo na konverziu: ")
			integer := 0
			err := 0
			integer, err = inputi()
			if err != 0 {
				print("Zly input!\n")
				main()
			} else {
				vysl := 0.0
				_, vysl = convert(integer, 0.0)
				print("Vysledok je: ", vysl, "\n")
			}
		} else {
			if str == "float" {
				print("Zadaj float cislo na konverziu: ")
				float := 0.0
				err := 0
				float, err = inputf()
				if err != 0 {
					print("Zly input!\n")
					main()
				} else {
					vysl := 0
					vysl, _ = convert(0, float)
					print("Vysledok je: ", vysl, "\n")
				}
			} else {
				print("Zly input!\n")
				main()
			}
		}
	}
}