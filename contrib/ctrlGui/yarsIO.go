package main

import (
	"encoding/binary"
	"fmt"
	"os"
)

func yarsSendString(message string) {
	n := len(message)
	msg := make([]byte, n+5, n+5)
	l := make([]byte, 4, 4)
	msg[0] = byte('s')
	binary.LittleEndian.PutUint32(l, uint32(n))
	fmt.Println(l)
	// binary.BigEndian.PutUint32(l, uint32(n))
	for i := 0; i < 4; i++ {
		msg[i+1] = l[i]
	}
	for i := 0; i < n; i++ {
		msg[i+5] = byte(message[i])
	}
	_, err := yarsIO.Writer.Write(msg)
	if err != nil {
		panic(err)
	}
	yarsIO.Flush()
}

func yarsReadString() string {
	id, _ := yarsIO.Reader.ReadByte()
	fmt.Println("received ", string(id))
	if string(id) != "s" {
		fmt.Println("We have a problem: ", string(id))
		os.Exit(-1)
	}
	l := yarsReadBytes(4)
	n := int(binary.LittleEndian.Uint32(l))
	s := yarsReadBytes(n)
	r := string(s)
	fmt.Println(r)
	return r
}

func yarsReadBytes(n int) []byte {
	bytes := make([]byte, n, n)
	for i := 0; i < n; i++ {
		bytes[i], _ = yarsIO.Reader.ReadByte()
	}
	return bytes
}
