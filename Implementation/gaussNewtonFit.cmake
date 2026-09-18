set(APP_NAME GaussNewtonFit)

file(GLOB GUI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/gui/*.cpp)
if(NOT WIN32)
	file(GLOB GUI_MM ${CMAKE_CURRENT_LIST_DIR}/src/gui/*.mm)
	list(APPEND GUI_SOURCES ${GUI_MM})
endif()
file(GLOB GUI_INCS ${CMAKE_CURRENT_LIST_DIR}/src/gui/*.h)
set(APP_PLIST ${CMAKE_CURRENT_LIST_DIR}/src/Info.plist)

set(CORE_SOURCES ${DATA_SOURCES} ${MODEL_SOURCES} ${SOLVER_SOURCES} ${REPORT_SOURCES})
list(FILTER CORE_SOURCES EXCLUDE REGEX "Tests\\.cpp$")
list(FILTER CORE_SOURCES EXCLUDE REGEX "src/main\\.cpp$")

add_executable(${APP_NAME} ${GUI_SOURCES} ${GUI_INCS} ${CORE_SOURCES})

target_include_directories(${APP_NAME} PRIVATE ${SOURCE_ROOT}/src ${SOURCE_ROOT}/include)

target_link_libraries(${APP_NAME}
	debug ${MU_LIB_DEBUG} debug ${NATGUI_LIB_DEBUG} debug ${MATRIX_LIB_DEBUG} debug ${NATPLOT_LIB_DEBUG}
	optimized ${MU_LIB_RELEASE} optimized ${NATGUI_LIB_RELEASE} optimized ${MATRIX_LIB_RELEASE} optimized ${NATPLOT_LIB_RELEASE})
if(APPLE)
	target_link_libraries(${APP_NAME} "-framework Foundation" "-framework CoreFoundation" "-framework AppKit")
endif()

setTargetPropertiesForGUIApp(${APP_NAME} ${APP_PLIST})
setIDEPropertiesForGUIExecutable(${APP_NAME} ${CMAKE_CURRENT_LIST_DIR})
setPlatformDLLPath(${APP_NAME})

set(THEME_DIR ${SOURCE_ROOT}/Theme)

if(APPLE)
	add_custom_command(TARGET ${APP_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${APP_NAME}>/../Resources
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_ROOT}/res $<TARGET_FILE_DIR:${APP_NAME}>/../Resources/res
		COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_ROOT}/res/DevRes.xml $<TARGET_FILE_DIR:${APP_NAME}>/../Resources/DevRes.xml
		COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_ROOT}/res/main.xml $<TARGET_FILE_DIR:${APP_NAME}>/../Resources/main.xml
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_ROOT}/res/tr $<TARGET_FILE_DIR:${APP_NAME}>/../Resources/tr
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${THEME_DIR} $<TARGET_FILE_DIR:${APP_NAME}>/../Resources/Theme
		COMMAND /bin/sh -c "ln -snf Theme '$<TARGET_FILE_DIR:${APP_NAME}>/../Resources/Work'"
		COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE_DIR:${APP_NAME}>/GaussNewtonFit $<TARGET_FILE_DIR:${APP_NAME}>/GaussNewtonFit.bin
		COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_ROOT}/launcher.sh $<TARGET_FILE_DIR:${APP_NAME}>/GaussNewtonFit
		COMMAND chmod +x $<TARGET_FILE_DIR:${APP_NAME}>/GaussNewtonFit
		COMMENT "Copying resources into app bundle Contents/Resources"
	)
endif()
