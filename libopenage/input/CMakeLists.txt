add_sources(libopenage
	action.cpp
	action_serialization
	event.cpp
	event_serialization
	input_context.cpp
	input_context_recorder_player.cpp
	input_manager.cpp
	text_to_event.cpp
)

pxdgen(
	input_manager.h
)
