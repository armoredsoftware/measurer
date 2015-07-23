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
make run-measurer PORT=<port>
```
A new measurer will wait for a client to initiate communication and begin sending measurement requests.

Launch the interactive client:
```bash
make build-client
make run-client PORT=<port>
```
Requests that can be issued to the measurer using the client are described in the [RLI documentation](https://github.com/armoredsoftware/measurer/wiki/Measurer-RLI)

  Running the Unit Tests
----------------------------

Build the tests:
```bash
make build-test
```

Run the tests:
```bash
make run-test
```