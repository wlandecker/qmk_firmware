#include QMK_KEYBOARD_H

enum layers {
    _BASE,
    _MODS,
    _NAV,
    _SYM,
};

enum custom_keycodes {
    CLEAR_OS = SAFE_RANGE,
    APP_SW,
};

// Thumb key: tap = Backspace, hold = _MODS layer
#define MOD_BSPC LT(_MODS, KC_BSPC)
#define NAV_SPC LT(_NAV, KC_SPC)
#define SYM_ENT LT(_SYM, KC_ENT)

// Optional aliases to make the keymap easier to read
#define OS_CMD  OS_LGUI
#define OS_OPT  OS_LALT
#define OS_SFT  OS_LSFT
#define OS_CTL  OS_LCTL
#define CMD(kc) LGUI(kc)

// App switch
#define APP_SWITCH_TIMEOUT 1000

static bool app_switch_active = false;
static uint16_t app_switch_timer = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /*
     * Ferris/Sweep-style 34-key layout:
     *
     * ,-----------------------------.                  ,-----------------------------.
     * |  Q  |  W  |  E  |  R  |  T  |                  |  Y  |  U  |  I  |  O  |  P  |
     * |-----+-----+-----+-----+-----|                  |-----+-----+-----+-----+-----|
     * |  A  |  S  |  D  |  F  |  G  |                  |  H  |  J  |  K  |  L  |  ;  |
     * |-----+-----+-----+-----+-----|                  |-----+-----+-----+-----+-----|
     * |  Z  |  X  |  C  |  V  |  B  |                  |  N  |  M  |  ,  |  .  |  /  |
     * `-----------------------------'                  `-----------------------------'
     *               | mod/bsps | nav/spc |        | shft | sym/ent |
     *               `--------------------'        `----------------'
     */

    [_BASE] = LAYOUT_split_3x5_2(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,

                          MOD_BSPC, NAV_SPC,              KC_RSFT,  SYM_ENT
    ),

    /*
     * _MODS layer:
     *
     * Hold MOD_BSPC to access this layer.
     *
     * One-shot mods:
     *   OS_CMD = Command on macOS
     *   OS_OPT = Option/Alt
     *   OS_SFT = Shift
     *   OS_CTL = Control
     *
     * Repeat:
     *   QK_REP  = repeat last key, including mods
     *   QK_AREP = alternate repeat, e.g. Right -> Left, Down -> Up
     *
     * CLEAR_OS clears stuck/mistaken one-shot mods.
     */

    [_MODS] = LAYOUT_split_3x5_2(
        CLEAR_OS,  CMD(KC_W),     KC_NO,     KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,
	    OS_CTL,        KC_NO,    OS_OPT,    OS_CMD, QK_AREP,     QK_REP,  OS_CMD,  OS_OPT,  KC_NO,   OS_CTL,
        CMD(KC_Z), CMD(KC_X), CMD(KC_C), CMD(KC_V),   KC_NO,      KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,

                          _______, _______,              _______, _______
    ),

    /*
     * _NAV layer stub.
     * Fill this however you already planned.
     */

    [_NAV] = LAYOUT_split_3x5_2(
        KC_ESC,  KC_NO,   KC_NO,   KC_NO,   KC_VOLU,     KC_PGUP,   KC_NO,   KC_UP,   KC_NO,    KC_HOME,
        KC_TAB,  KC_NO,   KC_NO,   APP_SW,  KC_VOLD,     KC_PGDN,   KC_LEFT, KC_DOWN, KC_RIGHT, KC_END,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,       KC_NO,     KC_NO,   KC_NO,   KC_NO,    KC_NO,

                          _______, _______,              _______, _______
    ),

    /*
     * _SYM layer stub.
     * Left outer thumb becomes shift to enable shift-symbols
     */

    [_SYM] = LAYOUT_split_3x5_2(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        KC_TILD, KC_NO,   KC_MINS, KC_NO,   KC_LBRC,     KC_RBRC, KC_NO,   KC_EQUAL,KC_BSLS, KC_QUOTE,
        KC_MINS, KC_NO,   KC_NO,   KC_NO,   KC_LABK,     KC_RABK, KC_NO,   KC_NO,   KC_NO,   KC_EQUAL,

                          _______, KC_LSFT,              _______, _______
    ),
};

// Shift + Volume Up -> Brightness Up
const key_override_t volu_to_briu = ko_make_basic(
    MOD_MASK_SHIFT,
    KC_VOLU,
    KC_BRIU
);

// Shift + Volume Down -> Brightness Down
const key_override_t vold_to_brid = ko_make_basic(
    MOD_MASK_SHIFT,
    KC_VOLD,
    KC_BRID
);

const key_override_t *key_overrides[] = {
    &volu_to_briu,
    &vold_to_brid,
    NULL
};

static bool shift_is_active(void) {
    uint8_t mods = get_mods() | get_oneshot_mods();
    return mods & MOD_MASK_SHIFT;
}

static void app_switch_release(void) {
    if (app_switch_active) {
        unregister_mods(MOD_BIT(KC_LGUI));
        app_switch_active = false;
    }
}

static void app_switch_trigger(bool reverse) {
    if (!app_switch_active) {
        register_mods(MOD_BIT(KC_LGUI));
        app_switch_active = true;
    }

    app_switch_timer = timer_read();

    if (reverse) {
        register_mods(MOD_BIT(KC_LSFT));
        tap_code(KC_TAB);
        unregister_mods(MOD_BIT(KC_LSFT));
    } else {
        tap_code(KC_TAB);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CLEAR_OS:
            if (record->event.pressed) {
                clear_oneshot_mods();
                reset_oneshot_layer();
                app_switch_release();
            }
            return false;

        case APP_SW:
            if (record->event.pressed) {
                bool reverse = shift_is_active();

                /*
                 * If Shift was one-shot, consume it here so it does not leak
                 * into the next normal keypress after app switching.
                 */
                clear_oneshot_mods();

                app_switch_trigger(reverse);
            }
            return false;
    }

    return true;
}

void matrix_scan_user(void) {
    if (app_switch_active && timer_elapsed(app_switch_timer) > APP_SWITCH_TIMEOUT) {
        app_switch_release();
    }
}

/*
 * Optional: prevent tapped Backspace from becoming the remembered Repeat Key.
 *
 * Without this, tapping Backspace and then QK_REP will produce another Backspace.
 * With this, Backspace is ignored by Repeat Key's memory.
 */
bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    switch (keycode) {
        case KC_BSPC:
            return false;
    }

    return true;
}
