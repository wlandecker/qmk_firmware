#include QMK_KEYBOARD_H

#if __has_include("keymap.h")
# include "keymap.h"
#endif

#define CTL_BSP MT(MOD_LCTL, KC_BSPC)
#define SFT_SPC MT(MOD_LSFT, KC_SPC)

#define CMD_NAV LGUI_T(KC_NO)
#define OPT_SYM LALT_T(KC_NO)
#define SHIFT_MACRO LSFT_T(KC_NO)

enum layers {
    _BASE = 0,
    _NAV,
    _SYM,
    _MACRO
};

enum custom_keycodes {
    APP_SW = SAFE_RANGE,
    WIN_SW
};

#define APP_SWITCH_TIMEOUT 1000

static bool app_switch_active = false;
static bool app_switch_registered_gui = false;
static uint16_t app_switch_timer = 0;

static bool shift_is_active(void) {
    return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
}

static bool physical_shift_is_active(void) {
    return get_mods() & MOD_MASK_SHIFT;
}

static void finish_app_switch(void) {
    if (app_switch_active && app_switch_registered_gui) {
        unregister_code(KC_LGUI);
    }

    app_switch_active = false;
    app_switch_registered_gui = false;
}

static void tap_key_maybe_shifted(uint16_t keycode, bool shifted) {
    // A utility function to send the shifted keycode of a mod-tap's tap key
    if (shifted) {
        if (physical_shift_is_active()) {
            // Real Shift is already down, so just tap the key.
            tap_code(keycode);
        } else {
            // One-shot Shift or logical Shift: send Shift+key manually.
            tap_code16(S(keycode));
            clear_oneshot_mods();
        }
    } else {
        tap_code(keycode);
    }
}

static void app_switch_step(void) {
    // A macro for switching apps on Mac OS
    bool reverse = shift_is_active();

    if (!app_switch_active) {
        app_switch_registered_gui = !(get_mods() & MOD_MASK_GUI);

        if (app_switch_registered_gui) {
            register_code(KC_LGUI);
        }

        app_switch_active = true;
    }

    tap_key_maybe_shifted(KC_TAB, reverse);
    app_switch_timer = timer_read();
}

static void window_cycle_step(void) {
    // A macro for cycling through windows on Mac OS
    bool reverse = shift_is_active();
    bool gui_was_already_down = get_mods() & MOD_MASK_GUI;

    // Avoid leaving the app-switch modal state active if this key is used next.
    finish_app_switch();

    if (!gui_was_already_down) {
        register_code(KC_LGUI);
    }

    tap_key_maybe_shifted(KC_GRV, reverse);

    if (!gui_was_already_down) {
        unregister_code(KC_LGUI);
    }
}

static void move_between_base_and(uint8_t target_layer) {
    if (get_highest_layer(layer_state) == target_layer) {
        layer_move(_BASE);
    } else {
        layer_move(target_layer);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {

        case APP_SW:
            if (record->event.pressed) {
                app_switch_step();
            }
            return false;

        case WIN_SW:
            if (record->event.pressed) {
                window_cycle_step();
            }
            return false;

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

        case SHIFT_MACRO:
            if (record->tap.count) {
                if (record->event.pressed) {
                    move_between_base_and(_MACRO);
                }
                return false;  // Don't let KC_NO get processed.
            }
            return true;       // Hold behavior: normal LSFT mod-tap.
    }

    return true;
}

void matrix_scan_user(void) {
    if (app_switch_active && timer_elapsed(app_switch_timer) > APP_SWITCH_TIMEOUT) {
        finish_app_switch();
    }
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CMD_NAV:
        case OPT_SYM:
        case SHIFT_MACRO:
            return true;
    }
    return false;
}

/* Reconfigured specifically for the 3x5 layout variant */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
      /* BASE layer */
      [0] = LAYOUT(
        KC_NO,  KC_Q,   KC_W,   KC_E,   KC_R,    KC_T,                                KC_Y,    KC_U,  KC_I,    KC_O,   KC_P,    KC_NO, 
        KC_NO,  KC_A,   KC_S,   KC_D,   KC_F,    KC_G,                                KC_H,    KC_J,  KC_K,    KC_L,   KC_QUOT, KC_NO, 
        KC_NO,  KC_Z,   KC_X,   KC_C,   KC_V,    KC_B,    TO(0),         TO(0),       KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH, KC_NO, 
                                        CTL_BSP, CMD_NAV, KC_SPC,        SHIFT_MACRO, OPT_SYM, KC_ENT
                ),
      
      /* NAV layer */
      [1] = LAYOUT(
        _______, KC_ESC, KC_NO, RM_VALU, KC_BRIU, KC_VOLU,                       KC_PGUP, KC_NO,   KC_UP,   KC_NO,   KC_HOME, _______, 
        _______, KC_TAB, KC_NO, RM_VALD, KC_BRID, KC_VOLD,                       KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_END,  _______, 
        _______, KC_NO,  KC_NO, KC_NO,   KC_NO,   KC_NO,   _______,     _______, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   _______, 
                                         _______, _______, SFT_SPC,     _______, _______, _______),
    
      /* SYM/NUM layer */
      [2] = LAYOUT(
        _______, KC_1,    KC_2,  KC_3,    KC_4,    KC_5,                          KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    _______, 
        _______, KC_GRV,  KC_NO, KC_MINS, KC_LABK, KC_LBRC,                       KC_RBRC,  KC_RABK, KC_EQL,  KC_BSLS, KC_SCLN, _______, 
        _______, KC_MINS, KC_NO, KC_NO,   KC_NO,   KC_RABK, _______,     _______, KC_RABK,  KC_NO,   KC_NO,   KC_NO,   KC_EQL,  _______, 
                                          _______, _______, _______,     _______, _______, _______),

      /* MACROCUTS layer */
      [3] = LAYOUT(
        _______, KC_NO,   KC_NO, KC_NO,   KC_NO,   KC_NO,                         KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, _______, 
        _______, KC_NO,   KC_NO, KC_NO,   WIN_SW,  APP_SW,                        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, _______, 
        _______, KC_NO,   KC_NO, KC_NO,   KC_NO,   KC_NO,   _______,     _______, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, _______, 
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

        case 3:
            // salmon: ##ff7a7a, scaled by persisted brightness.
            r = ((uint16_t)255 * val) / 255;
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
