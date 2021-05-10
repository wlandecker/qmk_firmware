#include QMK_KEYBOARD_H


#define _QWERTY 0
#define _FUN 1

enum hsv_enum {
  HUE,
  SAT,
  VAL
};

enum direction {
  INCR,
  DECR
};

typedef union {
  uint32_t raw;
  struct {
    bool     hsv_change;
    uint8_t  hue;
    uint8_t  sat;
    uint8_t  val;
  };
} user_config_t;

user_config_t user_config;

// https://github.com/qmk/qmk_firmware/blob/master/quantum/rgblight_list.h#L60-L77
uint8_t default_hue = 8;
uint8_t default_sat = 255;
uint8_t default_val = 77;

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  FUN,
  HUE_UP,
  HUE_DN,
  SAT_UP,
  SAT_DN,
  VAL_UP,
  VAL_DN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LBRC, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_RBRC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_EQL,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   TG(_FUN),      TG(_FUN),KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_MINS,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    KC_LCTL, KC_LCMD, KC_SPC,                    KC_RSFT, KC_ROPT, KC_ENT
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_FUN] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_ESC,  _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_VOLU,                            KC_BRIU, _______, _______, _______, _______, KC_EQL,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, _______, KC_UP,   _______, KC_VOLD,                            KC_BRID, _______, KC_UP,   _______, _______, KC_BSLS,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     VAL_UP,  HUE_UP,  KC_LEFT, KC_DOWN, KC_RIGHT,_______,                            _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     VAL_DN,  HUE_DN,  _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, _______,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                   _______, _______, _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),
};

void keyboard_post_init_user(void) {
  // Call the post init code.
  rgblight_enable_noeeprom(); // enables Rgb, without saving settings
  rgblight_sethsv_noeeprom(0, 0, 0);
  rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}

void update_hsv(enum hsv_enum my_hsv, enum direction my_direction) {

  // First, get.
  user_config.raw = eeconfig_read_user();
  int8_t sign = 1;
  if (my_direction == DECR) {
    sign = -1;
  }

  // Next, update.
  switch (my_hsv) {
    case HUE:
      user_config.hue = user_config.hue + sign * RGBLIGHT_HUE_STEP;
      break;
    case SAT:
      user_config.sat = user_config.sat + sign * RGBLIGHT_SAT_STEP;
      break;
    case VAL:
      user_config.val = user_config.val + sign * RGBLIGHT_VAL_STEP;
      break;
  }
  
  // Next, set.
  user_config.hsv_change = true;
  rgblight_sethsv_noeeprom(user_config.hue, user_config.sat, user_config.val);

  // Finally, store.
  eeconfig_update_user(user_config.raw); 
  // uprintf("Set EEPROM to H=%u, S=%u, V=%u \n", user_config.hue, user_config.sat, user_config.val);
}

void increase_hue(void) { update_hsv(HUE, INCR); }
void decrease_hue(void) { update_hsv(HUE, DECR); }
void increase_sat(void) { update_hsv(SAT, INCR); }
void decrease_sat(void) { update_hsv(SAT, DECR); }
void increase_val(void) { update_hsv(VAL, INCR); }
void decrease_val(void) { update_hsv(VAL, DECR); }


layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
      case _FUN:
        // has the user changed their config?
        user_config.raw = eeconfig_read_user();
        if (user_config.hsv_change) {
          // if so, use the previous HSV
          rgblight_sethsv_noeeprom(user_config.hue, user_config.sat, user_config.val);
        } else {
          // otherwise, initialize the config to the defaults.
          rgblight_sethsv_noeeprom(default_hue, default_sat, default_val);
          user_config.hue = default_hue;
          user_config.sat = default_sat;
          user_config.val = default_val;
          eeconfig_update_user(user_config.raw);
        }
        break;
      default: //  for any other layers, or the default layer
        rgblight_sethsv_noeeprom(0, 0, 0);
        break;
    }
  return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case FUN:
      if (record->event.pressed) {
        layer_on(_FUN);
      } else {
        layer_off(_FUN);
      }
      return false;
      break;
    case HUE_UP:
      if (record->event.pressed) {
        increase_hue();
      }
      return false;
      break;
    case HUE_DN:
      if (record->event.pressed) {
        decrease_hue();
      }
      return false;
      break;
    case SAT_UP:
      if (record->event.pressed) {
        increase_sat();
      }
      return false;
      break;
    case SAT_DN:
      if (record->event.pressed) {
        decrease_sat();
      }
      return false;
      break;
    case VAL_UP:
      if (record->event.pressed) {
        increase_val();
      }
      return false;
      break;
    case VAL_DN:
      if (record->event.pressed) {
        decrease_val();
      }
      return false;
      break;

  }
  return true;
}