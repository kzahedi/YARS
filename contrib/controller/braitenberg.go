package main

import "C"
import "math"

//export Init
func Init() {} // will add parameters later

//export Update
func Update(s []float64, a []float64) {
	a[1] = math.Tanh(-1.0 / 3.0 * (s[0] + s[1] + s[2]))
	a[0] = math.Tanh(-1.0 / 3.0 * (s[3] + s[4] + s[5]))
}

//export Reset
func Reset() {}

//export Close
func Close() {}

func main() {}
