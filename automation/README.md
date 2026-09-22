# Weather upload automation

This automation requests Munich weather every five minutes and uploads temperature and humidity observations to FROST:

- `weather_uploader.py` requests current conditions from WeatherAPI and creates two SensorThings Observations;
- `weather-upload.yml` defines the `*/5 * * * *` schedule and manual dispatch option.

To reuse it with infrastructure you control, configure these secrets or environment variables:

- `WEATHERAPI_KEY`
- `FROST_BASE_URL`
- `FROST_TEMPERATURE_DATASTREAM_ID`
- `FROST_HUMIDITY_DATASTREAM_ID`
- optional `FROST_API_TOKEN`

For a local run, copy the included template and fill in your own values:

```bash
cp automation/.env.example automation/.env
set -a
source automation/.env
set +a
```

The populated `.env` file is excluded from version control. In GitHub Actions, store the same values under **Settings → Secrets and variables → Actions**; the workflow injects them at runtime.

Run without `--submit` to inspect the payloads safely. Add `--submit` only when the target FROST instance and Datastream IDs are yours.

To activate the workflow in a fork, copy `weather-upload.yml` to `.github/workflows/` after configuring the repository secrets.
