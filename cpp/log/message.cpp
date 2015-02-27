// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "message.h"

#include "../crossplatform/timing.h"

#include "../util/thread_id.h"
#include "../util/strings.h"

namespace openage {
namespace log {


Message::Message(size_t msg_id,
                 const char *sourcefile,
                 unsigned lineno,
                 const char *functionname,
                 level lvl)
	:
	meta{msg_id, sourcefile, lineno, functionname, lvl, util::current_thread_id.val, timing::get_real_time()} {}


MessageBuilder::MessageBuilder(size_t msg_id,
                               const char *sourcefile,
                               unsigned lineno,
                               const char *functionname,
                               level lvl)
	:
	str_stream{false},
	constructed_message{msg_id, sourcefile, lineno, functionname, lvl} {}


/*
 * Those are just specializations; the general operators are implemented
 * in the header file.
 */
MessageBuilder &MessageBuilder::operator <<(const char *s) {
	return this->str(s);
}


MessageBuilder &MessageBuilder::operator <<(const std::string &s) {
	return this->str(s.c_str());
}


MessageBuilder &MessageBuilder::fmt(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	std::unique_ptr<char[]> str{util::vformat(fmt, ap)};
	this->str(str.get());

	va_end(ap);

	return *this;
}


MessageBuilder &MessageBuilder::str(const char *s) {
	if (this->str_stream.is_acquired()) {
		this->str_stream.stream_ptr->stream << s;
	} else {
		if (this->constructed_message.text.size() > 0) {
			this->constructed_message.text += s;
		} else {
			this->constructed_message.text = s;
		}
	}

	return *this;
}


void MessageBuilder::init_stream_if_necessary() {
	if (this->str_stream.acquire_if_needed()) {
		if (this->constructed_message.text.length() > 0) {
			this->str_stream.stream_ptr->stream << this->constructed_message.text;
		}
	}
}


Message &MessageBuilder::finalize() {
	if (this->str_stream.is_acquired()) {
		this->constructed_message.text = this->str_stream.get();
	}

	return this->constructed_message;
}


std::ostream &operator <<(std::ostream &os, const Message &msg) {
	level_properties props = get_level_properties(msg.meta.lvl);

	os << "\x1b[" << props.colorcode << "m" << std::setw(4) << props.name << "\x1b[m ";
	os << msg.meta.sourcefile << ":" << msg.meta.lineno << " ";
	os << "(" << msg.meta.functionname;
	os << ", thread " << msg.meta.thread_id << ")";
	os << ": " << msg.text;

	return os;
}


}} // namespace openage::log
