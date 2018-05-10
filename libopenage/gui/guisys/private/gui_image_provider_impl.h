// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include <QQuickImageProvider>

namespace qtsdl {

class GuiImageProvider;

class GuiImageProviderImpl : public QQuickImageProvider {
public:
	explicit GuiImageProviderImpl();
	virtual ~GuiImageProviderImpl();

	static std::unique_ptr<GuiImageProviderImpl> take_ownership(GuiImageProvider *image_provider);
	static std::vector<std::unique_ptr<GuiImageProviderImpl>> take_ownership(const std::vector<GuiImageProvider*> &image_providers);

	virtual const char* get_id() const = 0;

	virtual void give_up() = 0;
};

} // namespace qtsdl
