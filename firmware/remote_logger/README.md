# ENVLOGGER v1 Remote logger

Log timeseries data to an InfluxDB instance

## Uplink to the DB

The logger pushes data to an InfluxDB instance. It uses InfluxDB line protocol via UDP:

https://docs.influxdata.com/influxdb/v1.8/write_protocols/line_protocol_reference/#sidebar

Check the provided sample docker setup to run a local instance of InfluxDB+Grafana

## Configuration

Parameters relevant for the configuration can be found in src/reporter.cpp:

* INFLUX_SERVER_IP: IP address of the InfluxDB instance
* INFLUX_SERVER_PORT: listening port of the UDP endpoint

## Compilation and flashing

Using Particle desktop IDE (https://docs.particle.io/tutorials/developer-tools/dev/).
Ensure that the local compiler toolchain is installed.

1. CTRL-SHIFT-p (or CMD-SHIFT-p on OSX) -> Particle: Install library -> Adafruit_BME680
2. CTRL-SHIFT-p (or CMD-SHIFT-p on OSX) -> Particle: Flash application (local)

Note: the photon will detach from the cloud since the firmware configures the unit to run
in MANUAL system mode.
