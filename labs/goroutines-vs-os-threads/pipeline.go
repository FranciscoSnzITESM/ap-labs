package main

import(
	"fmt"
	"strconv"
	"os"
	"time"
)

func routine(n int, returnCH chan time.Time){
	if n > 1 {
		// fmt.Println("Goroutine waiting:", n)
		getCH := make(chan time.Time)
		go routine(n - 1, getCH)
		value := <- getCH
		// fmt.Println("Goroutine got value:", n)
		returnCH <- value
		return
	}
	// fmt.Println("Final goroutine")
	returnCH <- time.Now()
}

func main(){
	if len(os.Args) < 2{
		fmt.Println("Wrong parameters, usage: go run pipeline.go <number of goroutines>")
		return
	}
	i, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println("Error, can't convert to integer the parameter recieved")
		return
	}
	if i < 1 {
		fmt.Println("Error, the amount of goroutines has to be greater than 0")
		return
	}
	getCH := make(chan time.Time)
	go routine(i, getCH)
	value := <- getCH
	fmt.Println("Time taken for the value to transit the entire pipeline:",time.Since(value))
}