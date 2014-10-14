add_sources(${PROJECT_NAME}
	buf.cpp
	console.cpp
	draw.cpp
	tests.cpp
)

add_test_cpp(openage::console::tests::render "prints a few test lines to a buffer, and renders them to stdout")
add_demo_cpp(openage::console::tests::demo "showcases console as an interactive terminal on your current tty")
