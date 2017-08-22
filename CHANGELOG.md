# Changelog

## Unreleased
### Breaking changes
- `UPDATE_TOPIC` is now hardcoded to `${MQTT_REALM}/update`. 

### Features
- Serve device info via HTTP - use `INFO_HTTP_ENABLED = true` to enable
- Update individual devices using `${MQTT_REALM}/${DEVICE_ID}/update"` 
- Changelog added

### Fixes
