set(sources
  BodyConverter.cpp
  ColorButton.cpp
  CrawlerGenerator.cpp
  GeneratorButtonBox.cpp
  GratingGenerator.cpp
  BodyEditPlugin.cpp
  InertiaCalculator.cpp
  PipeGenerator.cpp
  SlopeGenerator.cpp
  StairsGenerator.cpp
  TerrainGenerator.cpp
  BentPipeGenerator.cpp
  FileDroppableWidget.cpp
  Notepad.cpp
)

set(headers
  BodyConverter.h
  ColorButton.h
  CrawlerGenerator.h
  GeneratorButtonBox.h
  GratingGenerator.h
  InertiaCalculator.h
  PipeGenerator.h
  SlopeGenerator.h
  StairsGenerator.h
  TerrainGenerator.h
  BentPipeGenerator.h
  FileDroppableWidget.h
  Notepad.h
  WidgetInfo.h
  exportdecl.h
)

choreonoid_make_header_public(FileDroppableWidget.h)

set(target CnoidBodyEditPlugin)
choreonoid_make_gettext_mo_files(${target} mofiles)
choreonoid_add_plugin(${target} ${sources} ${mofiles} HEADERS ${headers})
target_link_libraries(${target} PUBLIC CnoidBodyPlugin CnoidBookmarkPlugin)