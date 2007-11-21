SET(ADM_LIB ADM_filtersQt4)

SET(${ADM_LIB}_SRCS filter_none.cpp  filter_rsc.cpp)

SET(${ADM_LIB}_Q mainfilter)

Q_RCC(filter)

INCLUDE(adm_qt4)