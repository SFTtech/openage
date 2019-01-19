// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#include "minimap.h"

#include <cmath>

#include <QtQml>

// Must be included before the Qt GL headers because of epoxy.
#include "game_main_link.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGSimpleTextureNode>

#include "guisys/link/qtsdl_checked_static_cast.h"
#include "integration/private/gui_texture.h"
#include "integration/private/gui_texture_from_img_data.h"

#include "engine_link.h"

#include "gui_textured_points_shader.h"
#include "minimap_background.h"
#include "minimap_marker_type.h"

namespace openage {
namespace gui {

namespace {
const int registration = qmlRegisterType<Minimap>("yay.sfttech.openage", 1, 0, "Minimap");
}

BackgroundUpdate::BackgroundUpdate(std::tuple<std::unique_ptr<uint32_t[]>, int, int>&& background_tex, std::tuple<coord::chunk, coord::chunk_t> bounding_square)
	:
	background_tex{std::move(background_tex)},
	bounding_square{bounding_square} {
}

bool BackgroundUpdate::consumed() const {
	return !std::get<0>(this->background_tex).operator bool();
}

std::tuple<std::unique_ptr<GuiTexture>, QSizeF, std::tuple<coord::chunk, coord::chunk_t>> BackgroundUpdate::consume() {
	ENSURE(!this->consumed(), "Trying to create a background texture from the same terrain color data multiple times.");

	return std::tuple_cat(
		gui_texture_from_img_data(std::move(std::get<0>(this->background_tex)), std::get<1>(this->background_tex), std::get<2>(this->background_tex)),
		std::tie(this->bounding_square)
	);
}

Minimap::Minimap(QQuickItem *parent)
	:
	QQuickItem{parent},
	game{},
	game_dirty{},
	terrain_usage_hash{},
	need_fresh_bg{},
	marker_size{4},
	marker_atlas{},
	marker_atlas_dirty{},
	marker_atlas_spacing{0.f},
	marker_atlas_spacing_dirty{} {
	Q_UNUSED(registration);

	this->setFlag(QQuickItem::ItemHasContents, true);
	this->setAcceptedMouseButtons(Qt::LeftButton);
}

Minimap::~Minimap() {
}

namespace {
std::tuple<std::vector<gamedata::palette_color>, std::vector<gamedata::palette_color>> get_palettes(GameMainLink *game, const std::string &p0, const std::string &p1) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Palette loading allowed only in the game logic thread.");
	ENSURE(unwrap(game)->is_game_running(), "Game must be running to access palettes from the correct asset manager.");

	auto spec = unwrap(game)->get_game()->get_spec();

	return std::make_tuple(spec->get_palette(p0), spec->get_palette(p1));
}

std::tuple<std::string, std::string> get_shaders(GameMainLink *game) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Shader loading allowed only in the game logic thread.");
	ENSURE(unwrap(game)->is_game_running(), "Game must be running to access shaders from the correct asset manager.");

	const auto shaders_dir = unwrap(game)->get_game()->get_spec()->get_asset_manager()->get_asset_dir() / "shaders";

	return std::make_tuple(shaders_dir["atlasgrid.vert.glsl"].open().read(), shaders_dir["atlasgrid.frag.glsl"].open().read());
}

size_t get_terrain_usage_hash(GameMainLink *game) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Terrain hash calculation is allowed only in the game logic thread.");
	ENSURE(unwrap(game)->is_game_running(), "Game must be running to do terrain hash calculation.");

	return terrain_used_chunks_hash(*unwrap(game)->get_game()->terrain);
}

BackgroundUpdate get_background_update(GameMainLink *game, const std::vector<gamedata::palette_color> &palette) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Creation of the background texture is allowed only in the game logic thread.");

	auto &terrain = unwrap(game)->get_game()->terrain;
	return BackgroundUpdate{generate_minimap_background(*terrain, palette), terrain_bounding_square(*terrain)};
}

