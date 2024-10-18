set(sources
  GooglePlugin.cpp
)

set(headers
  exportdecl.h
)

choreonoid_qt_add_resources(RC_SRCS GoogleMaterialSymbols.qrc)
set(target CnoidGooglePlugin)
choreonoid_make_gettext_mo_files(${target} mofiles)
choreonoid_add_plugin(${target} ${sources} ${mofiles} ${RC_SRCS} HEADERS ${headers})
target_link_libraries(${target} PUBLIC CnoidBodyPlugin)