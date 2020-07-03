function(FindWayland libs)
  pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
  pkg_check_modules(WAYLAND_CURSOR wayland-cursor)
  if(WAYLAND_CLIENT_FOUND)
    set(${libs} ${${libs}} ${WAYLAND_CLIENT_LIBRARIES} PARENT_SCOPE)
    include_directories (${WAYLAND_CLIENT_INCLUDE_DIRS})
    pkg_check_modules (FOO wayland-client, wayland-client >=1.13.0)
    if (FOO_FOUND)
      add_definitions(-DUSE_WAYLAND_1_13)
    endif ()
  endif()

  if(WAYLAND_CURSOR_FOUND)
    set(${libs} ${${libs}} ${WAYLAND_CURSOR_LIBRARIES} PARENT_SCOPE)
    include_directories (${WAYLAND_CURSOR_INCLUDE_DIRS})
  endif()
endfunction(FindWayland)

function(FindX11 libs)
  pkg_check_modules(XCBPRESENT xcb-present)
  pkg_check_modules(X11    REQUIRED x11)
  pkg_check_modules(XKB    xkbcommon)
  pkg_check_modules(XRANDR xrandr)
  pkg_check_modules(XRENDER xrender)

  if(XCBPRESENT_FOUND)
	  set(${libs} ${${libs}} ${XCBPRESENT_LIBRARIES} PARENT_SCOPE)
	  include_directories (${XCBPRESENT_INCLUDE_DIRS})
      add_definitions(-DHAVE_XCBPRESENT)
  endif()

  if(X11_FOUND)
    set(${libs} ${${libs}} ${X11_LIBRARIES} PARENT_SCOPE)
    include_directories (${X11_INCLUDE_DIRS})
  endif()

  if(XKB_FOUND)
    set(${libs} ${${libs}} ${XKB_LIBRARIES} PARENT_SCOPE)
    include_directories (${XKB_INCLUDE_DIRS})
    add_definitions(-DHAVE_XKBLIB)
  endif()

  if(XRENDER_FOUND)
    set(${libs} ${${libs}} ${XRENDER_LIBRARIES} PARENT_SCOPE)
    include_directories (${XRENDER_INCLUDE_DIRS})
    add_definitions(-DHAVE_XRENDER)
  endif()

  if(XRANDR_FOUND)
    set(${libs} ${${libs}} ${XRANDR_LIBRARIES} PARENT_SCOPE)
    include_directories (${XRANDR_INCLUDE_DIRS})
    add_definitions(-DHAVE_XRANDR)
  endif()
endfunction(FindX11)

function(FindEGL libs)
  pkg_check_modules(EGL REQUIRED egl)
  if(EGL_FOUND)
    set(${libs} ${${libs}} ${EGL_LIBRARIES} PARENT_SCOPE)
    include_directories(${EGL_INCLUDE_DIRS})
  endif()
endfunction(FindEGL)

function(FindEGLWayland libs)
  pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
  if(WAYLAND_EGL_FOUND)
    set(${libs} ${${libs}} ${WAYLAND_EGL_LIBRARIES} PARENT_SCOPE)
    include_directories(${WAYLAND_EGL_INCLUDE_DIRS})
  endif()
endfunction(FindEGLWayland)
