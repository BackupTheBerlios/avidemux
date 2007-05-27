
SET(ADM_avsproxy_SRCS 
ADM_avsproxy.cpp  ADM_avsproxy_net.cpp)
ADD_LIBRARY(ADM_avsproxy STATIC ${ADM_avsproxy_SRCS})
include_directories(..)
