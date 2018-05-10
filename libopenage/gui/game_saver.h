// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <QObject>

namespace openage {
namespace gui {

class GameMainLink;
class GeneratorLink;

class GameSaver : public QObject {
	Q_OBJECT

	Q_ENUMS(State)
	Q_PROPERTY(QString errorString READ get_error_string NOTIFY error_string_changed)
	Q_PROPERTY(openage::gui::GameMainLink* game MEMBER game NOTIFY game_changed)
	Q_PROPERTY(openage::gui::GeneratorLink* generatorParameters MEMBER generator_parameters NOTIFY generator_parameters_changed)

public:
	explicit GameSaver(QObject *parent=nullptr);
	virtual ~GameSaver();

	QString get_error_string() const;

	Q_INVOKABLE void activate();
	Q_INVOKABLE void clearErrors();

public slots:
	void on_processed(const QString &error_string);

signals:
	void error_string_changed();
	void game_changed();
	void generator_parameters_changed();

private:
	QString error_string;
	GameMainLink *game;
	GeneratorLink *generator_parameters;
};

class GameSaverSignals : public QObject {
	Q_OBJECT

public:
signals:
	void error_message(const QString &error);
};

}} // namespace openage::gui
