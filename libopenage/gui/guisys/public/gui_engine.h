// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

namespace qtsdl {

class GuiRenderer;
class GuiImageProvider;
class GuiEngineImpl;
struct GuiSingletonItemsInfo;

/**
 * Represents one QML execution environment.
 */
class GuiEngine {
public:
	explicit GuiEngine(GuiRenderer *renderer, const std::vector<GuiImageProvider*> &image_providers=std::vector<GuiImageProvider*>(), GuiSingletonItemsInfo *singleton_items_info=nullptr);
	~GuiEngine();

private:
	friend class GuiEngineImpl;
	std::unique_ptr<GuiEngineImpl> impl;
};

} // namespace qtsdl
