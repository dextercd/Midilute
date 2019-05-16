#include <new>
#include <string>
#include <cstdint>
#include <cstdio>
#include <iostream>

#include <alsa/asoundlib.h>
#include <libguile.h>

// owning handle to alsa handle and midi port
struct midi_reader {
	snd_seq_t* handle;
	int port;

	midi_reader(snd_seq_t* h, int p)
		: handle{h}
		, port{p}
	{}

	midi_reader(midi_reader&&) = delete;

	~midi_reader()
	{
		snd_seq_delete_simple_port(handle, port);
		snd_seq_close(handle);
	}
};

SCM midi_reader_type;

midi_reader* scheme_to_midi_reader(SCM scheme_midi_reader)
{
	scm_assert_foreign_object_type(midi_reader_type, scheme_midi_reader);

	auto reader = reinterpret_cast<midi_reader*>(
		scm_foreign_object_ref(scheme_midi_reader, 0));

	return reader;
}

void midi_reader_finalizer(SCM scheme_midi_reader)
{
	auto reader = scheme_to_midi_reader(scheme_midi_reader);
	reader->~midi_reader();
}

void make_midi_reader_type()
{
	auto name = scm_from_utf8_symbol("midi-reader");
	auto slots = scm_list_1(scm_from_utf8_symbol("data"));
	midi_reader_type = scm_make_foreign_object_type(name, slots, midi_reader_finalizer);
}

std::string scheme_to_std_string(SCM str)
{
	std::size_t length;
	auto cstr = scm_to_locale_stringn(str, &length);

	std::string string(cstr, length);

	free(cstr);

	return string;
}

SCM midi_open_reader(SCM scmname)
{
	const auto name = scheme_to_std_string(scmname);

	snd_seq_t* handle;
	const auto open_err = snd_seq_open(&handle, "default", SND_SEQ_OPEN_INPUT, 0);

	if(open_err != 0)
		return SCM_BOOL_F;

	snd_seq_set_client_name(handle, name.c_str());

	const auto port_or_err = snd_seq_create_simple_port(handle, name.c_str(),
		SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);

	if(port_or_err < 0)
		return SCM_BOOL_F;

	const auto port = port_or_err;

	void* reader_data = scm_gc_malloc_pointerless(sizeof(midi_reader), "midi_reader");
	auto reader = new (reader_data) midi_reader(handle, port);

	return scm_make_foreign_object_1(midi_reader_type, reader);
}

SCM midi_blocking_read(SCM scheme_reader)
{
	auto reader = scheme_to_midi_reader(scheme_reader);

	snd_seq_event_t* event = nullptr;

	auto v = snd_seq_event_input(reader->handle, &event);

	// loop until it's a key press event
	while(event->type != SND_SEQ_EVENT_NOTEON || event->data.note.velocity == 0)
		v = snd_seq_event_input(reader->handle, &event);

	return scm_from_int((int)event->data.note.note);
}

SCM midi_non_blocking_read(SCM scheme_reader)
{
	return scm_from_int(1);
}

extern "C" void init()
{
	make_midi_reader_type();

	scm_c_define_gsubr("midi-open-reader",       1, 0, 0, reinterpret_cast<void*>(midi_open_reader));
	scm_c_define_gsubr("midi-blocking-read",     1, 0, 0, reinterpret_cast<void*>(midi_blocking_read));
	scm_c_define_gsubr("midi-non-blocking-read", 1, 0, 0, reinterpret_cast<void*>(midi_non_blocking_read));
}
