package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func walkFn(path string, info os.FileInfo, err error) error{
	if err != nil {
		fmt.Printf("Error accessing path %q: %v\n", path, err)
		return err
	}

	switch mode := info.Mode(); true{
	case mode.IsDir():
		nDir++
	case mode&os.ModeSymlink != 0:
		nSL++
	case mode&os.ModeDevice != 0:
		nDev++
	case mode&os.ModeSocket != 0:
		nSock++
	default:
		nOthers++
	}
	return nil
}

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	err := filepath.Walk(dir, walkFn)
	if err != nil {
		return err
	}
	return nil
}
var nDir, nSL, nDev, nSock, nOthers int
func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	err := scanDir(os.Args[1])
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
	str := 	   "+-----------------+----------+"
	fmt.Println("Directory Scanning Tool")
	fmt.Println(str)
	fmt.Printf("| Path            | %-9s|\n", os.Args[1])
	fmt.Println(str)
	fmt.Printf("| Directories     | %-9d|\n", nDir)
	fmt.Printf("| Symbolic Links  | %-9d|\n", nSL)
	fmt.Printf("| Devices         | %-9d|\n", nDev)
	fmt.Printf("| Sockets         | %-9d|\n", nSock)
	fmt.Printf("| Other files     | %-9d|\n", nOthers)
	fmt.Println(str)
}
