# Uncomment and edit following two lines to set component requirements (see esp-idf docs)
# set(COMPONENT_REQUIRES )
# set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "." "../..")
set(COMPONENT_PRIV_REQUIRES ESP32SimpleOTA)

register_component()
