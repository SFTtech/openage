// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {
namespace gui {

class MainArgsLink : public QObject {
	Q_OBJECT

	Q_PROPERTY(QString dataDir READ get_data_dir CONSTANT)

public:
	explicit MainArgsLink(QObject *parent, const QString &data_dir);
	virtual ~MainArgsLink();

	QString get_data_dir() const;

	static QObject* provider(QQmlEngine*, QJSEngine*);

private:
	const QString data_dir;
};

}} // namespace openage::gui
