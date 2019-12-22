// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_map>
#include <tuple>
#include <memory>

#include <QStringList>
#include <QHash>
#include <QList>
#include <QtGlobal>
#include <QtQml>

// since qt 5.14, the std::hash of q* types are included in qt
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace std {
template<>
struct hash<QString> {
	size_t operator()(const QString& val) const noexcept {
		return qHash(val);
	}
};
}
#endif

namespace qtsdl {

class GuiItemBase;

/**
 * The 'LR.tag' attached property.
 */
class GuiLiveReloaderAttachedProperty : public QObject {
	Q_OBJECT

	Q_PROPERTY(QString tag READ get_tag WRITE set_tag)

public:
	explicit GuiLiveReloaderAttachedProperty(QObject *object);

	GuiItemBase* get_attachee() const;

	QString get_tag() const;
	void set_tag(const QString &tag);

	QString get_tag_for_init() const;

private:
	/**
	 * Tag that allows to connect newly created shells to corresponding cores after GUI reload.
	 */
	QString tag;
};

/**
 * Non-instantiable type that is needed to use a 'LR.tag' attached property.
 */
class GuiLiveReloaderAttachedPropertyProvider : public QObject {
	Q_OBJECT

public:
	static GuiLiveReloaderAttachedProperty* qmlAttachedProperties(QObject*);
};

class PersistentCoreHolderBase;

/**
 * Stores objects that need to be kept alive across GUI reloads.
 */
class GuiLiveReloader {

public:
	void init_persistent_items(const QList<GuiLiveReloaderAttachedProperty*> &items);

private:
	using TagToPreservableMap = std::unordered_map<QString, std::unique_ptr<PersistentCoreHolderBase>>;
	TagToPreservableMap preservable;
};

} // namespace qtsdl

QML_DECLARE_TYPEINFO(qtsdl::GuiLiveReloaderAttachedPropertyProvider, QML_HAS_ATTACHED_PROPERTIES)
