#AQ_%.cpp : Q_%.cpp %.ui
##############################################################"
# FOR AQ
# Q_foo.cpp and foo.ui => AQ_foo.cpp, ui_foo.h and Q_foo.moc
##############################################################"
MACRO(Q_UIMOCIFY _source)                     
  ADD_CUSTOM_COMMAND(OUTPUT AQ_${_source}.cpp
                     COMMAND  ${QT_UIC_EXECUTABLE} ${_source}.ui -o ui_${_source}.h 
                     COMMAND  ${QT_MOC_EXECUTABLE} Q_${_source}.cpp -o Q_${_source}.moc 
                     COMMAND  echo "#include \"Q_${_source}.cpp\"" > AQ_${_source}.cpp
                     COMMAND  echo "#include \"Q_${_source}.moc\"" >> AQ_${_source}.cpp
                     DEPENDS  ${_source}.ui Q_${_source}.cpp
                     VERBATIM
                )
   #FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/AQ_${_source}.cpp "#include \"Q_${_source}.cpp\" \n" )
   #FILE(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/AQ_${_source}.cpp "#include \"Q_${_source}.moc\" \n" )
ENDMACRO(Q_UIMOCIFY _source)  
##############################################################"
# FOR AT
# T_foo.cpp i => AT_foo.cpp and T_foo.moc
##############################################################"

MACRO(Q_MOCIFY _source)                     
  ADD_CUSTOM_COMMAND(OUTPUT AT_${_source}.cpp
                     COMMAND   ${QT_MOC_EXECUTABLE} T_${_source}.cpp -o T_${_source}.moc 
                     COMMAND  echo "#include \"T_${_source}.cpp\"" > AT_${_source}.cpp
                     COMMAND  echo "#include \"T_${_source}.moc\"" >> AT_${_source}.cpp
                     DEPENDS   T_${_source}.cpp
                     VERBATIM
                )
   #FILE(WRITE AT_${_source}.cpp "#include \"T_${_source}.cpp\" \n" )
   #FILE(APPEND AT_${_source}.cpp "#include \"T_${_source}.moc\" \n" )
ENDMACRO(Q_MOCIFY _source)                     

# Add to our Targets..
FOREACH(_current ${${ADM_LIB}_Q})
 Q_UIMOCIFY(${_current})
 SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} AQ_${_current}.cpp)
ENDFOREACH(_current ${${ADM_LIB}_Q})
FOREACH(_current ${${ADM_LIB}_T})
 Q_MOCIFY(${_current})
 SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} AT_${_current}.cpp)
ENDFOREACH(_current ${${ADM_LIB}_T})


ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)

# EOF




