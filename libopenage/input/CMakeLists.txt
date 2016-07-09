add_sources(libopenage
	action.cpp
	action_serialization
	event.cpp
	input_context.cpp
	input_manager.cpp
	text_to_event.cpp
)

pxdgen(
	input_manager.h
)
