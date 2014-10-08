#include "pyinterface.h"

#include "lzxd.h"

class PyIOWrapper: public IOWrapper {
private:
	PyObject *readfunc, *writefunc;

public:
	PyIOWrapper(PyObject *readfunc, PyObject *writefunc)
			:
			readfunc{readfunc},
			writefunc{writefunc}
	{
		Py_INCREF(readfunc);
		Py_INCREF(writefunc);
	}

	virtual ~PyIOWrapper() {
		Py_DECREF(readfunc);
		Py_DECREF(writefunc);
	}

	virtual ssize_t read(void *buf, size_t size) {
		PyObject *args = Py_BuildValue("(n)", size);
		PyObject *result = PyObject_CallObject(this->readfunc, args);
		Py_XDECREF(args);
		if (result == NULL) {
			return -1;
		}

		Py_buffer py_buf;
		int buf_result = PyObject_GetBuffer(result, &py_buf, PyBUF_SIMPLE);
		Py_DECREF(result);

		if (buf_result < 0) {
			return -1;
		}
		memcpy(buf, py_buf.buf, py_buf.len);
		ssize_t read_bytes = py_buf.len;
		PyBuffer_Release(&py_buf);

		return read_bytes;
	}

	virtual ssize_t write(const void *buf, size_t size) {
		PyObject *args   = Py_BuildValue("(y#)", buf, size);
		PyObject *result = PyObject_CallObject(writefunc, args);
		Py_XDECREF(args);
		if (result == NULL) {
			return -1;
		}

		ssize_t write_count;
		if (PyLong_Check(result)) {
			write_count = PyLong_AsLong(result);
		} else {
			write_count = -1;
		}
		Py_DECREF(result);

		if (write_count != (ssize_t) size) {
			return -1;
		}

		return write_count;
	}
};

PyObject *lzxd_decompress(PyObject * /*self*/, PyObject *args) {
	// TODO: instead of returning NULL, raise python exceptions on error!

	unsigned window_bits;
	off_t output_length;
	PyObject *readfunc;
	PyObject *writefunc;

	if (not PyArg_ParseTuple(args, "InOO", &window_bits, &output_length, &readfunc, &writefunc)) {
		return NULL;
	}

	if (!PyCallable_Check(readfunc)) {
		return NULL;
	}
	if (!PyCallable_Check(writefunc)) {
		return NULL;
	}

	try {
		PyIOWrapper io_wrapper{readfunc, writefunc};
		decompress(window_bits, 0, 4096, output_length, &io_wrapper);
	} catch (char *error) {
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lzxd_methods[] = {
	{"decompress", &lzxd_decompress, METH_VARARGS, "decompresses a LZX blob; calls readfunc for new data, and writefunc to output decompressed data."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef lzxd_module = {
	PyModuleDef_HEAD_INIT,
	"lzxd",   // module name
	NULL,     // doc string
	-1,       // -1 -> global state
	lzxd_methods,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_lzxd() {
	PyObject *module = PyModule_Create(&lzxd_module);
	return module;
}

