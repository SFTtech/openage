// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

#include "../util/path.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

namespace openage {
namespace gui {

/**
 * Used to make arguments of the game available in QML.
 */
class MainArgsLink : public QObject {
	Q_OBJECT

	Q_PROPERTY(openage::util::Path assetDir MEMBER asset_dir CONSTANT)

public:
	explicit MainArgsLink(QObject *parent, const util::Path &asset_dir);
	virtual ~MainArgsLink() = default;

	/**
	 * Generates the MainArgsLink object which is then used within QML.
	 */
	static QObject* provider(QQmlEngine*, QJSEngine*);

private:
	util::Path asset_dir;
};

}} // namespace openage::gui