std::tuple<coord::CoordManager, bool> get_engine_coord_data(GameMainLink *game) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Operations with Engine are allowed only in the game logic thread.");

	if (Engine *engine = unwrap(game->get_engine()))
        return std::make_tuple(engine->coord, true);

    return std::tuple<coord::CoordManager, bool>{};
}

std::vector<ColoredTexturedPoint2D> get_marker_points(GameMainLink *game, const std::vector<gamedata::minimap_modes> &markers_types, const std::vector<gamedata::palette_color> &player_palette) {
	ENSURE(game->current_thread_is_game_logic_thread(), "Extracting of the unit positions is allowed only in the game logic thread.");
	return get_unit_marker_points(unwrap(game)->get_game()->placed_units, markers_types, player_palette);
}

const QSGGeometry::AttributeSet& defaultAttributes_ColoredTexturedPoint2D()
{
	static QSGGeometry::Attribute data[] = {
		QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
		QSGGeometry::Attribute::create(1, 1, GL_FLOAT),
		QSGGeometry::Attribute::create(2, 4, GL_UNSIGNED_BYTE),
		QSGGeometry::Attribute::create(3, 1, GL_UNSIGNED_BYTE)
	};
	static QSGGeometry::AttributeSet attrs = {3, sizeof(ColoredTexturedPoint2D), data};
	return attrs;
}
}

