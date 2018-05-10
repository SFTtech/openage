// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>

#include <QObject>
#include <QMetaType>

namespace qtsdl {

class GuiCallback : public QObject {
	Q_OBJECT

public:
	GuiCallback();
	virtual ~GuiCallback();

signals:
	void process_blocking(const std::function<void()> &f);

public slots:
	void process(const std::function<void()> &f);
};

} // namespace qtsdl

Q_DECLARE_METATYPE(std::function<void()>)
