SET(ADM_LIB ADM_dialogFactoryNone)

SET(${ADM_LIB}_SRCS 
	DIA_dialogFactory.cpp  FAC_toggle.cpp)

ADD_ADM_LIB_CLI_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})