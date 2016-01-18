// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <functional>

namespace qtsdl {

class GuiImageProviderImpl;

class GuiImageProvider {
public:
	explicit GuiImageProvider(std::unique_ptr<GuiImageProviderImpl> impl);
	~GuiImageProvider();

private:
	friend class GuiImageProviderImpl;
	std::unique_ptr<GuiImageProviderImpl, std::function<void(GuiImageProviderImpl*)>> impl;
};

} // namespace qtsdl
