# measurer
JSON-RPC invokable measurer for native applications and the Request Line Interpreter (RLI) interactive client.

  
  How to build the Measurer
----------------------------

Clone a copy of the Measurer:

```bash
git clone git@github.com:armoredsoftware/measurer.git
```

Enter the measurer directory:
```bash
cd measurer
```

Before first build, configure:
```bash
make configure-measurer
```

Build:
```bash
make build-measurer
```

  Running the Measurer
----------------------------

Launch the Measurer:
```bash
make run-measurer
```
A new measurer will wait for a driver to initiate communication and begin sending measurement requests.

Launch the interactive driver:
```bash
make build-measurer
make run-driver
```
Requests that can be issued to the measurer using the driver are described in the RLI documentation.

  Running the Unit Tests
----------------------------

Tests are in the test directory. Run instructions coming soon.