std::unique_ptr<QSGNode> Minimap::create_tree() {
	this->last_bounding_rect = QRectF{};
	this->need_fresh_bg = true;
	this->marker_atlas_dirty = true;
	this->marker_atlas_spacing_dirty = true;

	auto world_to_background_ptr = std::make_unique<QSGTransformNode>();
	auto world_to_background = world_to_background_ptr.get();

	auto geometry_ptr = std::make_unique<QSGGeometry>(defaultAttributes_ColoredTexturedPoint2D(), 0);
	geometry_ptr->setDrawingMode(GL_POINTS);
	auto geometry = geometry_ptr.get();

	auto material_ptr = std::unique_ptr<QSGSimpleMaterial<Icon>>(TexturedPointsShader::createMaterial(this->shaders));
	glEnable(GL_POINT_SPRITE);
	material_ptr->setFlag(QSGMaterial::Blending);

	if (!this->hidden_foreground)
		this->hidden_foreground = std::make_unique<QSGGeometryNode>();

	this->hidden_foreground->setFlag(QSGNode::OwnsGeometry);
	this->hidden_foreground->setFlag(QSGNode::OwnsMaterial);
	auto foreground = this->hidden_foreground.get();

	auto turn_ptr = std::make_unique<QSGTransformNode>();
	auto turn = turn_ptr.get();

	auto background_ptr = std::make_unique<QSGSimpleTextureNode>();
	background_ptr->setOwnsTexture(true);
	auto background = background_ptr.get();

	this->update_marker_atlas = [this, world_to_background, foreground, geometry] {
		if (this->marker_atlas_dirty) {
			const bool markers_disabled = !this->marker_atlas || !this->marker_atlas->isTextureProvider();

			if (markers_disabled && this->marker_atlas)
				qWarning() << "MinimapImpl.markerAtlas must have 'layer.enabled' or be a texture provider.";

			if (markers_disabled && world_to_background->childCount()) {
				ENSURE(world_to_background->firstChild() == foreground, "Scenegraph has a node that we have not attached to it. Expected our foreground node.");
				world_to_background->removeChildNode(foreground);
				this->hidden_foreground.reset(foreground);
			}

			if (!markers_disabled && !world_to_background->childCount())
				world_to_background->appendChildNode(this->hidden_foreground.release());

			if (!markers_disabled) {
				qtsdl::checked_static_cast<QSGSimpleMaterial<Icon>*>(foreground->material())->state()->marker_atlas = this->marker_atlas;
				qtsdl::checked_static_cast<QSGSimpleMaterial<Icon>*>(foreground->material())->state()->columns = std::ceil(std::sqrt(this->markers_types.size()));
				foreground->markDirty(QSGNode::DirtyMaterial);
			}

			this->marker_atlas_dirty = false;
		}

		if (this->marker_atlas_spacing_dirty) {
			qtsdl::checked_static_cast<QSGSimpleMaterial<Icon>*>(foreground->material())->state()->spacing = this->marker_atlas_spacing;
			foreground->markDirty(QSGNode::DirtyMaterial);

			this->marker_atlas_spacing_dirty = false;
		}

		if (this->marker_size > 0 && geometry->lineWidth()) {
			geometry->setLineWidth(this->marker_size);
			foreground->markDirty(QSGNode::DirtyGeometry);
		}
	};

    this->update_backround = [this, background, world_to_background, turn] (BackgroundUpdate &&bg_update, const coord::CoordManager &engine_coord_data) {
		if (!bg_update.consumed()) {
			const bool width_bigger = this->boundingRect().width() >= this->boundingRect().height();
			auto big_side = std::max(this->boundingRect().width(), this->boundingRect().height());
			auto small_side = std::min(this->boundingRect().width(), this->boundingRect().height());

			auto background_gui_tex = bg_update.consume();

			background->setTexture(std::get<std::unique_ptr<GuiTexture>>(background_gui_tex).release());
			background->setSourceRect({QPointF{}, std::get<QSizeF>(background_gui_tex)});

			auto terrain_bounding_square_max_side_len = terrain_square_dimensions(std::get<std::tuple<coord::chunk, coord::chunk_t>>(background_gui_tex));
            coord::phys_t max_terrain_side_len = coord::tile::from_chunk(coord::chunk{terrain_bounding_square_max_side_len, 0}, {0, 0}).to_phys2().ne;

			/**
			 * Setup transformation from the world coordinates to the coordinates on the minimap background texture.
			 */
			{
				QMatrix4x4 downscale_world;
				downscale_world.translate(big_side / 2, big_side / 2);
                downscale_world.scale(big_side / double(max_terrain_side_len), big_side / double(max_terrain_side_len));
                auto terrain_bounding_rect_center_phys = terrain_square_center(std::get<std::tuple<coord::chunk, coord::chunk_t>>(background_gui_tex)).to_tile3(*unwrap(game)->get_game()->terrain).to_phys3();
                downscale_world.translate(double(-terrain_bounding_rect_center_phys.ne), double(-terrain_bounding_rect_center_phys.se));
				world_to_background->setMatrix(downscale_world);
			}

			if (this->last_bounding_rect != this->boundingRect()) {
				QMatrix4x4 trans_to_center(QTransform::fromTranslate(-big_side / 2., -big_side / 2.));

                auto iv = (coord::tile3{1, 0, 0}.to_phys3() - coord::tile3{0, 0, 0}.to_phys3()).to_camgame(engine_coord_data).to_viewport();
                auto jv = (coord::tile3{0, 1, 0}.to_phys3() - coord::tile3{0, 0, 0}.to_phys3()).to_camgame(engine_coord_data).to_viewport();

				/**
				 * Background texture uses square pixels for tiles, so need to undo the tile sizing.
				 */
                auto i = QVector2D(iv.x / engine_coord_data.tile_size.x/2, iv.y / engine_coord_data.tile_size.y/2);
                auto j = QVector2D(jv.x / engine_coord_data.tile_size.x/2, jv.y / engine_coord_data.tile_size.y/2);

				QMatrix4x4 cam_rotate(
					i.x(), j.x(), 0.f, 0.f,
					i.y(), j.y(), 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f
				);

				/**
				 * Fit a rotated square into the same area.
				 */
				auto diagonal = cam_rotate * QVector4D(1.f, 1.f, 0.f, 0.f);
				auto diagonal_max_coord = std::max(diagonal.x(), diagonal.y());
				QMatrix4x4 scale_fit(QTransform::fromScale(1. / diagonal_max_coord, 1. / diagonal_max_coord));

				auto ratio = width_bigger ? std::make_tuple(1., small_side / big_side) : std::make_tuple(small_side / big_side, 1.);
				QMatrix4x4 aspect(QTransform::fromScale(std::get<0>(ratio), std::get<1>(ratio)));

				QMatrix4x4 trans_from_center(QTransform::fromTranslate(this->boundingRect().width() / 2., this->boundingRect().height() / 2.));

				turn->setMatrix(trans_from_center * aspect * scale_fit * cam_rotate * trans_to_center);

				background->setRect(0., 0., big_side, big_side);

				this->last_bounding_rect = this->boundingRect();
			}
		}

		ENSURE(background->texture(), "Minimap background texture is undefined.");
	};

    this->update_view_frame_rect = [this, world_to_background, turn] (const coord::CoordManager &engine_coord_data) {
        auto world_cam_left_top = coord::viewport{0, 0}.to_camgame(engine_coord_data).to_phys3(engine_coord_data);
        auto world_cam_right_bottom = engine_coord_data.viewport_size.to_camgame().to_phys3(engine_coord_data);

        QVector3D cam_frame_lt = turn->matrix() * world_to_background->matrix() * QVector3D(double(world_cam_left_top.ne), double(world_cam_left_top.se), 0);
        QVector3D cam_frame_rb = turn->matrix() * world_to_background->matrix() * QVector3D(double(world_cam_right_bottom.ne), double(world_cam_right_bottom.se), 0);

		auto cam_frame_size = cam_frame_rb - cam_frame_lt;

		QRect new_view_frame_rect(cam_frame_lt.x(), cam_frame_lt.y(), cam_frame_size.x(), cam_frame_size.y());

		if (this->view_frame_rect != new_view_frame_rect) {
			this->view_frame_rect = new_view_frame_rect;
			emit this->viewFrameRectChanged();
		}
	};

	this->update_marker_points = [this, foreground, geometry] (const std::vector<ColoredTexturedPoint2D> &unit_marker_points) {
		geometry->allocate(unit_marker_points.size());
		std::copy(std::begin(unit_marker_points), std::end(unit_marker_points), static_cast<ColoredTexturedPoint2D*>(geometry->vertexData()));
		foreground->markDirty(QSGNode::DirtyGeometry);
	};

	this->mouse_move_camera = [this, world_to_background, turn] (QMouseEvent *mouseEvent) {
		if (!this->game) {
			return;
		}
		Engine *engine = unwrap(game->get_engine());
		if (!engine) {
			return;
		}

		const QMatrix4x4 local_to_world_transform = (turn->matrix() * world_to_background->matrix()).inverted();
		const QPointF pos = local_to_world_transform.map(mouseEvent->localPos());

		emit this->game->in_game_logic_thread_blocking([engine, pos] {
                engine->coord.camgame_phys.ne = pos.x();
                engine->coord.camgame_phys.se = pos.y();
				});
	};

	foreground->setMaterial(material_ptr.release());
	foreground->setGeometry(geometry_ptr.release());
	background->appendChildNode(world_to_background_ptr.release());
	turn->appendChildNode(background_ptr.release());

	return std::move(turn_ptr); // clang workaround
}

