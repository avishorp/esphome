import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_RESET_PIN
from esphome import pins

CONF_SENSITIVITY = "sensitivity"
CONF_ALLOW_MULTIPLE_TOUCHES = "allow_multiple_touches"

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["binary_sensor", "output"]
CODEOWNERS = ["@avishorp"]

CONF_CAP1114_ID = "cap1114_id"
CONF_MIN_BRIGHTNESS = "min_brightness"
CONF_MAX_BRIGHTNESS = "max_brightness"

cap1114_ns = cg.esphome_ns.namespace("cap1114")
CAP1114Component = cap1114_ns.class_("CAP1114Component", cg.Component, i2c.I2CDevice)

MULTI_CONF = True
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CAP1114Component),
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_SENSITIVITY, default=5): cv.int_range(
                min=0, max=7
            ),
            cv.Optional(CONF_MIN_BRIGHTNESS): cv.int_range(0, 15),
            cv.Optional(CONF_MAX_BRIGHTNESS): cv.int_range(0, 15),
            cv.Optional(CONF_ALLOW_MULTIPLE_TOUCHES, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x28))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_sensitivity(config[CONF_SENSITIVITY]))
    cg.add(var.set_allow_multiple_touches(config[CONF_ALLOW_MULTIPLE_TOUCHES]))

    if CONF_RESET_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(pin))
    
    if CONF_MIN_BRIGHTNESS in config:
        cg.add(var.set_min_brightness(config[CONF_MIN_BRIGHTNESS]))

    if CONF_MAX_BRIGHTNESS in config:
        cg.add(var.set_max_brightness(config[CONF_MAX_BRIGHTNESS]))

    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
