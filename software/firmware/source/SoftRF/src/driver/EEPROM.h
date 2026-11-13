/*
 * EEPROMHelper.h
 * Copyright (C) 2016-2026 Linar Yusupov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#ifdef __cplusplus
#include "../system/SoC.h"
#endif /* __cplusplus */

#if !defined(EXCLUDE_EEPROM)
#if defined(ENERGIA_ARCH_CC13XX) || defined(ENERGIA_ARCH_CC13X2)
#include <EEPROM_CC13XX.h>
#elif defined(ARDUINO_ARCH_SAMD)
#include <FlashAsEEPROM.h>
#elif defined(RASPBERRY_PI) || defined(LUCKFOX_LYRA)
#include <raspi/EEPROM.h>
#else
#ifdef __cplusplus
#if defined(USE_EXT_EEPROM)
#include <JC_EEPROM.h>
#else
#include <EEPROM.h>
#endif /* USE_EXT_EEPROM */
#endif /* __cplusplus */
#endif /* CC13XX or CC13X2 */
#endif /* EXCLUDE_EEPROM */

#define SOFTRF_EEPROM_MAGIC   0xBABADEDA
#define SOFTRF_EEPROM_VERSION 0x00000061

/*
 * Optional Bluetooth LE services that can be switched off ("bt_svc_off" bits).
 * Default value 0 keeps every service enabled (legacy behaviour).
 * Leaving HM-10 serial alone makes a device that some EFB apps
 * (e.g. SeeYou Navigator) treat as a plain NMEA source.
 */
#define BT_SVC_OFF_NUS        0x01  /* Nordic UART service      */
#define BT_SVC_OFF_SENSBOX    0x02  /* SensBox service          */
#define BT_SVC_OFF_MIDI       0x04  /* MIDI over Bluetooth LE   */
#define BT_SVC_OFF_MASK       (BT_SVC_OFF_NUS | BT_SVC_OFF_SENSBOX | BT_SVC_OFF_MIDI)

/* GNSS fix indication on a status LED ("led_gnss") */
enum
{
	LED_GNSS_NONE,      /* legacy behaviour */
	LED_GNSS_NOFIX_RED, /* short red flash every 2 seconds while there is no fix */
	LED_GNSS_COUNT
};

/*
 * Miscellaneous user options ("ext_opts" bits).
 * Default value 0 keeps the legacy behaviour.
 */
#define EXT_SND_OFF_TRAFFIC   0x01  /* no beep when a new traffic is received */
#define EXT_SND_OFF_STARTUP   0x02  /* no power-on / power-off tones          */
#define EXT_SND_OFF_FIX       0x04  /* no melody on very first GNSS fix       */
#define EXT_SND_OFF_MASK      (EXT_SND_OFF_TRAFFIC | EXT_SND_OFF_STARTUP | EXT_SND_OFF_FIX)
#define EXT_NMEA_ID_PLAIN     0x08  /* $PFLAA: no "!callsign" substitute      */
#define EXT_OPTS_MASK         (EXT_SND_OFF_MASK | EXT_NMEA_ID_PLAIN)

enum
{
	EEPROM_EXT_LOAD,
	EEPROM_EXT_DEFAULTS,
	EEPROM_EXT_STORE
};

typedef struct Settings {
    uint8_t  mode;
    uint8_t  rf_protocol;
    uint8_t  band;
    uint8_t  aircraft_type;
    uint8_t  txpower;
    uint8_t  volume;
    uint8_t  led_num;
    uint8_t  pointer;

    bool     nmea_g:1;
    bool     nmea_p:1;
    bool     nmea_l:1;
    bool     nmea_s:1;
    bool     resvd1:1;
    uint8_t  nmea_out:3;

    uint8_t  bluetooth:3; /* ESP32 built-in Bluetooth */
    uint8_t  alarm:3;
    bool     stealth:1;
    bool     no_track:1;

    uint8_t  gdl90:3;
    uint8_t  d1090:3;
    uint8_t  json:2;

    uint8_t  power_save;

    int8_t   freq_corr; /* +/-, kHz */
    uint8_t  bt_svc_off; /* BT_SVC_OFF_* bits, 0 = all services enabled */
    uint8_t  led_gnss;   /* LED_GNSS_*, 0 = no GNSS fix indication      */
    uint8_t  ext_opts;   /* EXT_* bits, 0 = legacy behaviour            */

    /* Use a key provided by (local) gliding contest organizer */
    uint32_t igc_key[4];

} __attribute__((packed)) settings_t;

typedef struct EEPROM_S {
    uint32_t  magic;
    uint32_t  version;
    settings_t settings;
} eeprom_struct_t;

typedef union EEPROM_U {
   eeprom_struct_t field;
   uint8_t raw[sizeof(eeprom_struct_t)];
} eeprom_t;

void EEPROM_setup(void);
void EEPROM_defaults(void);
void EEPROM_store(void);
extern settings_t *settings;

#endif /* EEPROMHELPER_H */
