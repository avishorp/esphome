#include "cap1114.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace cap1114 {

static const char *const TAG = "cap1114";

void CAP1114Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up CAP1114...");

  // Reset device using the reset pin
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(false);
    delay(100);  // NOLINT
    this->reset_pin_->digital_write(true);
    delay(100);  // NOLINT
    this->reset_pin_->digital_write(false);
    delay(100);  // NOLINT
  }

  // Check if CAP1114 is actually connected
  this->read_byte(CAP1114_REG_PRODUCT_ID, &this->cap1114_product_id_);
  this->read_byte(CAP1114_REG_MFR_ID, &this->cap1114_manufacture_id_);
  this->read_byte(CAP1114_REG_REVISION, &this->cap1114_revision_);

  if (/*(this->cap1114_product_id_ != CAP1114_PRODUCT_ID) ||*/ (this->cap1114_manufacture_id_ != CAP1114_MANUFACTURER_ID)) {
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  // Set sensitivity
  // This value sets the DELTA_SENSE field in the sensitivity register. The
  // actual value written in 7-(configured value), in order to make it more
  // sensible from the user perspective: 0 - least sensitive, 7 - most sensitive
  uint8_t sensitivity;
  this->read_byte(CAP1114_REG_SENSITVITY, &sensitivity);
  this->write_byte(CAP1114_REG_SENSITVITY, (sensitivity & 0x70) | ((7-this->sensitivity_) << 4)); // Sensitivity

  // Set LED brightness
  this->write_byte(CAP1114_REG_LEDDIR, 0xff);  // LED/GPIO Direction

  // Setup the chip
  this->write_byte(CAP1114_REG_LEDDIR, 0xff);  // LED/GPIO Direction

  this->write_byte(CAP1114_REG_CONFIG2, 0x2a);
  this->write_byte(CAP1114_REG_MULTITOUCH, 0x8c);
}

void CAP1114Component::dump_config() {
  ESP_LOGCONFIG(TAG, "CAP1114:");
  LOG_I2C_DEVICE(this);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Product ID: 0x%x", this->cap1114_product_id_);
  ESP_LOGCONFIG(TAG, "  Manufacture ID: 0x%x", this->cap1114_manufacture_id_);
  ESP_LOGCONFIG(TAG, "  Revision ID: 0x%x", this->cap1114_revision_);

  for(auto tc : this->touch_channels_) {
    ESP_LOGCONFIG(TAG, " Touch Channel %d", tc->get_channel());
  }

  switch (this->error_code_) {
    case COMMUNICATION_FAILED:
      ESP_LOGE(TAG, "Product ID or Manufacture ID of the connected device does not match a known CAP1114.");
      break;
    case NONE:
    default:
      break;
  }
}

void CAP1114Component::loop() {
  uint16_t button_status;
  this->read_byte_16(CAP1114_REG_BTNSTATUS, &button_status);
  uint8_t status;
  this->read_byte(0, &status);

  if (button_status) {
    this->write_byte(CAP1114_REG_MAINSTAT, 0);
    // ESP_LOGCONFIG(TAG, " Touch %x %x", button_status, status);

    for (auto *channel : this->touch_channels_) {
      channel->process(button_status);
    }
    cleared_ = false;
  }
  else {
    if (!cleared_) {
      for (auto *channel : this->touch_channels_) {
        channel->process(0);
      }
      cleared_ = true;
    }
  }
}

void CAP1114OutputChannel::write_state(bool state) {
    uint16_t outputs;
    (*cap1114_)->read_byte_16(CAP1114_REG_LED_OUTPUT, &outputs);

    if (state) {
      outputs |= (1 << channel_);
    }
    else {
      outputs &= ~(1 << channel_);
    }
    (*cap1114_)->write_byte_16(CAP1114_REG_LED_OUTPUT, outputs);
}

void CAP1114OutputChannel::set_channel(uint8_t channel) {
  if (cap1114_ && channel < 8) {
    (*cap1114_)->write_byte(CAP1114_REG_LED_DIR, 1 << channel);
  }
}

void CAP1114OutputChannel::set_open_drain(bool open_drain) {
  if (cap1114_ && channel_ < 8) {
    (*cap1114_)->write_byte(CAP1114_REG_OUTPUT_TYPE, 1 << channel_);
  }
}

}  // namespace cap1114
}  // namespace esphome
