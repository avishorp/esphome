#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include <vector>

namespace esphome {
namespace cap1114 {


enum {
  CAP1114_I2CADDR = 0x28,
  CAP1114_PRODUCT_ID = 0x3a,
  CAP1114_MANUFACTURER_ID = 0x5d,
  CAP1114_REG_PRODUCT_ID = 0xfd,
  CAP1114_REG_REVISION = 0xff,
  CAP1114_REG_MFR_ID = 0xfe,
  CAP1114_REG_LEDDIR = 0x70,
  CAP1114_REG_LEDCTRL1 = 0x73,
  CAP1114_REG_LEDCTRL2 = 0x74,
  CAP1114_REG_BTNSTATUS = 0x03,
  CAP1114_REG_MAINSTAT = 0x00,
  CAP1114_REG_DRDUTY = 0x93,
  CAP1114_REG_SENSITVITY = 0x1f,
  CAP1114_REG_CONFIG2 = 0x40,
  CAP1114_REG_MULTITOUCH = 0x2a,
  CAP1114_REG_LED_DIR = 0x70,
  CAP1114_REG_OUTPUT_TYPE = 0x71,
  CAP1114_REG_LED_OUTPUT = 0x73
};

class CAP1114Component;

class CAP1114TouchChannel : public binary_sensor::BinarySensor {
 public:
  void set_channel(uint8_t channel) { channel_ = channel; }
  uint8_t get_channel() const { return channel_; }
  void process(uint8_t data) { this->publish_state(static_cast<bool>(data & (1 << this->channel_))); }

 protected:
  uint8_t channel_{0};
};

class CAP1114OutputChannel : public output::BinaryOutput, public Component {
  public:
    void set_channel(uint8_t channel);
    void set_parent(CAP1114Component* parent) { cap1114_ = parent; }
    uint8_t get_channel() const { return channel_; }
    void set_open_drain(bool open_drain);
    virtual void write_state(bool state);

  protected:
    uint8_t channel_{0};
    optional<CAP1114Component*> cap1114_;
};

class CAP1114Component : public Component, public i2c::I2CDevice {
 public:
  void register_touch_channel(CAP1114TouchChannel *channel) { this->touch_channels_.push_back(channel); }
  void register_output_channel(CAP1114OutputChannel *channel) { channel->set_parent(this); }
  void set_sensitivity(uint8_t sensitivity) { this->sensitivity_ = sensitivity; };
  void set_allow_multiple_touches(bool allow_multiple_touches) {
    this->allow_multiple_touches_ = allow_multiple_touches ? 0x41 : 0x80;
  };
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void loop() override;

 protected:
  std::vector<CAP1114TouchChannel *> touch_channels_{};
  std::vector<CAP1114OutputChannel *> output_channels_{};
  uint8_t sensitivity_{5};
  uint8_t allow_multiple_touches_{0x80};

  GPIOPin *reset_pin_{nullptr};

  uint8_t cap1114_product_id_{0};
  uint8_t cap1114_manufacture_id_{0};
  uint8_t cap1114_revision_{0};
  bool cleared_{false};

  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
  } error_code_{NONE};
};

}  // namespace cap1114
}  // namespace esphome
