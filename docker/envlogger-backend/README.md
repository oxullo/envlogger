# ENVLogger backend compose

Sample backend based on InfluxDB + Grafana + Chronograf.

It requires ENVLogger v1 hardware to run the firmware `remote_logger`. Ensure the firmware is properly configured with the exposed IP of the machine running docker.

Note: mainly based on https://github.com/jkehres/docker-compose-influxdb-grafana.git . Full documentation of the underlying setup can be found there.

## Quick Start

To start the app:

1. Install [docker-compose](https://docs.docker.com/compose/install/) on the docker host.
1. Clone this repo on the docker host.
1. Optionally, change default credentials or Grafana provisioning.
1. Run the following command from the root of the cloned repo:
```
docker-compose up -d
```

To stop the app:

1. Run the following command from the root of the cloned repo:
```
docker-compose down
```

## Ports

The services in the app run on the following ports:

| Host Port | Service |
| - | - |
| 3000 | Grafana |
| 8086 | InfluxDB HTTP |
| 8089 | InfluxDB UDP line protocol |
| 8888 | Chronograf |

Note that Chronograf does not support username/password authentication. Anyone who can connect to the service has full admin access.

## Volumes

The app creates the following named volumes (one for each service) so data is not lost when the app is stopped:

* influxdb-storage
* chronograf-storage
* grafana-storage

## Users

The app creates two admin users - one for InfluxDB and one for Grafana. By default, the username and password of both accounts is `admin`. To override the default credentials, set the following environment variables before starting the app:

* `INFLUXDB_USERNAME`
* `INFLUXDB_PASSWORD`
* `GRAFANA_USERNAME`
* `GRAFANA_PASSWORD`

The defaults can be found in the `.env` file.

## Database

The app creates a default InfluxDB database called `iot`. This is also the database which the UDP endpoint in use feeds data into.

## Data Sources

The app creates a Grafana data source called `InfluxDB` that's connected to the default IndfluxDB database (e.g. `iot`).

## Dashboards

A sample dashboard exposing all the ingested variables is provided and automatically deployed.
