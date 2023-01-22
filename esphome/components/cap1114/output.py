import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.output import BinaryOutput, BINARY_OUTPUT_SCHEMA, register_output
from esphome.const import CONF_CHANNEL, CONF_ID, CONF_OPEN_DRAIN
from . import cap1114_ns, CAP1114Component, CONF_CAP1114_ID

DEPENDENCIES = ["cap1114"]
CAP1114Output = cap1114_ns.class_("CAP1114OutputChannel", BinaryOutput, cg.Component)

CONFIG_SCHEMA = BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.Required(CONF_ID): cv.declare_id(CAP1114Output),
            cv.GenerateID(CONF_CAP1114_ID): cv.use_id(CAP1114Component),
            cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=11),
            cv.Optional(CONF_OPEN_DRAIN, False): cv.boolean
        }
    ).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await register_output(var, config)
    await cg.register_component(var, config)

    hub = await cg.get_variable(config[CONF_CAP1114_ID])
    cg.add(hub.register_output_channel(var))

    cg.add(var.set_channel(config[CONF_CHANNEL]))
    cg.add(var.set_open_drain(config.get(CONF_CHANNEL, False)))

