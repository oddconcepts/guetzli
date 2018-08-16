#define PY_SSIZE_T_CLEAN
//#include <Python.h>
#include <Python.h>
#include "py3.h"

#include "guetzli/processor.h"
#include "guetzli/quality.h"

#include "clguetzli/clguetzli.h"

PyObject* RgbToJpeg(PyObject* self, PyObject* args);
PyObject* JpegToJpeg(PyObject* self, PyObject* args);

static PyMethodDef methods[] =
{
    {"RgbToJpeg", RgbToJpeg, METH_VARARGS, "RgbToJpeg"},
    {"JpegToJpeg", JpegToJpeg, METH_VARARGS, "JpegToJpeg"},
    {NULL, NULL}
};

PyObject* RgbToJpeg(PyObject* self, PyObject* args) {
  guetzli::Params params;
  guetzli::ProcessStats stats;

  int width, height;

  Py_buffer rgb_info;
  int quality;
  std::string out;

  uint8_t* tmp;
  int tmp_size;

  if (!PyArg_ParseTuple(args, PY_ARG_BUFFER "iii",
          &rgb_info, &width, &height, &quality)) {
    return NULL;
  }

  params.butteraugli_target = static_cast<float>(
      guetzli::ButteraugliScoreForQuality(quality));
  
  tmp = (uint8_t*)rgb_info.buf;
  tmp_size = rgb_info.len;

  std::vector<uint8_t> rgb(tmp, tmp + tmp_size);

  if (!guetzli::Process(params, &stats, rgb, width, height, &out)) {
    Py_RETURN_NONE;
  }

  return PyBytes_FromStringAndSize((char*)out.data(), out.size());
}

PyObject* JpegToJpeg(PyObject* self, PyObject* args) {
  guetzli::Params params;
  guetzli::ProcessStats stats;

  Py_buffer src_info;
  int quality;

  std::string out;

  uint8_t* tmp;
  int tmp_size;

  if (!PyArg_ParseTuple(args, PY_ARG_BUFFER "i",
          &src_info, &quality)) {
    return NULL;
  }

  g_mathMode = MODE_CUDA;

  params.butteraugli_target = static_cast<float>(
      guetzli::ButteraugliScoreForQuality(quality));
  
  tmp = (uint8_t*)src_info.buf;
  tmp_size = src_info.len;

  std::string src(tmp, tmp + tmp_size);

  if (!guetzli::Process(params, &stats, src, &out)) {
    Py_RETURN_NONE;
  }

  return PyBytes_FromStringAndSize((char*)out.data(), out.size());
}

#if PY_VERSION_HEX >= 0x03000000
PyMODINIT_FUNC
PyInit__guetzli(void) {
    static PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "_guetzli",         /* m_name */
        NULL,               /* m_doc */
        -1,                 /* m_size */
        methods,            /* m_methods */
    };

    return PyModule_Create(&module_def);
}
#else
PyMODINIT_FUNC
init_guetzli(void)
{
    Py_InitModule("_guetzli", methods);
}
#endif

