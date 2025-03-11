set(sources
  BodyCreatorDialog.cpp
  FormatConverter.cpp
  ColorButton.cpp
  CrawlerCreator.cpp
  GeneratorButtonBox.cpp
  GratingCreator.cpp
  BodyEditPlugin.cpp
  InertiaCalculator.cpp
  PipeCreator.cpp
  SlopeCreator.cpp
  StairsCreator.cpp
  TerrainCreator.cpp
  BentPipeCreator.cpp
  FileDroppableWidget.cpp
  Notepad.cpp
)

set(headers
  BodyCreator.h
  BodyCreatorDialog.h
  FormatConverter.h
  ColorButton.h
  GeneratorButtonBox.h
  InertiaCalculator.h
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