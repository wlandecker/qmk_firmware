#include QMK_KEYBOARD_H

#if __has_include("keymap.h")
# include "keymap.h"
#endif

/* Reconfigured specifically for the 3x5 layout variant */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
      [0] = LAYOUT(
        KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T,                  KC_Y, KC_U, KC_I,    KC_O,   KC_P,    KC_DEL, 
        KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G,                  KC_H, KC_J, KC_K,    KC_L,   KC_SCLN, KC_QUOT, 
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,  TT(1), TT(2),   KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, 
                MT(MOD_LCTL,KC_BSPC), KC_LGUI, KC_SPC, KC_RSFT, KC_RALT, KC_ENT
                ),
      
      [1] = LAYOUT(
        KC_GRV,  KC_ESC,  KC_TRNS, RM_VALU, KC_BRIU, KC_VOLU,                     KC_PGUP, KC_TRNS, KC_UP,   KC_TRNS, KC_HOME, KC_BSPC, 
        KC_ESC,  KC_TAB,  KC_TRNS, RM_VALD, KC_BRID, KC_VOLD,                     KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_END,  S(KC_BSLS), 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
      
      [2] = LAYOUT(
        KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,       KC_5,                             KC_6,     KC_7,      KC_8,   KC_9,    KC_0,    KC_TRNS, 
        KC_TRNS, KC_GRV,  KC_TRNS, KC_MINS, S(KC_COMM), KC_LBRC,                          KC_RBRC,  S(KC_DOT), KC_EQL, KC_BSLS, KC_QUOT, KC_BSLS, 
        KC_MUTE, KC_MINS, KC_TRNS, KC_TRNS, KC_TRNS,    S(KC_COMM),   KC_TRNS, KC_TRNS,   S(KC_DOT), KC_TRNS, KC_TRNS, KC_TRNS, KC_EQL,  KC_TRNS, 
                                                    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),

};

#ifdef OTHER_KEYMAP_C
# include OTHER_KEYMAP_C
#endif

void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    // This is QMK's persisted RGB Matrix brightness value, 0-255.
    uint8_t val = rgb_matrix_get_val();

    switch (get_highest_layer(layer_state)) {
        case 1:
            // Burnt orange: #CC5500, scaled by persisted brightness.
            r = ((uint16_t)204 * val) / 255;
            g = ((uint16_t)85  * val) / 255;
            b = 0;
            break;

        case 2:
            // Teal: #008080, scaled by persisted brightness.
            r = 0;
            g = ((uint16_t)128 * val) / 255;
            b = ((uint16_t)128 * val) / 255;
            break;

        default:
            // Layer 0: always off, regardless of saved RGB brightness.
            r = 0;
            g = 0;
            b = 0;
            break;
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, r, g, b);
    }

    return false;
}
