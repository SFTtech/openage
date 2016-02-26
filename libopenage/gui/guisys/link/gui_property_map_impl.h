// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

namespace qtsdl {

class GuiPropertyMapImpl : public QObject {
	Q_OBJECT

public:
	GuiPropertyMapImpl();
	virtual ~GuiPropertyMapImpl();

signals:
	void property_changed(const QByteArray &name, const QVariant &value);

private:
	virtual bool event(QEvent *e) override;
};

} // namespace qtsdl
