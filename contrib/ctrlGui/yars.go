package main

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"net"
	"os"
	"os/exec"
	"strconv"
	"strings"

	"github.com/pkg/errors"
)

// YarsDomain has a min and max value
type YarsDomain struct {
	Min float32
	Max float32
}

// String to string
func (yd YarsDomain) String() string {
	return fmt.Sprintf("%f %f", yd.Min, yd.Max)
}

// YarsEntityConfiguration stores Sensor / Actuator configurations
type YarsEntityConfiguration struct {
	Name      string
	Dimension int
	Domain    []YarsDomain
	Mapping   []YarsDomain
	Value     []float32
}

func (yec YarsEntityConfiguration) String() string {
	s := fmt.Sprintf("Name: %s\nDimension %d\n", yec.Name, yec.Dimension)
	for i := 0; i < yec.Dimension; i++ {
		s = fmt.Sprintf("%sDomain %s\n", s, yec.Domain[i].String())
		s = fmt.Sprintf("%sMapping %s\n", s, yec.Domain[i].String())
	}
	return s
}

func makeYec() YarsEntityConfiguration {
	return YarsEntityConfiguration{Name: "", Dimension: -1, Domain: nil, Value: nil}
}

// YarsConfiguration stores sensor and actuator configurations
type YarsConfiguration struct {
	Name      string
	Sensors   []YarsEntityConfiguration
	Actuators []YarsEntityConfiguration
}

func (yc YarsConfiguration) String() string {
	s := fmt.Sprintf("Name: %s", yc.Name)
	for _, sensor := range yc.Sensors {
		s = fmt.Sprintf("%sSensor %s", s, sensor.String())
	}
	for _, actautor := range yc.Sensors {
		s = fmt.Sprintf("%sActuator %s", s, actautor.String())
	}
	return s
}

func makeYc() YarsConfiguration {
	return YarsConfiguration{Sensors: nil, Actuators: nil}
}

var yarsIO *bufio.ReadWriter
var yarsCfg YarsConfiguration

// YarsStart runs the yars executable (background)
func YarsStart(filename string) {
	fmt.Println(filename)
	cmd := exec.Command("yars", "--printTime", "xml/braitenberg_controller_tcpip.xml")
	cmd.Stdout = os.Stdout
	err := cmd.Start()
	if err != nil {
		log.Fatal(err)
	}
}

// YarsConnect opens the TCP/IP connection to YARS
func YarsConnect(port int) error {
	addr := fmt.Sprintf("localhost:%d", port)
	log.Println("Dial " + addr)
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return errors.Wrap(err, "Dialing "+addr+" failed")
	}
	yarsIO = bufio.NewReadWriter(bufio.NewReader(conn), bufio.NewWriter(conn))
	YarsGetConfiguration()
	return nil
}

// YarsGetConfiguration fills yarsCfg
func YarsGetConfiguration() {
	yarsCfg = makeYc()
	yarsSendString("CONFIGURATION")
	yarsReadString() // BEGIN CONFIGURIOATN
	yarsReadString() // BEGIN DATA TYPES
	yarsReadString() // INTEGER n BYTES
	yarsReadString() // DOUBLE n BYTES
	yarsReadString() // END DATA TYPES
	yarsReadString() // BEGIN ROBOT DATA

	name := yarsReadString()
	yarsCfg.Name = name[5:]

	var sensors []YarsEntityConfiguration
	var actuators []YarsEntityConfiguration

	for {
		str := yarsReadString()
		if strings.Contains(str, "BEGIN SENSOR") {
			sensors = append(sensors, yarsReadEntity())
		}
		if strings.Contains(str, "BEGIN ACTUATOR") {
			actuators = append(actuators, yarsReadEntity())
		}
		if str == "END CONFIGURATION" {
			break
		}
	}

	yarsCfg.Sensors = sensors
	yarsCfg.Actuators = actuators
}

// YarsUpdate send motor commands and reads sensor values
func YarsUpdate() {
	var actuators []float32

	yarsSendString("ACTUATORS")
	for _, a := range yarsCfg.Actuators {
		for _, v := range a.Value {
			actuators = append(actuators, float32(v))
		}
	}
	yarsSendFloatArray(actuators)

	yarsSendString("SENSORS")
	sensorValues := yarsReadFloatArray()

	index := 0
	for _, s := range yarsCfg.Sensors {
		for i := range s.Value {
			s.Value[i] = sensorValues[index]
			index++
		}
	}
}

func yarsReadEntity() YarsEntityConfiguration {
	e := makeYec()
	str := ""

	str = yarsReadString() // NAME ...
	e.Name = str[5:]

	str = yarsReadString() // DIMENSION ...
	str = strings.Split(str, " ")[1]
	n, _ := strconv.ParseInt(str, 10, 64)
	e.Dimension = int(n)

	e.Domain = make([]YarsDomain, n, n)
	e.Mapping = make([]YarsDomain, n, n)
	e.Value = make([]float32, n, n)

	for i := 0; i < e.Dimension; i++ {
		min, max := yarsReadDomain()
		e.Domain[i].Min = min
		e.Domain[i].Max = max

		min, max = yarsReadDomain()
		e.Mapping[i].Min = min
		e.Mapping[i].Max = max

		e.Value[i] = (max + min) / 2.0

	}

	return e
}

func yarsReadDomain() (float32, float32) {
	str := yarsReadString() // INTERNAL DOMAIN
	s := strings.Split(str, " ")
	min, _ := strconv.ParseFloat(s[2], 64)
	max, _ := strconv.ParseFloat(s[3], 64)
	return float32(min), float32(max)
}

func yarsSendString(message string) {
	n := len(message)
	msg := make([]byte, n+5, n+5)
	msg[0] = byte('s')
	binary.LittleEndian.PutUint32(msg[1:5], uint32(n))
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
	if string(id) != "s" {
		fmt.Println("We have a problem: ", string(id))
		os.Exit(-1)
	}
	l := yarsReadBytes(4)
	n := int(binary.LittleEndian.Uint32(l))
	s := yarsReadBytes(n)
	r := string(s)
	return r
}

func yarsSendFloatArray(values []float32) {
	n := len(values)
	var msg []byte
	msg = append(msg, byte('D'))
	l := make([]byte, 4, 4)
	binary.LittleEndian.PutUint32(l, uint32(n))
	for _, v := range l {
		msg = append(msg, v)
	}
	for _, v := range values {
		buf := make([]byte, 8, 8)
		binary.LittleEndian.PutUint64(buf, math.Float64bits(float64(v)))
		for _, w := range buf {
			msg = append(msg, w)
		}
	}
	_, err := yarsIO.Writer.Write(msg)
	if err != nil {
		panic(err)
	}
	yarsIO.Flush()
}

func yarsReadFloatArray() []float32 {
	var r []float32
	id, _ := yarsIO.Reader.ReadByte()
	if string(id) != "D" {
		fmt.Println("We have a problem: ", string(id))
		os.Exit(-1)
	}
	l := yarsReadBytes(4)
	n := int(binary.LittleEndian.Uint32(l))
	for i := 0; i < n; i++ {
		s := yarsReadBytes(8)
		bits := binary.LittleEndian.Uint64(s)
		f := math.Float64frombits(bits)
		r = append(r, float32(f))
	}
	return r
}

func yarsReadBytes(n int) []byte {
	bytes := make([]byte, n, n)
	for i := 0; i < n; i++ {
		bytes[i], _ = yarsIO.Reader.ReadByte()
	}
	return bytes
}
