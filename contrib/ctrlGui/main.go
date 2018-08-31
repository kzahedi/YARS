package main

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"time"

	"github.com/go-gl/gl/v3.2-core/gl"
	"github.com/go-gl/glfw/v3.2/glfw"
	"github.com/golang-ui/nuklear/nk"
	"github.com/xlab/closer"
)

const (
	winWidth  = 1000
	winHeight = 400

	panelWidth = 450

	maxVertexBuffer  = 512 * 1024
	maxElementBuffer = 128 * 1024
)

var filename []byte
var port []byte
var isInitialised bool
var cfg XMLCfg

func init() {
	runtime.LockOSThread()
}

func main() {
	filename = make([]byte, 1024, 1024)
	if len(os.Args) > 1 {
		for i, v := range os.Args[1] {
			filename[i] = byte(v)
		}
	}

	xml, err := ReadCfg("cfg.xml")
	if err != nil {
		panic(err)
	}
	cfg = xml

	isInitialised = false
	port = make([]byte, 6, 6)

	if err := glfw.Init(); err != nil {
		closer.Fatalln(err)
	}
	glfw.WindowHint(glfw.ContextVersionMajor, 3)
	glfw.WindowHint(glfw.ContextVersionMinor, 2)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCoreProfile)
	glfw.WindowHint(glfw.OpenGLForwardCompatible, glfw.True)
	win, err := glfw.CreateWindow(cfg.Window.Width, cfg.Window.Height, cfg.Window.Name, nil, nil)
	if err != nil {
		closer.Fatalln(err)
	}
	win.MakeContextCurrent()

	width, height := win.GetSize()
	log.Printf("glfw: created window %dx%d", width, height)

	if err := gl.Init(); err != nil {
		closer.Fatalln("opengl: init failed:", err)
	}
	gl.Viewport(0, 0, int32(width), int32(height))

	ctx := nk.NkPlatformInit(win, nk.PlatformInstallCallbacks)

	atlas := nk.NewFontAtlas()
	nk.NkFontStashBegin(&atlas)
	sansFont := nk.NkFontAtlasAddFromBytes(atlas, MustAsset("assets/FreeSans.ttf"), 16, nil)
	// sansFont := nk.NkFontAtlasAddDefault(atlas, 16, nil)
	nk.NkFontStashEnd()
	if sansFont != nil {
		nk.NkStyleSetFont(ctx, sansFont.Handle())
	}

	exitC := make(chan struct{}, 1)
	doneC := make(chan struct{}, 1)
	closer.Bind(func() {
		close(exitC)
		<-doneC
	})

	state := &State{
		bgColor: nk.NkRgba(28, 48, 62, 255),
	}
	fpsTicker := time.NewTicker(time.Second / 30)
	for {
		select {
		case <-exitC:
			nk.NkPlatformShutdown()
			glfw.Terminate()
			fpsTicker.Stop()
			close(doneC)
			return
		case <-fpsTicker.C:
			if win.ShouldClose() {
				close(exitC)
				continue
			}
			glfw.PollEvents()
			gfxMain(win, ctx, state)
		}
	}
}

func yarsPanel(win *glfw.Window, ctx *nk.Context, state *State) {
	// YARS Panel
	bounds := nk.NkRect(float32(cfg.Window.Width-255), float32(cfg.Window.Height-140), 255, 130)
	update := nk.NkBegin(ctx, "YARS Control Panel", bounds,
		nk.WindowBorder|nk.WindowMovable|nk.WindowMinimizable|nk.WindowTitle)

	if update > 0 {
		nk.NkLayoutRowDynamic(ctx, 30, 1)
		{
			nk.NkEditStringZeroTerminated(ctx, nk.EditField, filename, 1024, nk.NkFilterDefault)

		}
		nk.NkLayoutRowDynamic(ctx, 30, 3)
		{
			if nk.NkButtonLabel(ctx, "start") > 0 {
				port := YarsStart(string(filename))
				if port > 0 {
					err := YarsConnect(port)
					if err != nil {
						panic(err)
					}
					isInitialised = true
				}
			}
			if nk.NkButtonLabel(ctx, "reset") > 0 {
				log.Println("reset pressed")
				YarsReset()
			}
			if nk.NkButtonLabel(ctx, "quit") > 0 {
				log.Println("reset pressed")
				YarsQuit()
			}
		}
	}
	nk.NkEnd(ctx)
}

func actuatorPanel(win *glfw.Window, ctx *nk.Context, state *State) {
	// YARS Panel
	for i, a := range yarsCfg.Actuators {
		var update int32
		x, y, w, h, found := getActuator(cfg, a.Name)
		if found {
			bounds := nk.NkRect(float32(x), float32(y), float32(w), float32(h))
			update = nk.NkBegin(ctx, a.Name, bounds,
				nk.WindowBorder|nk.WindowMovable|nk.WindowScalable|nk.WindowMinimizable|nk.WindowTitle)
		} else {
			bounds := nk.NkRect(float32(420+i*10), float32(10+i*10), float32(panelWidth+10), float32(25*a.Dimension))
			update = nk.NkBegin(ctx, a.Name, bounds,
				nk.WindowBorder|nk.WindowMovable|nk.WindowScalable|nk.WindowMinimizable|nk.WindowTitle)
		}
		if update > 0 {
			for i := 0; i < a.Dimension; i++ {
				min := a.Mapping[i].Min
				max := a.Mapping[i].Max
				step := (max - min) / 1000.0
				nk.NkLayoutRowBegin(ctx, nk.Static, 30, 6)
				nk.NkLayoutRowPush(ctx, 75)
				if a.Dimension == 1 {
					nk.NkLabel(ctx, a.Name, nk.Left)
				} else {
					nk.NkLabel(ctx, a.Names[i], nk.Left)
				}
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", min), nk.Right)
				nk.NkLayoutRowPush(ctx, 150)
				a.Value[i] = nk.NkSlideFloat(ctx, min, a.Value[i], max, step)
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", max), nk.Right)
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", a.Value[i]), nk.Left)
				nk.NkLayoutRowEnd(ctx)
			}
		}
		nk.NkEnd(ctx)
	}
}

