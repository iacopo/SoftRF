# `$PSRFX` – extra settings (nRF52)

`$PSRFX` is an additional NMEA configuration sentence next to `$PSRFC`
(main settings) and `$PSRFS` (IGC key). It controls options that do not
exist in `$PSRFC`, so the `$PSRFC` format and every tool that parses it
stay unchanged.

All values are stored in bytes of `settings_t` that used to be reserved.
**`0` always means legacy behaviour**, and out-of-range values (for
example erased flash `0xFF` left by older firmware) are treated as `0`,
so upgrading never changes how a device behaves.

The sentence is accepted on any NMEA input (USB, Bluetooth LE, UART).
Like `$PSRFC`, applying new values stores them and restarts the device.

## Syntax

```
$PSRFX,?*<cs>                                              query
$PSRFX,1,<bt_svc_off>,<led_gnss>[,<snd_off>[,<nmea_id>[,<led_proto>]]]*<cs>  set
```

The query answers with `$PSRFX,1,<bt_svc_off>,<led_gnss>,<snd_off>,<nmea_id>,<led_proto>`.
When setting, trailing fields may be omitted and empty fields keep their
current value.

| Field        | Values | Meaning |
|--------------|--------|---------|
| `bt_svc_off` | bit mask | Bluetooth LE services to leave out: `1` Nordic UART, `2` SensBox, `4` MIDI. The HM-10 serial service (FFE0/FFE1) is always present. |
| `led_gnss`   | `0`, `1` | `1`: short red LED flash every 2 s while there is no GNSS fix. The green status LED is paused during the flash so a single RGB LED shows pure red. |
| `snd_off`    | bit mask | Buzzer events to mute: `1` new traffic beep, `2` power-on and power-off tones, `4` first GNSS fix melody. Traffic alarms keep following `volume`. |
| `nmea_id`    | `0`, `1` | `0`: `$PFLAA` ID field carries a callsign or a substitute (`200979!FLR_200979`). `1`: plain ID (`200979`) unless a real callsign is known (ADS-B flight number). |
| `led_proto`  | `0`, `1` | `1`: on boards with an RGB status LED the power colour tells the radio protocol: green FANET, blue FLARM, cyan any other. The blue LED then no longer shows Bluetooth status. Single colours keep the LED current unchanged. |

## Why

* Some EFB apps (SeeYou Navigator on iOS) ignore a device that exposes
  SensBox / Nordic UART / MIDI next to HM-10 and never read its NMEA
  stream. With `bt_svc_off = 7` the device looks like a plain HM-10
  NMEA source and works.
* Before, BLE MIDI was tied to the buzzer volume: the only way to drop
  the MIDI service was to silence the buzzer. `bt_svc_off` bit `4`
  removes MIDI while the buzzer keeps working.
* In crowded competitions the "new traffic" beep fires very often;
  `snd_off` bit `1` mutes only that event.
* With the device out of sight in the harness pocket a glance before
  take-off tells the protocol: `led_proto = 1`.
* Apps that do not understand the `!callsign` suffix show
  `FLR200979!FLR_200979`; `nmea_id = 1` gives `FLR200979`, the same as a
  FLARM device.

## Example

SeeYou Navigator setup: HM-10 only, red no-fix flash, no new-traffic
beep, plain IDs:

```
$PSRFX,1,7,1,1,1*54
```

Query: `$PSRFX,?*5C`. Back to legacy behaviour: `$PSRFX,1,0,0,0,0*52`.

The checksum is the XOR of all characters between `$` and `*`, in hex.
