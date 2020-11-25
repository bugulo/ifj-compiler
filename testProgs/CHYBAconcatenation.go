package main

func main() {
    main := "hello"
    call := "world"
    printer := ""
    printer = f(main, call)
    for i := 1; i < 11; i = i + 1 {
        print(i, ": ", printer, "\n")
    }
}

func f(param string, call string) (string) {
    space := " "
    final := param + space + call
    return final
}