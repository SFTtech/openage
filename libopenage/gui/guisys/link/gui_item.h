// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <cassert>
#include <functional>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <QObject>
#include <QMetaType>
#include <QRegularExpression>
#include <QDebug>

#include "../private/game_logic_caller.h"
#include "../private/livereload/deferred_initial_constant_property_values.h"
#include "qtsdl_checked_static_cast.h"
#include "gui_item_link.h"

namespace qtsdl {


/**
 * Cleans a text from unneeded content like "qtsdl".
 */
QString name_tidier(const char *name);


/**
 * Helper to hide the type of the core.
 */
struct PersistentCoreHolderBase {
	virtual ~PersistentCoreHolderBase() {
	}

	/**
	 * Establish link from the core to the shell.
	 * Core must have a public 'gui::GuiItemLink *gui_link` member.
	 */
	virtual void adopt_shell(GuiItemLink *link) = 0;
};

template<typename T>
struct PersistentCoreHolder : public PersistentCoreHolderBase {
	PersistentCoreHolder(std::unique_ptr<T> core)
		:
		PersistentCoreHolderBase{},
		core{std::move(core)} {
	}

	explicit PersistentCoreHolder()
		:
		PersistentCoreHolderBase{} {
	}

	virtual void adopt_shell(GuiItemLink *link) override {
		this->core->gui_link = link;
	}

	std::unique_ptr<T> core;
};


class GuiItemBase : public DeferredInitialConstantPropertyValues {
public:
	virtual ~GuiItemBase() {
	}

private:
	friend class GuiLiveReloader;
	friend class GuiSubtreeImpl;

	template<typename>
	friend class GuiItemMethods;

	template<typename>
	friend class GuiItemCoreInstantiator;

	template<typename>
	friend class GuiItemListModel;

	/**
	 * Creates and returns a core object inside a holder.
	 */
	std::unique_ptr<PersistentCoreHolderBase> instantiate_core() {
		return this->instantiate_core_func();
	}

	/**
	 * Attaches to a core object.
	 */
	void adopt_core(PersistentCoreHolderBase *holder, const QString &tag) {
		this->do_adopt_core(holder, tag);

		if (this->on_core_adopted_func)
			this->on_core_adopted_func();

		this->on_core_adopted();
	}

	void do_adopt_core(PersistentCoreHolderBase *holder, const QString &tag) {
		this->do_adopt_core_func(holder, tag);
	}

	virtual void on_core_adopted() {
	}

	/**
	 * Set up signal to be able to run code in the game logic thread.
	 */
	void set_game_logic_callback(GuiCallback *game_logic_callback) {
		this->game_logic_caller.set_game_logic_callback(game_logic_callback);
	}

protected:
	GameLogicCaller game_logic_caller;
	std::function<std::unique_ptr<PersistentCoreHolderBase>()> instantiate_core_func;
	std::function<void(PersistentCoreHolderBase*, const QString&)> do_adopt_core_func;
	std::function<void()> on_core_adopted_func;
};

template<typename T>
class GuiItemOrigin {
	template<typename>
	friend class GuiItemMethods;

	template<typename>
	friend class GuiItemCoreInstantiator;

	/**
	 * Implementation object.
	 */
	PersistentCoreHolder<typename Unwrap<T>::Type> *holder;
};

/**
 * Member function of the GuiPersistentItem
 */
template<typename T>
class GuiItemMethods {
public:

#ifndef NDEBUG
	virtual ~GuiItemMethods() {
	}
#endif

	/**
	 * Get core.
	 */
	template<typename U>
	U* get() const {
		if (checked_static_cast<T*>(this)->holder->core) {
			assert(checked_static_cast<U*>(checked_static_cast<T*>(this)->holder->core.get()));
			return checked_static_cast<U*>(checked_static_cast<T*>(this)->holder->core.get());
		} else {
			return nullptr;
		}
	}

	/**
	 * Get core.
	 */
	template<typename U>
	U* get() {
		if (checked_static_cast<T*>(this)->holder->core) {
			assert(checked_static_cast<U*>(checked_static_cast<T*>(this)->holder->core.get()));
			return checked_static_cast<U*>(checked_static_cast<T*>(this)->holder->core.get());
		} else {
			return nullptr;
		}
	}

	/**
	 * Invoke a function in the logic thread (the thread of the core of this object).
	 */
	template<typename F, typename ... Args>
	void i(F f, Args&& ... args) {
		GuiItemBase *base = checked_static_cast<GuiItemBase*>(checked_static_cast<T*>(this));

		emit base->game_logic_caller.in_game_logic_thread([=] {
			static_assert_about_unwrapping<F, decltype(unwrap(checked_static_cast<T*>(this))), decltype(unwrap_if_can(args))...>();
			f(unwrap(checked_static_cast<T*>(this)), unwrap_if_can(args)...);
		});
	}

protected:
	/**
	 * Set property.
	 */
	template<typename F, typename P, typename A>
	void s(F f, P &p, A &&arg) {
		if (p != arg) {
			p = std::forward<A>(arg);
			this->assign_while_catching_constant_properies_inits(f, p);
		}
	}

