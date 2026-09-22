# Weather upload automation

The final presentation documents a scheduled GitHub Actions job that requested Munich weather every five minutes and uploaded the temperature and humidity to FROST. The original repository and WeatherAPI key were not included in the submitted project folder.

The files here provide a credential-free reconstruction of that documented workflow:

- `weather_uploader.py` requests current conditions from WeatherAPI and creates two SensorThings Observations;
- `weather-upload.example.yml` reproduces the submitted `*/5 * * * *` schedule but remains outside `.github/workflows`, so it cannot run automatically from this repository.

To reuse it with infrastructure you control, configure these secrets or environment variables:

- `WEATHERAPI_KEY`
- `FROST_BASE_URL`
- `FROST_TEMPERATURE_DATASTREAM_ID`
- `FROST_HUMIDITY_DATASTREAM_ID`
- optional `FROST_API_TOKEN`

Run without `--submit` to inspect the payloads safely. Add `--submit` only when the target FROST instance and Datastream IDs are yours.

