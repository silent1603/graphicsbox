#ifndef OPENGL_UTILS
#define OPENGL_UTILS


//#define MAPPING_GL_FUNCTION(X)
//#define MAP(type,name) 
bool loadGLFunction(const char* name);
bool loadAllGLFunction(const char* name);

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
bool loadWGLFunction(const char* name);
bool loadAllWGLFunction(const char* name);
#endif

#if __linux__
bool loadGLXFunction(const char* name);
bool loadALLGLXFunction(const char* name);
#endif
#endif