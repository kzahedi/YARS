# ASan corpus — skipped configs

These XML configurations were excluded from the 500-iteration ASan corpus run because
they cannot start without an external controller library or runtime peer.

## Controller-not-found (exit 255 at startup)

| Config                  | Required controller / runtime |
| ----------------------- | ----------------------------- |
| `xml/hexapod.xml`       | Python controller             |
| `xml/hexapod_low.xml`   | Python controller             |
| `xml/hexapod_crbm.xml`  | CRBM C++ controller           |
| `xml/braitenberg.xml`   | (ran — included in corpus)    |

(Note: the table above lists configs that *failed* with controller errors; ran-but-passed
configs are not repeated here.)

## IPC-dependent configs (require a peer on the other side of the socket / pipe)

| Config                                            | Channel    |
| ------------------------------------------------- | ---------- |
| `xml/braitenberg_controller_tcpip.xml`            | TCP/IP     |
| `xml/braitenberg_controller_named_pipe.xml`       | named pipe |
| `xml/braitenberg_controller_go.xml`               | TCP/IP (Go)|
| `xml/braitenberg_controller_go_gui.xml`           | TCP/IP (Go)|
| `xml/braitenberg_controller_julia.xml`            | TCP/IP     |
| `xml/braitenberg_controller_matlab.xml`           | TCP/IP     |
| `xml/braitenberg_controller_python.xml`           | TCP/IP     |
| `xml/muscle_tcpip.xml`                            | TCP/IP     |
| `xml/hexapod_mpi.xml`                             | MPI        |
| `xml/hexapod_ralf.xml`                            | (ran — included; see hexapod_ralf.log)|

## Re-enable when

- Python/CRBM controllers are built and available on `--lib` path.
- A short-lived test controller is spawned alongside the run for the IPC variants.
- The `controller-build-verification` capability provides a way to declare which
  controllers a given build supports.