QSGNode* Minimap::updatePaintNode(QSGNode *node, UpdatePaintNodeData*) {
	std::unique_ptr<QSGNode> root{node};

	if (this->game && this->game->get_state() != GameMainLink::State::Null) {
		if (root || this->hidden_root || !this->bg_update.consumed()) {
			if (!root) {
				if (std::get<0>(this->shaders).empty())
					emit this->game->in_game_logic_thread_blocking([this] {
						if (unwrap(this->game)->is_game_running())
							this->shaders = get_shaders(this->game);
					});

				/**
				 * No link to the asset manager yet.
				 */
				if (std::get<0>(this->shaders).empty()) {
					this->update();
					return root.release();
				}

				root = this->hidden_root ? std::move(this->hidden_root) : create_tree();
			}

			if (std::get<bool>(this->engine_coord_data)) {
                auto& engine_coord_data = std::get<coord::CoordManager>(this->engine_coord_data);
				this->update_backround(std::move(this->bg_update), engine_coord_data);
				this->update_view_frame_rect(engine_coord_data);
			}

			this->update_marker_atlas();
			this->update_marker_points(this->unit_marker_points);
		} else {
			this->need_fresh_bg = true;
		}

		this->polish();
	} else {
		if (root)
			this->hidden_root = std::move(root);
	}

	this->update();

	return root.release();
}

