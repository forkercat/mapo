set(SRCROOT ${PROJECT_SOURCE_DIR}/editor)

add_executable(editor "")

target_sources(editor
PUBLIC
	${SRCROOT}/editor.h
PRIVATE
	${SRCROOT}/editor.cpp
)
