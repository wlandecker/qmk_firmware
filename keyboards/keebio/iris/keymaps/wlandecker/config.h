/*
Copyright 2017 Danny Nguyen <danny@keeb.io>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// #define USE_I2C
#define EE_HANDS

// #define BACKLIGHT_PIN B5
// #define BACKLIGHT_LEVELS 5

// #define QMK_LED B0

// /* ws2812 RGB LED */
#define RGB_DI_PIN D6
#define RGBLED_NUM 12    // Number of LEDs
#define RGBLED_SPLIT { 6, 6 }
// #define RGBLIGHT_ANIMATIONS

#define RGBLIGHT_VAL_STEP 17
#define RGBLIGHT_HUE_STEP 5
#define RGBLIGHT_SAT_STEP 10