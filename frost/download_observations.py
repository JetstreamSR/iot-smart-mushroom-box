#!/usr/bin/env python3
"""Download the archived box and weather observations from FROST."""

import argparse
import csv
import json
from pathlib import Path
from urllib.parse import urlencode
from urllib.request import urlopen

FROST_BASE_URL = "https://gi3.gis.lrg.tum.de/frost/v1.1"
DATASTREAMS = {
    1507: "box_temperature",
    1508: "soil_moisture",
    1670: "box_illuminance",
    1671: "box_iaq",
    1673: "box_humidity",
    1665: "weather_temperature",
    1666: "weather_humidity",
}


def read_json(url):
    with urlopen(url, timeout=30) as response:
        return json.load(response)


def observations(datastream_id, max_per_stream):
    query = urlencode(
        {
            "$select": "phenomenonTime,result,resultTime",
            "$orderby": "phenomenonTime asc",
            "$top": 1000,
        }
    )
    url = f"{FROST_BASE_URL}/Datastreams({datastream_id})/Observations?{query}"
    count = 0
    while url:
        page = read_json(url)
        for observation in page.get("value", []):
            if max_per_stream and count >= max_per_stream:
                return
            yield observation
            count += 1
        url = page.get("@iot.nextLink")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, default=Path("frost_observations.csv"))
    parser.add_argument(
        "--max-per-stream",
        type=int,
        default=1000,
        help="Maximum observations per Datastream; use 0 for all observations.",
    )
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)

    with args.output.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(
            stream,
            fieldnames=[
                "datastream_id",
                "measurement",
                "phenomenon_time",
                "result_time",
                "result",
            ],
        )
        writer.writeheader()
        for datastream_id, measurement in DATASTREAMS.items():
            for observation in observations(datastream_id, args.max_per_stream):
                writer.writerow(
                    {
                        "datastream_id": datastream_id,
                        "measurement": measurement,
                        "phenomenon_time": observation.get("phenomenonTime"),
                        "result_time": observation.get("resultTime"),
                        "result": observation.get("result"),
                    }
                )

    print(f"Wrote {args.output}")


if __name__ == "__main__":
    main()
