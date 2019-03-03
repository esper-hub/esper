# Changelog

## Unreleased
### Breaking changes
- `PulseFeature` has ben renamed to `Pulse`
- `UPDATE_TOPIC` is now hardcoded to `${MQTT_REALM}/update`.
- Docker file moved to project root.
- Build output moved from esper directory to site directory.

### Features
- Added `${MQTT_REALM}/${DEVICE_ID}/status` containing `ONLINE` or `OFFLINE`
- Serve device info via HTTP - use `INFO_HTTP_ENABLED = true` to enable
- Update individual devices using `${MQTT_REALM}/${DEVICE_ID}/update` 
- Changelog added

### Fixes
