find_package(PkgConfig)
pkg_check_modules(PC_GBM QUIET gbm)
find_library(GBM_LIBRARIES NAMES gbm HINTS ${PC_GBM_LIBRARY_DIRS})
find_path(GBM_INCLUDE_DIRS gbm.h HINTS ${PC_GBM_INCLUDE_DIRS})

set(GBM_DEFINITIONS ${PC_GBM_CFLAGS_OTHER})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GBM DEFAULT_MSG GBM_INCLUDE_DIRS GBM_LIBRARIES)
mark_as_advanced(GBM_INCLUDE_DIRS GBM_LIBRARIES GBM_DEFINITIONS)
