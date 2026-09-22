# Grafana dashboard

The project dashboard combines indoor measurements from the mushroom box with outdoor Munich weather observations. The dashboard used the TUM course [Grafana service](https://gi3.gis.lrg.tum.de/grafana/) and the FROST SensorThings Datastreams listed in `dashboard_data_mapping.json`.

## Dashboard panels

| Panel | Visualization | Datastream |
| --- | --- | ---: |
| Weather API temperature | Gauge | 1665 |
| Box temperature | Gauge | 1507 |
| Temperature outside vs inside | Time series | 1665 and 1507 |
| Weather API humidity | Gauge | 1666 |
| Box humidity | Gauge | 1673 |
| Humidity outside vs inside | Time series | 1666 and 1673 |
| Soil moisture | Time series | 1508 |
| Box illuminance | Time series | 1670 |
| Box indoor air quality | Time series | 1671 |

The project README includes the final dashboard view for the monitoring period from 8 to 13 July 2025.

## Data quality

The dashboard screenshot includes the hardware and data-pipeline debugging period. Large excursions in the growing-medium moisture, illuminance, and indoor-air-quality panels are transient test artifacts. They should be excluded when interpreting environmental changes or evaluating the control response.
