SET(ADM_LIB ADM_filtersQt4)

SET(${ADM_LIB}_SRCS filter_none.cpp  filter_rsc.cpp)

SET(${ADM_LIB}_Q mainfilter)

INCLUDE(adm_qt4)

Q_RCC(filter)