import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import (
    CONF_CODE,
    CONF_HIGH,
    CONF_ID,
    CONF_LENGTH,
    CONF_LOW,
    CONF_PROTOCOL,
    CONF_RAW,
    CONF_SYNC,
    CONF_TRIGGER_ID,
    CONF_DURATION,
)

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@rh1rich"]

ifan_remote_ns = cg.esphome_ns.namespace("ifan_remote")
IFanRemoteComponent = ifan_remote_ns.class_(
    "IFanRemoteComponent", cg.Component, uart.UARTDevice
)

IFanRemoteCommand = ifan_remote_ns.struct("IFanRemoteCommand")

IFanRemoteReceivedCommandTrigger = ifan_remote_ns.class_(
    "IFanRemoteReceivedCommandTrigger", automation.Trigger.template(IFanRemoteCommand)
)

CONF_ON_COMMAND_RECEIVED = "on_command"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(IFanRemoteComponent),
            cv.Optional(CONF_ON_COMMAND_RECEIVED): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        IFanRemoteReceivedCommandTrigger
                    ),
                }
            ),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for conf in config.get(CONF_ON_COMMAND_RECEIVED, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(IFanRemoteCommand, "command")], conf)
