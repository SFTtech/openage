// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/texture.h"
#include <memory>
#include <string>

#include "renderer/gui/guisys/public/gui_subtree.h"

namespace qtgui {
class GuiInput;
class GuiRenderer;
class GuiQmlEngine;
class GuiApplication;
} // namespace qtgui

namespace openage {
namespace util {
class Path;
}

namespace renderer {
class RenderPass;
class Renderer;
class Window;

class UniformInput;

namespace gui {

/**
 * Interface (= HUD) of the game.
 *
 * openage's GUI is Qt-based and uses QML definitions
 * to create buttons, etc. Qt draws the GUI into a texture
 * that is then drawn into the screen by our wn renderer.
 */
class GUI {
public:
	explicit GUI(std::shared_ptr<qtgui::GuiApplication> app,
	             std::shared_ptr<Window> window,
	             const util::Path &source,
	             const util::Path &rootdir,
	             const util::Path &assetdir,
	             const std::shared_ptr<Renderer> &renderer);
	virtual ~GUI() = default;

	/**
	 * Get the input handler of the GUI.
	 *
	 * @return Input handler of the GUI.
	 */
	std::shared_ptr<qtgui::GuiInput> get_input_handler() const;

	/**
	 * Get the render pass of the GUI.
	 *
	 * The render pass has the GUI texture filled by Qt assigned as
	 * a renderable.
	 *
	 * @return Render pass of the GUI.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass() const;

	/**
	 * Render the GUI texture.
	 */
	void render();

private:
	/**
	 * Create the render pass of the GUI.
	 *
	 * Called during initialization of the GUI.
	 *
	 * @param width Width of the GUI.
	 * @param height Height of the GUI.
	 * @param shaderdir Directory containg the shader source files.
	 */
	void initialize_render_pass(size_t width,
	                            size_t height,
	                            const util::Path &shaderdir);

	/**
	 * Resize the GUI. This updates the GUI texture size and propagates
	 * the changes to the GUI render pass.
	 *
	 * @param width New width of the GUI.
	 * @param height New height of the GUI.
	 */
	void resize(size_t width, size_t height);

	/**
	 * Reference to the Qt GUI application singleton.
	 */
	std::shared_ptr<qtgui::GuiApplication> application;

	/**
	 * Qt-based renderer for the GUI texture. Draws into
	 * \p gui_texture.
	 */
	std::shared_ptr<qtgui::GuiRenderer> gui_renderer;

	/**
	 * Input handler for the GUI window
	 */
	std::shared_ptr<qtgui::GuiInput> gui_input;

	/**
	 * Qt QML Engine wrapper.
	 */
	std::shared_ptr<qtgui::GuiQmlEngine> engine;

	/**
	 * Manages Qt QML components and items.
	 */
	qtgui::GuiSubtree subtree;

	/**
	 * Reference to the openage renderer.
	 * Used to fetch texture objects for the GUI texture.
	 */
	std::shared_ptr<Renderer> renderer;

	/**
	 * Uniform input for the GUI texture handle.
	 */
	std::shared_ptr<renderer::UniformInput> texture_unif;

	/**
	 * GUI texture handle. The GUI renderer ( \p gui_renderer ) draws
	 * into this texture.
	 */
	std::shared_ptr<renderer::Texture2d> texture;

	/**
	 * Render pass for the whole GUI. The GUI texture is attached to
	 * this pass via a \p renderer::resources::Renderable.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;
};

} // namespace gui
} // namespace renderer
} // namespace openage
