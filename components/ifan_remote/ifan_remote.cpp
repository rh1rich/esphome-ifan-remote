#include "ifan_remote.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <cstring>

namespace esphome {
namespace ifan_remote {

static const char *const TAG = "ifan_remote";

bool IFanRemoteComponent::process_byte_(uint8_t byte) {
  size_t pos = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  ESP_LOGVV(TAG, "Processing byte: 0x%02X", byte);

  // Byte 0-1: Command start
  if (pos == 0)
    return byte == CMD_START1;
  if (pos == 1) {
    ESP_LOGV(TAG, "Detected command start (0x%02X%02X)", CMD_START1, CMD_START2);
    return byte == CMD_START2;
  }

  // Byte 2-6: Command
  if (pos < (CMD_SIZE - 1))
    return true;

  // Byte 7: Checksum
  uint8_t chksum = raw[2] + raw[3] + raw[4] + raw[5] + raw[6];
  IFanRemoteCommand command;
  command.high = (raw[2] << 8) | raw[3];
  command.low = (raw[4] << 16) | (raw[5] << 8) | raw[6];
  if (chksum == raw[7]) {
    ESP_LOGD(TAG, "Received command %04X:%06X", command.high, command.low);
    this->data_callback_.call(command);
  }
  else {
    ESP_LOGW(TAG, "Received command %04X:%06X with invalid checksum %02X. Should be %02X.", command.high, command.low, raw[7], chksum);
  }

  // return false to reset buffer
  return false;
}

void IFanRemoteComponent::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (millis() - this->rx_last_millis_ > 50) {
      this->rx_buffer_.clear();
    }
    if (!this->process_byte_(byte)) {
      this->rx_buffer_.clear();
    }
    this->rx_last_millis_ = millis();
  }
}

}  // namespace ifan_remote
}  // namespace esphome