	/**
	 * Set property even if it's the same.
	 */
	template<typename F, typename P, typename A>
	void sf(F f, P &p, A &&arg) {
		p = std::forward<A>(arg);
		this->assign_while_catching_constant_properies_inits(f, p);
	}

private:
	/**
	 * Static QML properties assignments during the init phase are stored for a batch assignment at the end of the init phase.
	 */
	template<typename F, typename A>
	void assign_while_catching_constant_properies_inits(F f, A &&arg) {
		GuiItemBase *base = checked_static_cast<GuiItemBase*>(checked_static_cast<T*>(this));

		static_assert_about_unwrapping<F, decltype(unwrap(checked_static_cast<T*>(this))), decltype(unwrap_if_can(arg))>();

		if (base->init_over)
			emit base->game_logic_caller.in_game_logic_thread([=] {f(unwrap(checked_static_cast<T*>(this)), unwrap_if_can(arg));});
		else
			base->static_properties_assignments.push_back([=] {
				emit base->game_logic_caller.in_game_logic_thread([=] {f(unwrap(checked_static_cast<T*>(this)), unwrap_if_can(arg));});
			});
	}
};

class GuiItemQObject : public QObject, public GuiItemBase, public GuiItemLink {
	Q_OBJECT

public:
	explicit GuiItemQObject(QObject *parent=nullptr);
};

template<typename T>
class GuiItemCoreInstantiator : public GuiItemMethods<T> {
public:
	/**
	 * Sets up a factory for the type T.
	 */
	explicit GuiItemCoreInstantiator(GuiItemBase *item_base)
		:
		GuiItemMethods<T>{} {

		using namespace std::placeholders;

		item_base->instantiate_core_func = std::bind(&GuiItemCoreInstantiator::instantiate_core, this);
		item_base->do_adopt_core_func = std::bind(&GuiItemCoreInstantiator::do_adopt_core, this, _1, _2);
	}

private:
	/**
	 * Creates and returns a core object of type Unwrap<T>::Type inside a holder.
	 */
	std::unique_ptr<PersistentCoreHolderBase> instantiate_core() {
		T *origin = checked_static_cast<T*>(this);

		if (origin->holder) {
			return std::unique_ptr<PersistentCoreHolderBase>();
		} else {
			auto core = std::make_unique<typename Unwrap<T>::Type>(origin);
			return std::make_unique<typename std::remove_reference<decltype(*origin->holder)>::type>(std::move(core));
		}
	}

	/**
	 * Attaches to a core object.
	 */
	void do_adopt_core(PersistentCoreHolderBase *holder, const QString &tag) {
		assert(holder);

		T *origin = checked_static_cast<T*>(this);

		if (origin->holder) {
			qFatal("Error in QML code: GuiLiveReloader was asked to use same tag '%s' for multiple objects.", qUtf8Printable(tag));
		} else {
			if (typeid(decltype(*origin->holder)) != typeid(*holder)) {
				qFatal(
					"Error in QML code: GuiLiveReloader was asked "
					"to restore '%s' into different type '%s' "
					"using tag '%s'.",
					qUtf8Printable(name_tidier(typeid(decltype(*origin->holder)).name())),
					qUtf8Printable(name_tidier(typeid(*holder).name())),
					qUtf8Printable(tag)
				);
			} else {
				origin->holder = checked_static_cast<decltype(origin->holder)>(holder);
				origin->holder->adopt_shell(origin);
			}
		}
	}

	GuiItemCoreInstantiator(const GuiItemCoreInstantiator&) = delete;
	GuiItemCoreInstantiator& operator=(const GuiItemCoreInstantiator&) = delete;
};

/**
 * Base for shell QObjects that need corresponding cores to be kept alive across GUI reloads.
 *
 * These shell objects are destroyed by Qt during GUI reload.
 * Their cores are adopted by new shells identified by the 'LR.tag' in QML code.
 *
 * For each descendant the specializations of Wrap and Unwrap must be provided.
 * Corresponding cores must have public 'gui::GuiItemLink *gui_link` members.
 *
 * @tparam T type of the concrete shell class (pass the descendant class)
 */
template<typename T>
class GuiItem : public GuiItemOrigin<T>, public GuiItemCoreInstantiator<T> {
public:
	/**
	 * Creates an empty QObject shell for a core that is wrappable into a type *T.
	 */
	explicit GuiItem(GuiItemBase *item_base)
		:
		GuiItemOrigin<T>{},
		GuiItemCoreInstantiator<T>{item_base} {
	}
};

template<typename T>
class GuiItemInterface : public GuiItemOrigin<T>, public GuiItemMethods<T> {
public:
	explicit GuiItemInterface()
		:
		GuiItemOrigin<T>{},
		GuiItemMethods<T>{} {
	}
};

namespace {
class NullClass {
	NullClass() = delete;
};
}

/**
 * Shadows inherited member functions.
 */
template<typename T>
class Shadow: public T {
public:
	Shadow(QObject *parent=nullptr)
		:
		T{parent} {
	}

	void get(NullClass);
	void i(NullClass);
	void s(NullClass);
	void sf(NullClass);
};

/**
 * Switches the factory to production of the instances of the derived class.
 */
template<typename T, typename P>
class Inherits: public Shadow<T>, public GuiItemCoreInstantiator<P> {
public:
	using Shadow<T>::get;
	using GuiItemCoreInstantiator<P>::get;
	using Shadow<T>::i;
	using GuiItemCoreInstantiator<P>::i;
	using Shadow<T>::s;
	using GuiItemCoreInstantiator<P>::s;
	using Shadow<T>::sf;
	using GuiItemCoreInstantiator<P>::sf;

	Inherits(QObject *parent=nullptr)
		:
		Shadow<T>{parent},
		GuiItemCoreInstantiator<P>{this} {
	}

	virtual ~Inherits() {
	}
};

} // namespace qtsdl