func sensorPanel(win *glfw.Window, ctx *nk.Context, state *State) {
	// YARS Panel
	for i, s := range yarsCfg.Sensors {
		x, y, w, h, found := getSensor(cfg, s.Name)
		var update int32
		if found {
			bounds := nk.NkRect(float32(x), float32(y), float32(w), float32(h))
			update = nk.NkBegin(ctx, s.Name, bounds,
				nk.WindowBorder|nk.WindowMovable|nk.WindowScalable|nk.WindowMinimizable|nk.WindowTitle)
		} else {
			bounds := nk.NkRect(float32(420+i*10), float32(10+i*10), float32(panelWidth+10), float32(25+30*s.Dimension))
			update = nk.NkBegin(ctx, s.Name, bounds,
				nk.WindowBorder|nk.WindowMovable|nk.WindowScalable|nk.WindowMinimizable|nk.WindowTitle)
		}

		if update > 0 {
			for i := 0; i < s.Dimension; i++ {
				min := s.Mapping[i].Min
				max := s.Mapping[i].Max
				step := (max - min) / 1000.0
				nk.NkLayoutRowBegin(ctx, nk.Static, 30, 6)
				nk.NkLayoutRowPush(ctx, 75)
				nk.NkLabel(ctx, s.Names[i], nk.Left)
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", min), nk.Right)
				nk.NkLayoutRowPush(ctx, 150)
				nk.NkSlideFloat(ctx, min, s.Value[i], max, step)
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", max), nk.Right)
				nk.NkLayoutRowPush(ctx, 50)
				nk.NkLabel(ctx, fmt.Sprintf("%.2f", s.Value[i]), nk.Left)
				nk.NkLayoutRowEnd(ctx)
			}
		}
		nk.NkEnd(ctx)
	}
}

func gfxMain(win *glfw.Window, ctx *nk.Context, state *State) {
	nk.NkPlatformNewFrame()

	yarsPanel(win, ctx, state)
	if isInitialised == true {
		actuatorPanel(win, ctx, state)
		sensorPanel(win, ctx, state)
		YarsUpdate()
	}

	// Render
	bg := make([]float32, 4)
	nk.NkColorFv(bg, state.bgColor)
	width, height := win.GetSize()
	gl.Viewport(0, 0, int32(width), int32(height))
	gl.Clear(gl.COLOR_BUFFER_BIT)
	gl.ClearColor(bg[0], bg[1], bg[2], bg[3])
	nk.NkPlatformRender(nk.AntiAliasingOn, maxVertexBuffer, maxElementBuffer)
	win.SwapBuffers()
}

type Option uint8

const (
	Easy Option = 0
	Hard Option = 1
)

type State struct {
	bgColor nk.Color
	prop    int32
	opt     Option
}

func onError(code int32, msg string) {
	log.Printf("[glfw ERR]: error %d: %s", code, msg)
}

// bounds := nk.NkRect(50, 50, 230, 250)
// update := nk.NkBegin(ctx, "Demo", bounds,
// 	nk.WindowBorder|nk.WindowMovable|nk.WindowScalable|nk.WindowMinimizable|nk.WindowTitle)

// if update > 0 {
// 	nk.NkLayoutRowStatic(ctx, 30, 80, 1)
// 	{
// 		if nk.NkButtonLabel(ctx, "button") > 0 {
// 			log.Println("[INFO] button pressed!")
// 		}
// 	}
// 	nk.NkLayoutRowDynamic(ctx, 30, 2)
// 	{
// 		if nk.NkOptionLabel(ctx, "easy", flag(state.opt == Easy)) > 0 {
// 			state.opt = Easy
// 		}
// 		if nk.NkOptionLabel(ctx, "hard", flag(state.opt == Hard)) > 0 {
// 			state.opt = Hard
// 		}
// 	}
// 	nk.NkLayoutRowDynamic(ctx, 25, 1)
// 	{
// 		nk.NkPropertyInt(ctx, "Compression:", 0, &state.prop, 100, 10, 1)
// 	}
// 	nk.NkLayoutRowDynamic(ctx, 20, 1)
// 	{
// 		nk.NkLabel(ctx, "background:", nk.TextLeft)
// 	}
// 	nk.NkLayoutRowDynamic(ctx, 25, 1)
// 	{
// 		size := nk.NkVec2(nk.NkWidgetWidth(ctx), 400)
// 		if nk.NkComboBeginColor(ctx, state.bgColor, size) > 0 {
// 			nk.NkLayoutRowDynamic(ctx, 120, 1)
// 			state.bgColor = nk.NkColorPicker(ctx, state.bgColor, nk.ColorFormatRGBA)
// 			nk.NkLayoutRowDynamic(ctx, 25, 1)
// 			r, g, b, a := state.bgColor.RGBAi()
// 			r = nk.NkPropertyi(ctx, "#R:", 0, r, 255, 1, 1)
// 			g = nk.NkPropertyi(ctx, "#G:", 0, g, 255, 1, 1)
// 			b = nk.NkPropertyi(ctx, "#B:", 0, b, 255, 1, 1)
// 			a = nk.NkPropertyi(ctx, "#A:", 0, a, 255, 1, 1)
// 			state.bgColor.SetRGBAi(r, g, b, a)
// 			nk.NkComboEnd(ctx)
// 		}
// 	}
// }
// nk.NkEnd(ctx)
