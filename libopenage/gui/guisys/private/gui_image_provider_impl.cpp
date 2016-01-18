// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_image_provider_impl.h"

#include <algorithm>

#include "../public/gui_image_provider.h"

namespace qtsdl {

GuiImageProviderImpl::GuiImageProviderImpl()
	:
	QQuickImageProvider{QQmlImageProviderBase::Texture, QQuickImageProvider::ForceAsynchronousImageLoading} {
}

GuiImageProviderImpl::~GuiImageProviderImpl() {
}

std::unique_ptr<GuiImageProviderImpl> GuiImageProviderImpl::take_ownership(GuiImageProvider *image_provider) {
	std::unique_ptr<GuiImageProviderImpl> ptr{image_provider->impl.release()};
	image_provider->impl = decltype(image_provider->impl){ptr.get(), [] (GuiImageProviderImpl*) {}};
	return ptr;
}

std::vector<std::unique_ptr<GuiImageProviderImpl>> GuiImageProviderImpl::take_ownership(const std::vector<GuiImageProvider*> &image_providers) {
	std::vector<std::unique_ptr<GuiImageProviderImpl>> image_provider_owning_ptrs(image_providers.size());

	std::transform(std::begin(image_providers), std::end(image_providers), std::begin(image_provider_owning_ptrs), static_cast<std::unique_ptr<GuiImageProviderImpl>(*)(GuiImageProvider*)>(GuiImageProviderImpl::take_ownership));

	return image_provider_owning_ptrs;
}

} // namespace qtsdl