void Minimap::mousePressEvent(QMouseEvent *event)
{
	this->mouse_move_camera(event);
}

void Minimap::mouseMoveEvent(QMouseEvent *event)
{
	this->mouse_move_camera(event);
}

void Minimap::updatePolish() {
	emit this->game->in_game_logic_thread_blocking([this] {
		if (unwrap(this->game)->is_game_running()) {
			if (this->game_dirty)
				std::tie(this->player_palette, this->palette) = get_palettes(this->game, "converted/player_palette.docx", "converted/general_palette.docx");

			auto new_terrain_usage_hash = get_terrain_usage_hash(this->game);
			if (this->game_dirty || this->need_fresh_bg || this->last_bounding_rect != this->boundingRect() || this->terrain_usage_hash != new_terrain_usage_hash) {
				this->bg_update = get_background_update(this->game, this->palette);
				this->terrain_usage_hash = new_terrain_usage_hash;
			}

			this->engine_coord_data = get_engine_coord_data(this->game);
			this->unit_marker_points = get_marker_points(this->game, this->markers_types, this->player_palette);

			this->game_dirty = false;
			this->need_fresh_bg = false;
		}
	});
}

GameMainLink* Minimap::get_game() const {
	return this->game;
}

void Minimap::set_game(GameMainLink *game) {
	if (this->game != game) {
		this->game = game;
		this->game_dirty = true;
	}
}

QQuickItem* Minimap::get_marker_atlas() const {
	return this->marker_atlas;
}

void Minimap::set_marker_atlas(QQuickItem *marker_atlas) {
	if (this->marker_atlas != marker_atlas) {
		this->marker_atlas = marker_atlas;
		this->marker_atlas_dirty = true;
	}
}

float Minimap::get_marker_atlas_spacing() const {
	return this->marker_atlas_spacing;
}

void Minimap::set_marker_atlas_spacing(float marker_atlas_spacing) {
	if (this->marker_atlas_spacing != marker_atlas_spacing) {
		this->marker_atlas_spacing = marker_atlas_spacing;
		this->marker_atlas_spacing_dirty = true;
	}
}

QVariantList Minimap::get_markers_types() const {
	QVariantList result;

	for (auto &marker_type : this->markers_types)
		result.push_back(QVariant::fromValue(MinimapMarkerAttachedPropertyProvider::minimap_modes_from_gamedata(marker_type)));

	return result;
}

void Minimap::set_markers_types(const QVariantList &markers_types) {
	std::vector<gamedata::minimap_modes> new_markers_types;

	for (auto &marker_type_var : markers_types) {
		bool ok = false;

		const int marker_type_int = marker_type_var.toInt(&ok);

		if (ok) {
			gamedata::minimap_modes marker_type;
			std::tie(marker_type, ok) = MinimapMarkerAttachedPropertyProvider::int_to_minimap_modes_gamedata(marker_type_int);

			if (ok)
				new_markers_types.push_back(marker_type);
		}

		if (!ok)
			qWarning() << "'MinimapImpl.markersTypes' property must be an array of 'MinimapMarker' enum values, but found a" << marker_type_var << "element inside.";
	}

	if (this->markers_types != new_markers_types) {
		this->markers_types = new_markers_types;
		this->marker_atlas_dirty = true;
	}
}

}} // namespace openage::gui

/* vim: set ts=4 sw=4 tw=0 noexpandtab cindent softtabstop=4 :*/
