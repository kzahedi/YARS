## ADDED Requirements

### Requirement: contrib/controller compiles as shared library
All C++ controllers in `contrib/controller/` SHALL compile into a single shared library (`libYarsControllers.dylib`) that YARS can load via `--lib`.

Controllers included: Braitenberg2a, Braitenberg2b, Braitenberg3a, Braitenberg3b, ConstantValue, NIL, NMODEController, PythonController, SineController, SquareWave, TCPIP, NamedPipeController, MATLAB.

#### Scenario: Shared library builds without errors
- **WHEN** `cmake && make` is run in `contrib/controller/`
- **THEN** `libYarsControllers.dylib` is produced with no compiler errors

#### Scenario: YARS loads the library at runtime
- **WHEN** `yars --lib <path>/libYarsControllers.dylib --xml braitenberg.xml --iterations 10`
- **THEN** the Braitenberg2b controller is loaded and simulation runs without error

### Requirement: External language controller sources are syntax-verified
Script-based controllers (Python, Go, Julia) SHALL be verified for syntax correctness.

#### Scenario: Python controller passes syntax check
- **WHEN** `python3 -m py_compile contrib/controller/braitenberg.py` is run
- **THEN** exits with code 0

#### Scenario: Go controller passes compilation check
- **WHEN** `go build contrib/controller/braitenberg.go` is run (if Go is installed)
- **THEN** exits with code 0, or absence of Go is documented

#### Scenario: Julia controller passes syntax check
- **WHEN** `julia --check-bounds=yes contrib/controller/braitenberg.jl` is run (if Julia is installed)
- **THEN** exits with code 0, or absence of Julia is documented
