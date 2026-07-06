#include QMK_KEYBOARD_H

#if __has_include("keymap.h")
# include "keymap.h"
#endif

#define CTL_BSP MT(MOD_LCTL, KC_BSPC)

#define CMD_NAV LGUI_T(KC_NO)
#define OPT_SYM LALT_T(KC_NO)

enum layers {
    _BASE = 0,
    _NAV,
    _SYM
};

static void move_between_base_and(uint8_t target_layer) {
    if (get_highest_layer(layer_state) == target_layer) {
        layer_move(_BASE);
    } else {
        layer_move(target_layer);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CMD_NAV:
            if (record->tap.count) {
                if (record->event.pressed) {
                    move_between_base_and(_NAV);
                }
                return false;  // Don't let KC_NO get processed.
            }
            return true;       // Hold behavior: normal LGUI mod-tap.

        case OPT_SYM:
            if (record->tap.count) {
                if (record->event.pressed) {
                    move_between_base_and(_SYM);
                }
                return false;  // Don't let KC_NO get processed.
            }
            return true;       // Hold behavior: normal LALT mod-tap.
    }

    return true;
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CMD_NAV:
        case OPT_SYM:
            return true;
    }
    return false;
}

/* Reconfigured specifically for the 3x5 layout variant */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
      /* BASE layer */
      [0] = LAYOUT(
        KC_NO,  KC_Q,   KC_W,   KC_E,   KC_R,    KC_T,                            KC_Y,    KC_U,  KC_I,    KC_O,   KC_P,    KC_NO, 
        KC_NO,  KC_A,   KC_S,   KC_D,   KC_F,    KC_G,                            KC_H,    KC_J,  KC_K,    KC_L,   KC_SCLN, KC_NO, 
        KC_NO,  KC_Z,   KC_X,   KC_C,   KC_V,    KC_B,    TO(0),         TO(0),   KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH, KC_NO, 
                                        CTL_BSP, CMD_NAV, KC_SPC,        KC_RSFT, OPT_SYM, KC_ENT
                ),
      
      /* NAV layer */
      [1] = LAYOUT(
        _______, KC_ESC, KC_NO, RM_VALU, KC_BRIU, KC_VOLU,                       KC_PGUP, KC_NO,   KC_UP,   KC_NO,   KC_HOME, _______, 
        _______, KC_TAB, KC_NO, RM_VALD, KC_BRID, KC_VOLD,                       KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_END,  _______, 
        _______, KC_NO,  KC_NO, KC_NO,   KC_NO,   KC_NO,   _______,     _______, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   _______, 
                                         _______, _______, _______,     _______, _______, _______),
    
      /* SYM/NUM layer */
      [2] = LAYOUT(
        _______, KC_1,    KC_2,  KC_3,    KC_4,    KC_5,                          KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    _______, 
        _______, KC_GRV,  KC_NO, KC_MINS, KC_LABK, KC_LBRC,                       KC_RBRC,  KC_RABK, KC_EQL,  KC_BSLS, KC_QUOT, _______, 
        _______, KC_MINS, KC_NO, KC_NO,   KC_NO,   KC_RABK, _______,     _______, KC_RABK,  KC_NO,   KC_NO,   KC_NO,   KC_EQL,  _______, 
                                          _______, _______, _______,     _______, _______, _______),

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
