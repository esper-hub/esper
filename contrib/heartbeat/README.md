This systemd unit provides a timer for the heartbeat functionality.

Requirements
============
`mosquitto_pub` from:
- mosquitto-clients (Debian)
- mosquitto (Archlinux)

Usage
=====
Deploy `esper-heartbeat.service` and `esper-heartbeat.timer` to `/etc/systemd/system`.

By default it waits until the `mosquitto.service` is started. If you are using a different broker, or the broker is not running locally, change that accordingly.

Also change `host` and `topic` to match your site configuration.

Finally enable the timer:
```shell
systemctl enable --now esper-heartbeat.timer
```

It will now issue heartbeats every minute.
