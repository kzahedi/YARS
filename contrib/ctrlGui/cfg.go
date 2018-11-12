package main

import (
	"encoding/xml"
	"os"
)

// XML Window info
type XMLWindow struct {
	XMLName xml.Name `xml:"window"`
	Name    string   `xml:"name,attr"`
	X       int      `xml:"x,attr"`
	Y       int      `xml:"y,attr"`
	Width   int      `xml:"width,attr"`
	Height  int      `xml:"height,attr"`
}

// XML Sensor info
type XMLSensor struct {
	XMLName xml.Name `xml:"sensor"`
	Name    string   `xml:"name,attr"`
	X       int      `xml:"x,attr"`
	Y       int      `xml:"y,attr"`
	Width   int      `xml:"width,attr"`
	Height  int      `xml:"height,attr"`
}

// XMLActuator info
type XMLActuator struct {
	XMLName xml.Name `xml:"actuator"`
	Name    string   `xml:"name,attr"`
	X       int      `xml:"x,attr"`
	Y       int      `xml:"y,attr"`
	Width   int      `xml:"width,attr"`
	Height  int      `xml:"height,attr"`
	Initial float32  `xml:"initial,attr"`
}

// XMLCfg xml config
type XMLCfg struct {
	XMLName  xml.Name      `xml:"cfg"`
	Name     string        `xml:"key,attr"`
	Window   XMLWindow     `xml:"window"`
	Actuator []XMLActuator `xml:"actuator"`
	Sensor   []XMLSensor   `xml:"sensor"`
}

// ReadCfg read config
func ReadCfg(filename string) (XMLCfg, error) {
	xmlCfg := XMLCfg{Name: "", Window: XMLWindow{Name: "", X: 0, Y: 0, Width: 800, Height: 600}}

	file, err := os.Open(filename)
	defer file.Close()
	if err != nil {
		return xmlCfg, err
	}

	if err = xml.NewDecoder(file).Decode(&xmlCfg); err != nil {
		return xmlCfg, err
	}

	return xmlCfg, nil
}

func getSensor(cfg XMLCfg, name string) (int, int, int, int, bool) {
	if len(cfg.Sensor) == 0 {
		return 0, 0, 0, 0, false
	}
	for _, s := range cfg.Sensor {
		if s.Name == name {
			return s.X, s.Y, s.Width, s.Height, true
		}
	}
	return 0, 0, 0, 0, false
}

func getActuator(cfg XMLCfg, name string) (int, int, int, int, float32, bool) {
	if len(cfg.Actuator) == 0 {
		return 0, 0, 0, 0, 0.0, false
	}
	for _, a := range cfg.Actuator {
		if a.Name == name {
			return a.X, a.Y, a.Width, a.Height, a.Initial, true
		}
	}
	return 0, 0, 0, 0, 0.0, false
}
