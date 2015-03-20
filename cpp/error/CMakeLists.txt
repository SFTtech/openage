add_sources(${PROJECT_NAME}
	backtrace.cpp
	demo.cpp
	error.cpp
	handlers.cpp
	stackanalyzer.cpp
)

pxdgen(
	backtrace.h
	error.h
)
