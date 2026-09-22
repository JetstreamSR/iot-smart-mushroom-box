// Reconstructed from the FROST/MongoDB mapping shown in the final presentation.
// Set DEVICE_EUI in the environment instead of committing a real device ID.

const FROST_BASE_URL = "https://gi3.gis.lrg.tum.de/frost/v1.1";

const group23MushroomMapping = {
  _id: process.env.DEVICE_EUI,
  name: "Group23 Mushroom",
  group_id: 23,
  dev_eui: process.env.DEVICE_EUI,
  datastreams: [
    {
      lpp_id: 1,
      sta_servers: [
        { sta_url: FROST_BASE_URL, datastream_iot_id: 1507 },
      ],
    },
    {
      lpp_id: 2,
      sta_servers: [
        { sta_url: FROST_BASE_URL, datastream_iot_id: 1508 },
      ],
    },
    {
      lpp_id: 3,
      sta_servers: [
        { sta_url: FROST_BASE_URL, datastream_iot_id: 1670 },
      ],
    },
    {
      lpp_id: 4,
      sta_servers: [
        { sta_url: FROST_BASE_URL, datastream_iot_id: 1671 },
      ],
    },
    {
      lpp_id: 5,
      sta_servers: [
        { sta_url: FROST_BASE_URL, datastream_iot_id: 1673 },
      ],
    },
  ],
};

module.exports = group23MushroomMapping;
