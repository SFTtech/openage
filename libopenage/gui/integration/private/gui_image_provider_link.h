// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <QObject>
#include <QPointer>

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {

namespace gui {

class GuiImageProviderLink : public QObject {
	Q_OBJECT

public:
	explicit GuiImageProviderLink(QObject *parent);
	virtual ~GuiImageProviderLink();

	static QObject *provider(QQmlEngine *, const char *id);
};

} // namespace gui
} // namespace openage
