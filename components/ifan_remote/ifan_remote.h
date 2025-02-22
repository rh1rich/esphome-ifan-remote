#pragma once

#include <utility>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace ifan_remote {

static const uint8_t CMD_SIZE = 8;
static const uint8_t CMD_START1 = 0xAA;
static const uint8_t CMD_START2 = 0x55;

struct IFanRemoteCommand {
  uint16_t high;
  uint32_t low;
};

class IFanRemoteComponent : public uart::UARTDevice, public Component {
 public:
  void loop() override;
  void add_on_command_received_callback(std::function<void(IFanRemoteCommand)> callback) {
    this->data_callback_.add(std::move(callback));
  }

 protected:
  bool process_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint32_t rx_last_millis_{0};

  CallbackManager<void(IFanRemoteCommand)> data_callback_;
};

class IFanRemoteReceivedCommandTrigger : public Trigger<IFanRemoteCommand> {
 public:
  explicit IFanRemoteReceivedCommandTrigger(IFanRemoteComponent *parent) {
    parent->add_on_command_received_callback([this](IFanRemoteCommand data) { this->trigger(data); });
  }
};

}  // namespace ifan_remote
}  // namespace esphome
