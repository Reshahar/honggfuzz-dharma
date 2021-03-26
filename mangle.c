#include "libhfcommon/common.h"
#include "mangle.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "libhfcommon/log.h"
#include "libhfcommon/util.h"

#include <Python.h>
#include <pthread.h>

static bool inited = false;
PyObject *genfunc;
static pthread_mutex_t g_mutex;

void mangle_mangleContent(run_t * run,int speed_factor)
{
   if (!inited) {

         Py_SetProgramName("honggdharma");
         Py_Initialize();

         PyObject *sysmodule = PyImport_ImportModule("sys");
         PyObject *syspath = PyObject_GetAttrString(sysmodule, "path");
         PyList_Append(syspath, PyString_FromString("."));
         Py_DECREF(syspath);
         Py_DECREF(sysmodule);

         PyObject *mod = PyImport_ImportModule("honggdharma");

         if (mod != NULL) {

              genfunc = PyObject_GetAttrString(mod, "generate");

         } else {

            printf("\nUnable to load honggdharma.py!\n");
            exit(1);

         }


         inited = true;

   }
   pthread_mutex_lock(&g_mutex);
   PyObject * result = PyObject_CallObject(genfunc, NULL);
   char *tc = PyString_AsString(result);
   Py_DECREF(result);
   pthread_mutex_unlock(&g_mutex);
   size_t len = strlen(tc);
   if (speed_factor < 5) {}

   if (len > (run->dynfile->size)) {
      return;
   }

   run->dynfile->size = len + 1;

   memmove(&run->dynfile->data[0], tc, len + 1);

}
