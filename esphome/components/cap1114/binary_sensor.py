import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_CHANNEL
from . import cap1114_ns, CAP1114Component, CONF_CAP1114_ID

DEPENDENCIES = ["cap1114"]
CAP1114Channel = cap1114_ns.class_("CAP1114TouchChannel", binary_sensor.BinarySensor)

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(CAP1114Channel).extend(
    {
        cv.GenerateID(CONF_CAP1114_ID): cv.use_id(CAP1114Component),
        cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=14),
    }
)

async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    hub = await cg.get_variable(config[CONF_CAP1114_ID])
    cg.add(var.set_channel(config[CONF_CHANNEL]))

    cg.add(hub.register_touch_channel(var))
