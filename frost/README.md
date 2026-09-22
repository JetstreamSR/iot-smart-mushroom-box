# FROST SensorThings configuration

The final presentation preserved two code screenshots that document how Cayenne LPP channels were mapped to FROST Datastreams. `channel_mapping.example.js` transcribes that mapping, with the device identifier replaced by an environment variable.

## Historical channel mapping

| Cayenne LPP channel | Measurement | FROST Datastream |
| ---: | --- | ---: |
| 1 | Box temperature | [1507](https://gi3.gis.lrg.tum.de/frost/v1.1/Datastreams(1507)) |
| 2 | Growing-medium moisture | [1508](https://gi3.gis.lrg.tum.de/frost/v1.1/Datastreams(1508)) |
| 3 | Illuminance | [1670](https://gi3.gis.lrg.tum.de/frost/v1.1/Datastreams(1670)) |
| 4 | Indoor air quality | [1671](https://gi3.gis.lrg.tum.de/frost/v1.1/Datastreams(1671)) |
| 5 | Box humidity | [1673](https://gi3.gis.lrg.tum.de/frost/v1.1/Datastreams(1673)) |

The public FROST server still returns metadata and observations for the project. Because it is course infrastructure, availability and write permissions may change.

## Download archived observations

The downloader uses only Python's standard library:

```bash
python frost/download_observations.py --output data/frost_observations.csv
```

It requests the five box Datastreams and the two Munich weather Datastreams, follows SensorThings pagination links, and writes one CSV file. By default it keeps at most 1,000 observations from each Datastream to avoid an unexpectedly large download. Pass `--max-per-stream 0` to retrieve every available observation.
