package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	arr := strings.Fields(s)
	mapa := make(map[string]int)
	for _,v := range arr {
		mapa[v]+=1
	}
	return mapa
}

func main() {
	wc.Test(WordCount)
}
