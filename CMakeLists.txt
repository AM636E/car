cmake_minimum_required(VERSION 3.22)
project(smash)

find_package(PkgConfig REQUIRED)

# Check for GLib and GIO
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(BLUEZ REQUIRED bluez)
# Find D-Bus package
pkg_check_modules(DBUS REQUIRED dbus-1)
# Specify the C standard

set(SOURCE_DIR src)
file(GLOB SOURCES "${SOURCE_DIR}/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${GLIB_INCLUDE_DIRS} 
    ${GIO_INCLUDE_DIRS} 
    ${BLUEZ_INCLUDE_DIRS}
    ${DBUS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} /usr/lib/libpigpio.so 
    ${GLIB_LIBRARIES} 
    ${GIO_LIBRARIES} 
    ${BLUEZ_LIBRARIES}
    ${DBUS_LIBRARIES}
)

# Additional compiler and linker flags from pkg-config
target_compile_options(${PROJECT_NAME} PRIVATE ${GLIB_CFLAGS_OTHER} ${GIO_CFLAGS_OTHER} -Wno-implicit-function-declaration)
target_link_options(${PROJECT_NAME} PRIVATE 
    ${GLIB_LDFLAGS_OTHER} 
    ${GIO_LDFLAGS_OTHER}    
    ${DBUS_CFLAGS_OTHER}
)