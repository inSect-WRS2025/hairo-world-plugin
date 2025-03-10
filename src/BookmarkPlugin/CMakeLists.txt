set(sources
  ArchiveListDialog.cpp
  BookmarkManager.cpp
  BookmarkPlugin.cpp
  HamburgerMenu.cpp
  HistoryManager.cpp
  JoyKey.cpp
  KIOSKManager.cpp
  LayoutManager.cpp
  SystemTrayIcon.cpp
  WorldLogManager.cpp
  SwitchableWidget.cpp
)

set(headers
  ArchiveListDialog.h
  BookmarkManager.h
  HamburgerMenu.h
  HistoryManager.h
  JoyKey.h
  KIOSKManager.h
  LayoutManager.h
  SystemTrayIcon.h
  WorldLogManager.h
  SwitchableWidget.h
  exportdecl.h
)

choreonoid_make_header_public(SystemTrayIcon.h)
choreonoid_make_header_public(HamburgerMenu.h)
choreonoid_make_header_public(SwitchableWidget.h)

set(target CnoidBookmarkPlugin)
choreonoid_make_gettext_mo_files(${target} mofiles)
choreonoid_add_plugin(${target} ${sources} ${mofiles} HEADERS ${headers})
target_link_libraries(${target} PUBLIC CnoidBodyPlugin CnoidMotionCapturePlugin CnoidGooglePlugin)