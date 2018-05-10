// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "registrations.h"

#include <QMetaType>

namespace openage {
namespace gui {

const int reg_path = qRegisterMetaType<util::Path>("util::Path");

}} // openage::gui
