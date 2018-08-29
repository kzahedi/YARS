package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"os/exec"
	"strconv"
	"strings"

	"github.com/pkg/errors"
)

// YarsDomain has a min and max value
type YarsDomain struct {
	Min float64
	Max float64
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
	Value     []float64
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
	fmt.Println(yarsCfg)
	yarsSendString("QUIT")
	fmt.Println("Done")
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

	for i := 0; i < e.Dimension; i++ {
		min, max := yarsReadDomain()
		e.Domain[i].Min = min
		e.Domain[i].Max = max

		min, max = yarsReadDomain()
		e.Domain[i].Min = min
		e.Domain[i].Max = max
	}

	return e
}

func yarsReadDomain() (float64, float64) {
	str := yarsReadString() // INTERNAL DOMAIN
	s := strings.Split(str, " ")
	min, _ := strconv.ParseFloat(s[2], 64)
	max, _ := strconv.ParseFloat(s[3], 64)
	return min, max
}
