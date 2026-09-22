#!/usr/bin/env python3
"""Fetch current Munich weather and optionally post it to FROST."""

import argparse
import json
import os
from datetime import datetime, timezone
from urllib.parse import urlencode
from urllib.request import Request, urlopen


def required_environment(name):
    value = os.getenv(name)
    if not value:
        raise SystemExit(f"Missing required environment variable: {name}")
    return value


def request_json(url, *, method="GET", payload=None, token=None):
    headers = {"Accept": "application/json"}
    data = None
    if payload is not None:
        headers["Content-Type"] = "application/json"
        data = json.dumps(payload).encode("utf-8")
    if token:
        headers["Authorization"] = f"Bearer {token}"
    request = Request(url, data=data, headers=headers, method=method)
    with urlopen(request, timeout=30) as response:
        body = response.read()
        return json.loads(body) if body else None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--location", default="Munich")
    parser.add_argument(
        "--submit",
        action="store_true",
        help="Post observations. Without this flag, only print the payloads.",
    )
    args = parser.parse_args()

    weather_key = required_environment("WEATHERAPI_KEY")
    query = urlencode({"key": weather_key, "q": args.location, "aqi": "no"})
    weather = request_json(f"https://api.weatherapi.com/v1/current.json?{query}")

    phenomenon_time = datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")
    payloads = {
        required_environment("FROST_TEMPERATURE_DATASTREAM_ID"): {
            "phenomenonTime": phenomenon_time,
            "result": weather["current"]["temp_c"],
        },
        required_environment("FROST_HUMIDITY_DATASTREAM_ID"): {
            "phenomenonTime": phenomenon_time,
            "result": weather["current"]["humidity"],
        },
    }

    print(json.dumps(payloads, indent=2))
    if not args.submit:
        print("Dry run only. Add --submit to create observations.")
        return

    frost_base = required_environment("FROST_BASE_URL").rstrip("/")
    token = os.getenv("FROST_API_TOKEN")
    for datastream_id, payload in payloads.items():
        url = f"{frost_base}/Datastreams({datastream_id})/Observations"
        request_json(url, method="POST", payload=payload, token=token)
        print(f"Uploaded observation to Datastream {datastream_id}")


if __name__ == "__main__":
    main()
